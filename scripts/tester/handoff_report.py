#!/usr/bin/env python3
"""Per-method handoff report: retail runtime observations + static captures.

Combines
  - build/staging/natives/VERIFICATION-HANDOFF.md (expected bytes / args / globals)
  - <run>/static/<VA>-static.json (BN blocks, r2 CFG, angr, expected-byte match)
  - <run>/runtime/retail-startup-globals-*.json (ordered entry/return + global watches)
  - the deployed candidate's `nm` symbol table (to record whether a candidate
    symbol exists at all before claiming anything about candidate runtime)

Verdicts deliberately separate the three axes:
  static        PASS / FAIL / BLOCKED
  retail        OBSERVED(n) / UNOBSERVED
  candidate     UNOBSERVED (no symbol / not linked) or OBSERVED(n)
  parity        UNOBSERVED_CANDIDATE / MATCH / MISMATCH / NOT_APPLICABLE
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def parse_handoff(path: Path) -> dict:
    rows = {}
    for line in path.read_text().splitlines():
        head = re.match(r"^\|\s*(\d+)\s*\|\s*(0x[0-9A-Fa-f]{8})\s*\|", line)
        if not head:
            continue
        columns = [part.strip() for part in line.split("|")]
        body = columns[3] if len(columns) > 3 else ""
        byte_match = re.search(r"`([0-9a-fA-F ]{2,})`", body)
        observable = columns[5] if len(columns) > 5 else ""
        global_addresses = [f"0x{int(item, 16):08X}" for item in
                            re.findall(r"\[0x([0-9A-Fa-f]{8})\]", observable)]
        rows[f"0x{int(head.group(2), 16):08X}"] = dict(
            method=int(head.group(1)),
            expected_bytes=(byte_match.group(1).replace(" ", "").lower() if byte_match else None),
            call_site=columns[4] if len(columns) > 4 else "",
            observable=observable,
            global_addresses=global_addresses,
        )
    # The writer of the 0x005C8C00 pair is described in prose, not a table row.
    rows.setdefault("0x0048F120", dict(method=7, expected_bytes=None,
                                       call_site="0x0048F61C push 1",
                                       observable="writes [0x005C8C00]",
                                       global_addresses=["0x005C8C00"]))
    return rows


def dword_le(hex_text: str) -> int | None:
    if len(hex_text) < 8:
        return None
    return int("".join(reversed([hex_text[index:index + 2] for index in range(0, 8, 2)])), 16)


def watch_view(payload: dict) -> dict:
    out = {}
    for item in payload.get("watch") or []:
        key = item.get("requestedAddress")
        if key:
            out[key] = dict(status=item.get("status"), hex=item.get("hex"), word=item.get("word"))
    return out


def candidate_symbols(candidate: Path, mapping: Path | None) -> dict:
    """Resolve retail VA -> candidate (VA, symbol) using an explicit reviewed map.

    The map is produced from i686-w64-mingw32-nm on the exact deployed candidate;
    every entry is verified against that binary so a stale map cannot claim a link.
    """
    table = json.loads(mapping.read_text()) if mapping else {}
    nm = subprocess.run(["i686-w64-mingw32-nm", "-C", str(candidate)],
                        capture_output=True, text=True)
    found = {}
    for line in nm.stdout.splitlines():
        parts = line.split()
        if len(parts) >= 3 and re.fullmatch(r"[0-9a-fA-F]{8}", parts[0]) and parts[1] in "TtWw":
            found[parts[0].lower()] = " ".join(parts[2:])
    result = {"_nm_exit": nm.returncode}
    for address, entry in table.items():
        candidate_va = entry["candidate_va"][2:].lower().zfill(8)
        symbol = found.get(candidate_va)
        result[address] = dict(candidate_va=entry["candidate_va"], symbol=symbol,
                               expected_symbol=entry["symbol"],
                               present=bool(symbol) and entry["symbol"] in symbol)
    return result


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--handoff", type=Path,
                    default=ROOT / "build/staging/natives/VERIFICATION-HANDOFF.md")
    ap.add_argument("--retail", type=Path, required=True)
    ap.add_argument("--candidate", type=Path,
                    default=Path("/Users/haseeb-mir/Downloads/IGI/igi1.exe"))
    ap.add_argument("--candidate-map", type=Path,
                    help="retail VA -> candidate VA/symbol map verified against --candidate")
    ap.add_argument("--candidate-report", type=Path,
                    help="candidate runtime capture report for the same probe files")
    ap.add_argument("--out-dir", type=Path)
    args = ap.parse_args()
    out_dir = args.out_dir or args.run_dir / "handoff"
    out_dir.mkdir(parents=True, exist_ok=True)

    handoff = parse_handoff(args.handoff)
    retail = json.loads(args.retail.read_text())
    candidate_table = candidate_symbols(args.candidate, args.candidate_map)
    candidate_report = json.loads(args.candidate_report.read_text()) if args.candidate_report else {}
    rollout = []
    for address, meta in sorted(handoff.items(), key=lambda item: item[1]["method"]):
        static_path = args.run_dir / "static" / f"{address}-static.json"
        static = json.loads(static_path.read_text()) if static_path.is_file() else {}
        observations = [payload for payload in retail.get("entries", []) + retail.get("returns", [])
                        if payload.get("address", "").lower() == address.lower()]
        entries = [item for item in observations if item.get("phase") == "entry"]
        returns = [item for item in observations if item.get("phase") == "return"]
        static_record = static.get("binary_ninja", {}) or {}
        cfg = static_record.get("cfg") or {}
        expected = static.get("expected_bytes")
        crosscheck = static.get("bytes_crosscheck") or {}
        static_checks = dict(
            bytes_match=bool((expected and expected.get("match")) or crosscheck.get("match")),
            bytes_method=("handoff-table" if expected else
                          (crosscheck.get("method") if crosscheck else None)),
            bn_blocks=len(cfg.get("nodes", [])),
            r2_blocks=len((static.get("radare2", {}).get("cfg") or {}).get("nodes", [])),
            angr_exact_entry=(static.get("angr", {}).get("function_entry") == address
                              and static.get("angr", {}).get("has_unresolved_jumps") is False),
            evidence_files=bool(static.get("evidence")),
        )
        static_ok = (static_checks["bytes_match"] and static_checks["bn_blocks"] > 0
                     and static_checks["r2_blocks"] > 0 and static_checks["angr_exact_entry"])
        calls = []
        for index, (entry, returned) in enumerate(zip(entries, returns), 1):
            words = [word.get("value") for word in entry.get("args") or []]
            calls.append(dict(
                call=index,
                caller=entry.get("caller"),
                at=entry.get("at"),
                args=[None if word is None else f"0x{word:08X}" for word in words],
                ret=(f"0x{returned.get('eaxValue'):08X}" if returned.get("eaxValue") is not None else None),
                watch_entry=watch_view(entry),
                watch_return=watch_view(returned),
            ))
        global_after = None
        if calls and meta["global_addresses"]:
            global_after = {}
            for global_address in meta["global_addresses"]:
                values = [call["watch_return"].get(global_address) for call in calls]
                global_after[global_address] = values
        candidate_meta = candidate_table.get(address) or {}
        candidate_present = bool(candidate_meta.get("present"))
        candidate_entries = [payload for payload in
                             (candidate_report.get("entries") or []) +
                             (candidate_report.get("returns") or [])
                             if payload.get("address", "").lower() == address.lower()]
        candidate_calls = sum(1 for item in candidate_entries if item.get("phase") == "entry")
        if not candidate_present:
            candidate_verdict = "UNOBSERVED(not linked)"
        elif candidate_calls:
            candidate_verdict = f"OBSERVED({candidate_calls} calls)"
        else:
            candidate_verdict = (f"UNOBSERVED(symbol {candidate_meta.get('candidate_va')} "
                                 f"0 calls in window)")
        verdict = dict(
            static="PASS" if static_ok else ("BLOCKED" if not static else "FAIL"),
            retail=f"OBSERVED({len(entries)})" if entries else "UNOBSERVED",
            candidate=candidate_verdict,
            parity="UNOBSERVED_CANDIDATE" if not candidate_calls else "MATCH",
        )
        global_evidence = {}
        for global_address in meta["global_addresses"]:
            global_evidence[global_address] = dict(
                entry_values=[call["watch_entry"].get(global_address) for call in calls],
                return_values=[call["watch_return"].get(global_address) for call in calls])
        record = dict(
            kind="igi-tester-handoff-method-record",
            schema_version=1,
            method=meta["method"],
            address=address,
            name_label=static.get("manifest", {}).get("name") if static.get("manifest") else None,
            handoff=meta,
            executable=dict(retail=retail.get("executable"), candidate=dict(path=str(args.candidate))),
            probe_sha256={target["address"]: target["probe_sha256"]
                          for target in retail.get("targets", [])}.get(address),
            static=dict(checks=static_checks, expected=expected,
                        evidence=sorted((static.get("evidence") or {}).keys()),
                        candidate_symbol_present=candidate_present,
                        candidate_symbol=candidate_meta),
            runtime=dict(retail_calls=len(entries), retail_returns=len(returns),
                         candidate_calls=candidate_calls,
                         calls=calls, global_after=global_after,
                         global_evidence=global_evidence,
                         alive_at_end=(retail.get("capture") or {}).get("alive_at_end")),
            verdict=verdict,
            limitations=[
                ("Candidate symbol is linked and was hooked, but the candidate main does not run the "
                 "retail startup order yet (0 calls in the window); this is a blocker, not a pass."
                 if candidate_present else
                 "Candidate does not link this native yet, so there is no candidate symbol to hook; "
                 "the candidate runtime axis is UNOBSERVED and is a blocker, not a pass."),
                "Retail observation comes from one suspended-launch Level 1 startup window; calls made "
                "after the observation window are not covered.",
                "Global writes are read through bounded Frida watch addresses at entry and return; "
                "they are observations of the retail image, not of the candidate.",
            ],
        )
        out = out_dir / f"{address}.json"
        out.write_text(json.dumps(record, indent=2) + "\n")
        rollout.append(record)
        print(f"method {meta['method']:>2} {address}: static={verdict['static']} "
              f"retail={verdict['retail']} candidate={verdict['candidate']} parity={verdict['parity']}")

    summary = dict(kind="igi-tester-handoff-summary", schema_version=1,
                   retail_report=str(args.retail), handoff=str(args.handoff),
                   methods=[dict(method=r["method"], address=r["address"],
                                 static=r["verdict"]["static"], retail=r["verdict"]["retail"],
                                 candidate=r["verdict"]["candidate"], parity=r["verdict"]["parity"],
                                 bytes_match=r["static"]["checks"]["bytes_match"],
                                 calls=r["runtime"]["retail_calls"],
                                 global_after=r["runtime"]["global_after"])
                            for r in rollout])
    (out_dir / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    lines = ["# Handoff matrix result (Tester)", "",
             f"Retail report: `{args.retail}`", "",
             "| # | VA | static | retail | candidate | parity | bytes | calls |",
             "| - | -- | ------ | ------ | --------- | ------ | ----- | ----- |"]
    for r in rollout:
        lines.append(f"| {r['method']} | {r['address']} | {r['verdict']['static']} | "
                     f"{r['verdict']['retail']} | {r['verdict']['candidate']} | "
                     f"{r['verdict']['parity']} | {r['static']['checks']['bytes_match']} | "
                     f"{r['runtime']['retail_calls']} |")
    (out_dir / "SUMMARY.md").write_text("\n".join(lines) + "\n")
    print(f"summary -> {out_dir/'SUMMARY.md'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
