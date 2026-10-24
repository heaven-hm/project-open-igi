#!/usr/bin/env python3
"""Build per-native Tester records for the P05c parser/tokenizer/handler set.

Inputs are the tester's own static capture (`static/<VA>-static.json`) and the
retail runtime captures taken in the four request scenarios. Expectations come
from build/audit/p05c-20261023T223344Z (Lead static packet) and the staged
transcription build/staging/natives/app_main_commands.cpp.

No status is promoted: static PASS and runtime OBSERVED are recorded per axis,
and candidate stays UNOBSERVED until the symbol is linked and the candidate main
executes the retail startup order.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
STAGED = ROOT / "build/staging/natives/app_main_commands.cpp"

NATIVES = {
    "0x0048F360": dict(
        name="AppMain_ParseCmdLineArgs_48F360", kind="parser", blocks=22, size=286,
        scenarios=["retail-parser-window-level1"]),
    "0x004A5A0C": dict(
        name="AppMain_TokenizeDelimited_4A5A0C", kind="tokenizer", blocks=13, size=156,
        scenarios=["retail-parser-window-level1"]),
    "0x0048F350": dict(
        name="AppMain_WindowHandler_48F350", kind="handler", blocks=1, size=9,
        scenarios=["retail-handlers-window", "retail-handlers-wininput",
                   "retail-handlers-germany", "retail-handlers-usa"]),
    "0x0048F920": dict(
        name="AppMain_WinInputHandler_48F920", kind="handler", blocks=1, size=9,
        scenarios=["retail-handlers-wininput"]),
    "0x0048F930": dict(
        name="AppMain_GermanyHandler_48F930", kind="handler", blocks=1, size=21,
        scenarios=["retail-handlers-germany"]),
    "0x0048F950": dict(
        name="AppMain_USAHandler_48F950", kind="handler", blocks=1, size=12,
        scenarios=["retail-handlers-usa"]),
    "0x0048F960": dict(
        name="AppMain_HardcoreGermanHandler_48F960", kind="hardcore", blocks=1, size=9,
        scenarios=["retail-handlers-hardcoregerman-token", "retail-handlers-hardcoregerman"]),
    "0x00410360": dict(
        name="HumanPlayer_UnlimitedAmmoHandler_410360", kind="unlimited", blocks=1, size=8,
        scenarios=["retail-handlers-unlimitedammo-token", "retail-handlers-hardcoregerman"]),
}

# Staged-source review findings (Tester read of build/staging/natives/app_main_commands.cpp
# against the live BN pseudo C / disassembly captured in the same run).
STAGED_REVIEW = {
    "0x0048F360": [
        "matches BN: frame 0x208 with a 0x200 stack copy of the command line (strlen+1 memcpy)",
        "matches BN: tokenizer called with \" \t\", tokens shorter than the option name skipped, "
        "a-z folded to A-Z for a case-insensitive first-strlen(name) comparison",
        "matches BN: handler called with token + strlen(name); the handler return is discarded",
        "matches BN: returns the final tokenizer result (0 at end of line)",
        "runtime adds two invocations the handoff did not list: UnlimitedAmmo (caller 0x41006B, "
        "handler 0x410360) and HardcoreGerman (caller main 0x48F7C3, handler 0x48F960)",
    ],
    "0x004A5A0C": [
        "matches BN: 0x20-byte delimiter bitmap built from the delimiter string including its NUL; "
        "leading delimiters skipped, stop at NUL; scan stops on a delimiter, that byte is zeroed, "
        "the scan position advances past it",
        "matches BN: arg1==NULL continues from the static continuation pointer; an empty token "
        "returns 0 (the (start - position) & start trick)",
        "documented substitution: retail stores the continuation pointer in image global 0x00936060; "
        "the staged source keeps the same role in a file-scope pointer because that absolute address "
        "is not mapped in the candidate image - behavior-preserving for a single process, checked "
        "by the re-entrant call pattern observed at runtime",
    ],
    "0x0048F350": [
        "matches BN: discards the suffix pointer, pushes 0, calls the screen-mode setter, returns its result",
        "runtime confirms the handler entry (suffix pointer word recorded) and the resulting "
        "SetScreenMode(0) call in the same window-level1 startup",
    ],
    "0x0048F920": [
        "matches BN: discards the suffix pointer, pushes 1, calls the Win32-input setter, returns its result",
        "runtime confirms handler entry, return 1, and the following Input_SetUseWin32Input(1) call",
    ],
    "0x0048F930": [
        "matches BN: pushes the \"Germany\" literal, calls the language setter, pushes 1, calls the "
        "German-version setter, returns that result",
        "runtime confirms handler return 1, SetLanguage(0x00541D60=\"Germany\") return 8 and "
        "SetGermanVersion(1) return 1, each from inside the handler",
    ],
    "0x0048F950": [
        "matches BN: pushes the 0x005382C8 \"USA\" literal, calls the language setter, returns its result",
        "runtime confirms handler return 4 and SetLanguage(0x005382C8=\"USA\") return 4 from inside the handler",
    ],
    "0x0048F960": [
        "matches BN and the staged body: 9 bytes 6a00e849f9ffff59c3 = `push 0; call 0x0048F2B0; pop ecx; retn`",
        "runtime confirms execution only when the command line carries the HardcoreGerman token: handler entry "
        "1 call with an empty suffix, EAX 0, followed by SetGermanVersion(0); [0x005C8BDC] stays 0",
    ],
    "0x00410360": [
        "live BN and r2 agree on 8 bytes c60514e2560001c3 = `mov byte [0x0056E214], 1; retn`; the staged "
        "app_main_commands.cpp does not contain this handler yet, so the staged-vs-live comparison covers the "
        "five main-startup handlers only",
        "runtime confirms the byte write [0x0056E214] 0->1 and an unchanged EAX when the UnlimitedAmmo token is "
        "present; unobserved in plain `window level1` (later-phase HumanPlayer_Init dependency, not main startup)",
    ],
}

# Expected registrations captured from main + Game_Init in `window level1`.
PARSER_EXPECTED = [
    ("Window", "0x0048F350", "0x48F6AB"),
    ("WinInput", "0x0048F920", "0x48F6BA"),
    ("Germany", "0x0048F930", "0x48F6C9"),
    ("USA", "0x0048F950", "0x48F6D8"),
    ("Level", "0x00415B10", "0x41553F"),
]


def text_of(hex_text: str | None) -> str | None:
    if not hex_text:
        return None
    try:
        return bytes.fromhex(hex_text).split(b"\x00")[0].decode("ascii", "replace")
    except ValueError:
        return None


def load_report(path: Path) -> dict:
    report = json.loads(path.read_text())
    report["_by_address"] = {}
    returns = {(r["address"], r["callIndex"]): r for r in report.get("returns", [])}
    for entry in report.get("entries", []):
        record = returns.get((entry["address"], entry["callIndex"]), {})
        snapshot = (entry.get("memory") or {}).get("snapshot") or {}
        report["_by_address"].setdefault(entry["address"], []).append(dict(
            callIndex=entry.get("callIndex"), at=entry.get("at"), caller=entry.get("caller"),
            args=[word.get("value") for word in entry.get("args") or []],
            arg0_text=text_of(snapshot.get("hex")), arg0_hex=snapshot.get("hex"),
            arg0_status=snapshot.get("status"),
            ret=record.get("eaxValue")))
    report["_executable"] = (report.get("executable") or {}).get("sha256")
    report["_watch_by_address"] = {}
    for entry in report.get("entries", []):
        record = returns.get((entry["address"], entry["callIndex"]), {})
        report["_watch_by_address"].setdefault(entry["address"], []).append(
            {item.get("requestedAddress"): item.get("word") for item in record.get("watch") or []})
    return report


def runtime_checks(native: dict, reports: dict) -> dict:
    kind = native["kind"]
    checks = []
    details = {}
    if kind == "parser":
        calls = reports["retail-parser-window-level1"]["_by_address"].get("0x0048F360", [])
        details["calls"] = calls
        found = []
        for expected_name, expected_handler, expected_caller in PARSER_EXPECTED:
            match = next((call for call in calls
                          if call["arg0_text"] == expected_name
                          and len(call["args"]) > 1
                          and f"0x{call['args'][1]:08X}" == expected_handler
                          and call["caller"] == expected_caller.lower()), None)
            found.append(dict(name=expected_name, handler=expected_handler,
                              caller=expected_caller, matched=bool(match),
                              call_index=match["callIndex"] if match else None,
                              ret=f"0x{match['ret']:08X}" if match and match["ret"] is not None else None))
        ordered = [item["call_index"] for item in found if item["call_index"]]
        checks.append(dict(check="five_registrations_with_handlers", passed=all(i["matched"] for i in found)))
        checks.append(dict(check="registration_order", passed=ordered == sorted(ordered) and len(ordered) == 5))
        checks.append(dict(check="all_returns_zero",
                           passed=all(call["ret"] == 0 for call in calls)))
        checks.append(dict(check="two_extra_invocations_recorded",
                           passed=len(calls) >= 7))
        details["expected"] = found
    elif kind == "tokenizer":
        calls = reports["retail-parser-window-level1"]["_by_address"].get("0x004A5A0C", [])
        starts = [call for call in calls if call["args"] and call["args"][0]]
        conts = [call for call in calls if call["args"] and not call["args"][0]]
        checks.append(dict(check="start_calls_with_string", passed=bool(starts)))
        checks.append(dict(check="continuation_calls_with_null_state", passed=bool(conts)))
        checks.append(dict(check="continuations_end_with_zero", passed=any(call["ret"] == 0 for call in conts)))
        details["calls"] = calls
        details["start_calls"] = len(starts)
        details["continuation_calls"] = len(conts)
    elif kind == "hardcore":
        token_run = reports.get("retail-handlers-hardcoregerman-token") or {}
        plain_run = reports.get("retail-handlers-hardcoregerman") or {}
        token_calls = token_run.get("_by_address", {}).get("0x0048F960", [])
        plain_calls = plain_run.get("_by_address", {}).get("0x0048F960", [])
        setters = token_run.get("_by_address", {}).get("0x0048F2B0", [])
        checks.append(dict(check="handler_called_once_with_token", passed=len(token_calls) == 1,
                           calls=len(token_calls)))
        checks.append(dict(check="handler_not_called_in_plain_window", passed=len(plain_calls) == 0,
                           calls=len(plain_calls)))
        if token_calls:
            call = token_calls[0]
            checks.append(dict(check="handler_returns_zero", passed=call["ret"] == 0))
            checks.append(dict(check="empty_suffix_pointer",
                               passed=(call["arg0_status"] == "read" and
                                       bool(call["arg0_hex"]) and call["arg0_hex"].startswith("00"))))
        checks.append(dict(check="german_version_setter_called_with_zero",
                           passed=any(setter["args"] and setter["args"][0] == 0 and
                                      setter["callIndex"] > 1 for setter in setters)))
        details["token_run_calls"] = token_calls
        details["plain_run_calls"] = plain_calls
        details["setter_calls"] = setters
        if token_calls:
            watch = token_run.get("_watch_by_address", {})
    elif kind == "unlimited":
        token_run = reports.get("retail-handlers-unlimitedammo-token") or {}
        plain_run = reports.get("retail-handlers-hardcoregerman") or {}
        token_calls = token_run.get("_by_address", {}).get("0x00410360", [])
        plain_calls = plain_run.get("_by_address", {}).get("0x00410360", [])
        checks.append(dict(check="handler_called_once_with_token", passed=len(token_calls) == 1,
                           calls=len(token_calls)))
        checks.append(dict(check="unobserved_in_plain_window_later_phase",
                           passed=len(plain_calls) == 0, calls=len(plain_calls)))
        if token_calls:
            checks.append(dict(check="eax_unchanged_zero", passed=token_calls[0]["ret"] == 0))
            watches = token_run.get("_watch_by_address", {}).get("0x00410360", [])
            checks.append(dict(check="byte_0056e214_written_to_one",
                               passed=any(watch.get("0x56e214") == 1 for watch in watches)))
        details["token_run_calls"] = token_calls
        details["plain_run_calls"] = plain_calls
    else:
        address = native["address"]
        for scenario in native["scenarios"]:
            report = reports.get(scenario)
            if not report:
                continue
            calls = report["_by_address"].get(address, [])
            checks.append(dict(check=f"handler_called_once[{scenario}]", passed=len(calls) == 1,
                               calls=len(calls), scenario=scenario))
            details[scenario] = dict(calls=calls)
            if scenario == "retail-handlers-window":
                writer = report["_by_address"].get("0x0048F120", [])
                checks.append(dict(check="set_screen_mode_zero_after_window_handler",
                                   passed=any(call["args"] and call["args"][0] == 0 for call in writer)))
            if scenario == "retail-handlers-wininput":
                setter = report["_by_address"].get("0x0048F280", [])
                checks.append(dict(check="win32_input_enabled_after_handler",
                                   passed=any(call["args"] and call["args"][0] == 1 and
                                              call["caller"] not in ("0x48f648",) for call in setter)
                                   or any(call["args"] and call["args"][0] == 1 for call in setter[1:])))
            if scenario == "retail-handlers-germany":
                language = [c for c in report["_by_address"].get("0x0048F300", [])
                            if c["arg0_text"] == "Germany"]
                german = [c for c in report["_by_address"].get("0x0048F2B0", [])
                          if c["caller"] not in (None, "0x48f661")]
                checks.append(dict(check="language_set_germany", passed=bool(language)))
                checks.append(dict(check="german_version_enabled", passed=bool(german)))
            if scenario == "retail-handlers-usa":
                language = [c for c in report["_by_address"].get("0x0048F300", [])
                            if c["arg0_text"] == "USA"]
                checks.append(dict(check="language_set_usa", passed=bool(language)))
    return dict(checks=checks, passed=all(item["passed"] for item in checks) and bool(checks),
                details=details)


# Candidate symbol VA per retail VA on the deployed integrated build (nm -C).
CANDIDATE_SYMBOLS = {
    "0x0048F360": "0x0041FE90", "0x004A5A0C": "0x0041FD00",
    "0x0048F350": "0x004201D0", "0x0048F920": "0x004201E0",
    "0x0048F930": "0x004201F0", "0x0048F950": "0x00420210",
    "0x0048F960": "0x00420220",
    "0x00410360": None,  # intentionally not linked (later-phase dependency)
}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--candidate", type=Path,
                    default=ROOT / "build-win32-no-verify-gate/igi1.exe")
    ap.add_argument("--candidate-report", type=Path,
                    help="candidate runtime report captured on the deployed candidate")
    args = ap.parse_args()
    out_dir = args.run_dir / "p05c"
    out_dir.mkdir(parents=True, exist_ok=True)

    reports = {}
    for name in ("retail-parser-window-level1", "retail-handlers-window",
                 "retail-handlers-wininput", "retail-handlers-germany",
                 "retail-handlers-usa", "retail-handlers-hardcoregerman-token",
                 "retail-handlers-unlimitedammo-token", "retail-handlers-hardcoregerman"):
        path = args.run_dir / "runtime" / f"{name}.json"
        reports[name] = load_report(path)

    candidate_report = None
    if args.candidate_report and args.candidate_report.is_file():
        candidate_report = load_report(args.candidate_report)

    staged = dict(path=str(STAGED), sha256=None,
                  present=STAGED.is_file(),
                  note="Staged transcription, not linked into the engine; reviewed as the "
                       "candidate-to-be, not as retail evidence.")
    if STAGED.is_file():
        import hashlib
        staged["sha256"] = hashlib.sha256(STAGED.read_bytes()).hexdigest()

    summary = []
    for address, native in NATIVES.items():
        native = dict(native, address=address)
        static_path = args.run_dir / "static" / f"{address}-static.json"
        static = json.loads(static_path.read_text()) if static_path.is_file() else {}
        cfg = (static.get("binary_ninja") or {}).get("cfg") or {}
        crosscheck = static.get("bytes_crosscheck") or {}
        expected_bytes = static.get("expected_bytes") or {}
        blocks = len(cfg.get("nodes", []))
        static_checks = dict(
            bytes_match=bool(crosscheck.get("match") or expected_bytes.get("match")),
            bytes_method=("bn_memory_vs_r2_pD" if crosscheck else
                          ("handoff-table" if expected_bytes else None)),
            bn_blocks=blocks,
            bn_blocks_match_expected=blocks == native["blocks"],
            r2_blocks=len((static.get("radare2") or {}).get("cfg", {}).get("nodes", [])),
            angr_exact_entry=((static.get("angr") or {}).get("function_entry") == address
                              and (static.get("angr") or {}).get("has_unresolved_jumps") is False),
            evidence_files=bool(static.get("evidence")),
            staged_source_present=staged["present"],
        )
        static_ok = (static_checks["bytes_match"] and static_checks["bn_blocks_match_expected"]
                     and static_checks["r2_blocks"] > 0 and static_checks["angr_exact_entry"])
        runtime = runtime_checks(native, reports)
        record = dict(
            kind="igi-tester-p05c-native-record", schema_version=1,
            address=address, name=native["name"], kind_detail=native["kind"],
            retail=dict(report_executable=(reports[
                "retail-parser-window-level1"]["_executable"])),
            candidate=dict(path=str(args.candidate),
                           sha256=hashlib.sha256(args.candidate.read_bytes()).hexdigest()),
            candidate_symbol_va=CANDIDATE_SYMBOLS.get(address),
            candidate_runtime=dict(
                report=str(args.candidate_report) if args.candidate_report else None,
                calls=(len(candidate_report["_by_address"].get(address, []))
                       if candidate_report else None),
                executable_sha256=(candidate_report["_executable"] if candidate_report else None)),
            staged_source=staged,
            staged_review=STAGED_REVIEW.get(address, []),
            static=dict(checks=static_checks, expected_size=native["size"],
                        cfg_blocks=blocks, angr=(static.get("angr") or {}),
                        verdict="PASS" if static_ok else ("BLOCKED" if not static else "FAIL")),
            runtime=dict(checks=runtime["checks"], passed=runtime["passed"],
                         verdict="OBSERVED" if runtime["passed"] else "INCOMPLETE",
                         details=runtime["details"]),
            candidate_verdict=(
                f"UNOBSERVED(symbol {CANDIDATE_SYMBOLS[address]} 0 calls in window)"
                if candidate_report and CANDIDATE_SYMBOLS.get(address)
                else ("UNOBSERVED(not linked)" if not CANDIDATE_SYMBOLS.get(address)
                      else "UNOBSERVED(symbol linked, no candidate report)")),
            parity="UNOBSERVED_CANDIDATE",
            limitations=[
                "Runtime observations come from four sequential suspended-launch retail runs "
                "(`window level1` plus one optional CLI token); later calls outside the 20 s "
                "window are not covered.",
                "The Window handler's call to 0x0048F120 is established by the captured temporal "
                "order and by disassembly; in one capture the Frida return-address read for the "
                "handler-internal call came back out-of-image (0x1db0a21), and an earlier capture "
                "recorded 0x48F357 inside the handler.",
                "arg snapshots on handler suffix pointers point into the parser's stack copy of the "
                "command line; they are raw bounded reads, not parsed strings.",
                "Candidate does not link these natives yet; the candidate runtime axis stays "
                "UNOBSERVED and is a blocker, not a pass.",
            ],
        )
        out = out_dir / f"{address}.json"
        out.write_text(json.dumps(record, indent=2) + "\n")
        summary.append(record)
        print(f"{address} {native['name']}: static={record['static']['verdict']} "
              f"runtime={record['runtime']['verdict']} candidate={record['candidate_verdict']}")
        for item in runtime["checks"]:
            if not item["passed"]:
                print(f"    FAILED {item['check']} {item}")

    lines = ["# P05c parser / tokenizer / handler records (Tester)", "",
             f"Staged source: `{staged['path']}` sha256={staged['sha256']}", "",
             "| VA | name | static | runtime | candidate |",
             "| -- | ---- | ------ | ------- | --------- |"]
    for record in summary:
        lines.append(f"| {record['address']} | {record['name']} | {record['static']['verdict']} | "
                     f"{record['runtime']['verdict']} | {record['candidate_verdict']} |")
    (out_dir / "SUMMARY.md").write_text("\n".join(lines) + "\n")
    print(f"summary -> {out_dir/'SUMMARY.md'}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
