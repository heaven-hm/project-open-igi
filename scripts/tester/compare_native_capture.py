#!/usr/bin/env python3
"""Ordered comparison of a retail vs candidate Frida probe capture.

Both inputs are reports produced by scripts/tester/frida_probe_driver.py, using
the SAME probe files. The comparison is per target address, ordinal call by
ordinal call:

  - call order (entry sequence index)
  - every recorded stack argument word
  - the return value (EAX) when recorded
  - bounded memory snapshots (entry pointer argument and return-pointer)
  - call/return balance

It reports the exact first divergence and never turns missing candidate calls
into a match. Verdicts:
  MATCH                     same count and every compared field equal
  MISMATCH                  first difference at a concrete call ordinal
  UNOBSERVED_CANDIDATE      retail observed calls, candidate observed none
  UNOBSERVED_RETAIL         candidate observed calls, retail observed none
  BOTH_UNOBSERVED           no calls on either side
  INCOMPLETE                call/return balance broken on one side
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path


def load(path: Path) -> dict:
    return json.loads(path.read_text())


def by_target(report: dict) -> dict:
    grouped: dict[str, list[dict]] = {}
    for payload in report.get("entries", []) + report.get("returns", []):
        grouped.setdefault(payload.get("address"), []).append(payload)
    for address in grouped:
        grouped[address].sort(key=lambda item: (item.get("callIndex") or 0,
                                                item.get("phase") != "entry"))
    return grouped


def memory_view(payload: dict):
    memory = payload.get("memory")
    return memory.get("snapshot") if memory else None


def return_memory_view(payload: dict):
    memory = payload.get("returnMemory")
    return memory.get("snapshot") if memory else None


def field_digest(payload: dict) -> dict:
    return dict(
        args=[word.get("hex") for word in payload.get("args") or []],
        memory=(memory_view(payload) or {}).get("hex"),
        memory_status=(memory_view(payload) or {}).get("status"),
        eax=payload.get("eax"),
        return_memory=(return_memory_view(payload) or {}).get("hex"),
        return_memory_status=(return_memory_view(payload) or {}).get("status"),
    )


def pointer_snapshot_equal(left: dict, right: dict, index: int, string_mode: bool = False) -> bool:
    """Typed pointer comparison: equal only when both sides read the SAME bytes.

    Raw pointer values are never comparable across processes. When both captures
    snapshotted the pointer argument at `index` and the bytes match, the argument
    is treated as equal; otherwise the raw words are compared (fail closed).
    """
    left_memory = left.get("memory") or {}
    right_memory = right.get("memory") or {}
    if left_memory.get("argumentIndex") != index or right_memory.get("argumentIndex") != index:
        return False
    left_snapshot = left_memory.get("snapshot") or {}
    right_snapshot = right_memory.get("snapshot") or {}
    if not (left_snapshot.get("status") == "read" and right_snapshot.get("status") == "read"
            and left_snapshot.get("hex") is not None):
        return False
    left_hex, right_hex = left_snapshot.get("hex"), right_snapshot.get("hex")
    if string_mode:
        left_hex, right_hex = c_string_hex(left_hex), c_string_hex(right_hex)
    return left_hex == right_hex


def c_string_hex(hex_text: str | None) -> str | None:
    """Prefix of a byte snapshot up to and including the first NUL byte."""
    if hex_text is None:
        return None
    for index in range(0, len(hex_text) - 1, 2):
        if hex_text[index:index + 2] == "00":
            return hex_text[:index + 2]
    return hex_text


def memory_text(payload: dict, key: str, string_mode: bool) -> str | None:
    snapshot = ((payload.get(key) or {}).get("snapshot") or {})
    data = snapshot.get("hex")
    if data is None:
        return None
    if not string_mode:
        return data
    if snapshot.get("status") == "read":
        return c_string_hex(data)
    return data


def return_pointer_snapshot_equal(left_return: dict, right_return: dict) -> bool:
    left_snapshot = (left_return.get("returnMemory") or {}).get("snapshot") or {}
    right_snapshot = (right_return.get("returnMemory") or {}).get("snapshot") or {}
    return (left_snapshot.get("status") == "read" and right_snapshot.get("status") == "read"
            and left_snapshot.get("hex") is not None
            and left_snapshot.get("hex") == right_snapshot.get("hex"))


def compare_target(address: str, retail: list[dict], candidate: list[dict],
                   default_args: int | None = None,
                   address_args: dict[str, int] | None = None,
                   string_addresses: set[str] | None = None) -> dict:
    retail_entries = [item for item in retail if item.get("phase") == "entry"]
    candidate_entries = [item for item in candidate if item.get("phase") == "entry"]
    retail_returns = [item for item in retail if item.get("phase") == "return"]
    candidate_returns = [item for item in candidate if item.get("phase") == "return"]
    result = dict(
        address=address,
        retail_entries=len(retail_entries),
        retail_returns=len(retail_returns),
        candidate_entries=len(candidate_entries),
        candidate_returns=len(candidate_returns),
        matched_prefix=0,
        first_divergence=None,
    )
    if len(retail_entries) != len(retail_returns):
        result["verdict"] = "INCOMPLETE"
        result["first_divergence"] = dict(side="retail", reason="entry/return count mismatch")
        return result
    if len(candidate_entries) != len(candidate_returns):
        result["verdict"] = "INCOMPLETE"
        result["first_divergence"] = dict(side="candidate", reason="entry/return count mismatch")
        return result
    if not retail_entries and not candidate_entries:
        result["verdict"] = "BOTH_UNOBSERVED"
        return result
    if retail_entries and not candidate_entries:
        result["verdict"] = "UNOBSERVED_CANDIDATE"
        result["first_divergence"] = dict(side="candidate", reason="no calls observed at all",
                                          retail_first=field_digest(retail_entries[0]))
        return result
    if candidate_entries and not retail_entries:
        result["verdict"] = "UNOBSERVED_RETAIL"
        return result
    string_mode = bool(string_addresses) and address in string_addresses
    arg_limit = default_args
    if address_args and address in address_args:
        arg_limit = address_args[address]
    limit = min(len(retail_entries), len(candidate_entries))
    for index in range(limit):
        left, right = field_digest(retail_entries[index]), field_digest(candidate_entries[index])
        left_return = field_digest(retail_returns[index]) if index < len(retail_returns) else {}
        right_return = field_digest(candidate_returns[index]) if index < len(candidate_returns) else {}
        differing = []
        if arg_limit is None:
            if left["args"] != right["args"]:
                differing.append("args")
        else:
            left_args = (retail_entries[index].get("args") or [])
            right_args = (candidate_entries[index].get("args") or [])
            for word_index in range(min(arg_limit, len(left_args), len(right_args))):
                if (left["args"][word_index] != right["args"][word_index]
                        and not pointer_snapshot_equal(retail_entries[index],
                                                       candidate_entries[index], word_index,
                                                       string_mode)):
                    differing.append(f"args[{word_index}]")
        left_memory = memory_text(retail_entries[index], "memory", string_mode)
        right_memory = memory_text(candidate_entries[index], "memory", string_mode)
        left_return_memory = memory_text(
            retail_returns[index] if index < len(retail_returns) else {}, "returnMemory", string_mode)
        right_return_memory = memory_text(
            candidate_returns[index] if index < len(candidate_returns) else {}, "returnMemory", string_mode)
        for key in ("memory", "memory_status", "eax", "return_memory", "return_memory_status"):
            if key == "memory":
                same = left_memory == right_memory
            elif key == "return_memory":
                same = left_return_memory == right_return_memory
            else:
                same = left[key] == right.get(key)
            if same:
                continue
            if key == "eax" and return_pointer_snapshot_equal(
                    retail_returns[index] if index < len(retail_returns) else {},
                    candidate_returns[index] if index < len(candidate_returns) else {}):
                continue
            differing.append(key if key in ("memory", "memory_status", "eax") else f"return.{key}")
        if differing:
            result["first_divergence"] = dict(
                side="candidate", call_ordinal=index + 1, fields=differing,
                retail=dict(entry=left, return_=left_return),
                candidate=dict(entry=right, return_=right_return))
            result["verdict"] = "MISMATCH"
            return result
        result["matched_prefix"] = index + 1
    if len(retail_entries) != len(candidate_entries):
        result["verdict"] = "MISMATCH"
        result["first_divergence"] = dict(
            side="candidate", call_ordinal=limit + 1,
            reason="call count differs",
            retail_entries=len(retail_entries), candidate_entries=len(candidate_entries))
        return result
    result["verdict"] = "MATCH"
    return result


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--retail", type=Path, required=True)
    ap.add_argument("--candidate", type=Path, required=True)
    ap.add_argument("--out", type=Path)
    ap.add_argument("--default-args", type=int, default=None,
                    help="compare only the first N stack argument words (default: every recorded word)")
    ap.add_argument("--address-args", action="append", default=[], metavar="ADDRESS=N",
                    help="override --default-args for one retail target address; repeatable")
    ap.add_argument("--string-address", action="append", default=[], metavar="ADDRESS",
                    help="compare this target's memory snapshots as NUL-terminated strings; repeatable")
    args = ap.parse_args()
    string_addresses = {f"0x{int(item, 0):08X}" for item in args.string_address} or None
    address_args = {}
    for item in args.address_args:
        address_text, _, words_text = item.partition("=")
        if not address_text or not words_text.isdigit():
            raise SystemExit(f"--address-args must be ADDRESS=N: {item}")
        address_args[f"0x{int(address_text, 0):08X}"] = int(words_text)
    retail_report, candidate_report = load(args.retail), load(args.candidate)
    if retail_report.get("executable", {}).get("sha256") == candidate_report.get("executable", {}).get("sha256"):
        raise SystemExit("retail and candidate reports point at identical executable sha256")
    probes = {target["probe_sha256"] for target in retail_report.get("targets", [])}
    candidate_probes = {target["probe_sha256"] for target in candidate_report.get("targets", [])}
    if probes != candidate_probes:
        raise SystemExit("probe set differs between retail and candidate captures")
    grouped_retail, grouped_candidate = by_target(retail_report), by_target(candidate_report)
    addresses = sorted(set(grouped_retail) | set(grouped_candidate))
    comparison = dict(
        kind="igi-tester-native-probe-comparison",
        schema_version=1,
        retail=dict(report=str(args.retail), executable=retail_report["executable"],
                    captured_at=retail_report.get("captured_at")),
        candidate=dict(report=str(args.candidate), executable=candidate_report["executable"],
                       captured_at=candidate_report.get("captured_at")),
        args_comparison=dict(default_args=args.default_args,
                             address_args=address_args or None,
                             string_addresses=sorted(string_addresses) if string_addresses else None),
        targets=[compare_target(address, grouped_retail.get(address, []),
                                grouped_candidate.get(address, []),
                                default_args=args.default_args,
                                address_args=address_args,
                                string_addresses=string_addresses) for address in addresses],
    )
    comparison["verdict"] = ("MATCH" if comparison["targets"] and
                             all(t["verdict"] == "MATCH" for t in comparison["targets"]) else "NOT_MATCH")
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(json.dumps(comparison, indent=2) + "\n")
    for target in comparison["targets"]:
        print(f"{target['address']}: {target['verdict']} "
              f"retail={target['retail_entries']} candidate={target['candidate_entries']} "
              f"prefix={target['matched_prefix']}")
        if target["first_divergence"]:
            print(f"  first divergence: {json.dumps(target['first_divergence'])[:400]}")
    print(f"overall: {comparison['verdict']} -> {args.out}")
    return 0 if comparison["verdict"] == "MATCH" else 1


if __name__ == "__main__":
    raise SystemExit(main())
