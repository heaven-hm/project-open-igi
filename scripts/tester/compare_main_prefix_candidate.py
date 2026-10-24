#!/usr/bin/env python3
"""Typed candidate main-prefix comparison against the accepted retail baseline.

Reads the accepted retail capture, the candidate capture and
`contracts/candidate-profile.json`. The candidate is a reconstruction whose
code addresses differ, so the comparison is per observable and per proven
relation, never raw pointer equality across processes. Missing calls, substituted
helpers and changed API call counts remain mismatches even when a profile records
them as known differences. The boundary is the
QTask_CreateTask call: its entry is the boundary; return/exception behavior is
outside that scope and is not inferred from a missing return. No status is
promoted; verdicts are PREFIX_* only.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import pathlib

UNAVAILABLE = "unavailable"


def as_int(value):
    if value is None:
        return None
    if isinstance(value, int):
        return value
    try:
        return int(str(value), 0)
    except (TypeError, ValueError):
        return None


def hexv(value):
    return UNAVAILABLE if value is None else hex(value)


def events(report, kind):
    return [event for event in report.get("events") or [] if event.get("type") == kind]


def calls(report, name=None):
    return [event for event in events(report, "api_call")
            if name is None or event.get("name") == name]


def returns(report, name):
    return [event.get("returnValue") for event in events(report, "api_return")
            if event.get("name") == name]


def words(event):
    if not event:
        return []
    return [word.get("value") for word in event.get("argWords") or []]


def strings(event):
    if not event:
        return []
    return [(entry.get("slot"), entry.get("text")) for entry in event.get("strings") or []]


def struct_u32(snapshot, offset):
    hex_text = (snapshot or {}).get("hex") or ""
    chunk = hex_text[offset * 2:offset * 2 + 8]
    if len(chunk) != 8:
        return None
    return sum(int(chunk[i:i + 2], 16) << (8 * (i // 2)) for i in range(0, 8, 2))


def globals_at(report, label):
    for event in events(report, "globals"):
        if event.get("tag") == "qtask" and event.get("phase") == "call":
            for snapshot in event.get("snapshots") or []:
                if snapshot.get("label") == label:
                    return snapshot
    return None


def snapshot_u32(snapshot, offset=0):
    return struct_u32(snapshot, offset)


def snapshot_bytes(snapshot, offset, length):
    hex_text = (snapshot or {}).get("hex") or ""
    chunk = hex_text[offset * 2:offset * 2 + length * 2]
    if len(chunk) != length * 2:
        return None
    return bytes(int(chunk[i:i + 2], 16) for i in range(0, len(chunk), 2))


class Comparison:
    def __init__(self, profile, retail, candidate, probe_root=None):
        self.profile = profile
        self.retail = retail
        self.candidate = candidate
        self.rows = []
        self.divergences = []
        self.probe_root = probe_root

    def row(self, observable, status, retail_value, candidate_value, note=""):
        self.rows.append(dict(observable=observable, status=status,
                              retail=retail_value, candidate=candidate_value, note=note))

    def compare_equal(self, observable, retail_value, candidate_value, note=""):
        status = "MATCH" if retail_value == candidate_value else "MISMATCH"
        if status == "MISMATCH" and not self.divergences:
            self.divergences.append(dict(observable=observable, retail=retail_value,
                                         candidate=candidate_value, note=note))
        self.row(observable, status, retail_value, candidate_value, note)
        return status == "MATCH"

    def process_scoped(self, observable, retail_value, candidate_value, note):
        self.row(observable, "PROCESS_SCOPED_VALUE", retail_value, candidate_value, note)

    def declared(self, observable, retail_value, candidate_value, note, divergence_id):
        self.row(observable, "DECLARED_BUILD_DIVERGENCE", retail_value, candidate_value,
                 f"{note} [{divergence_id}]")

    def identity(self):
        profile = self.profile
        self.compare_equal("retail image sha256", profile["retail_sha256"],
                           (self.retail.get("executable") or {}).get("sha256"))
        sha = ((self.candidate.get("executable") or {}).get("sha256")
               or (self.candidate.get("identity") or {}).get("sha256"))
        self.compare_equal("candidate image sha256", profile["candidate_sha256"], sha)
        targets = self.candidate.get("targets") or []
        probe_sha = targets[0].get("probe_sha256") if targets else None
        self.compare_equal("candidate probe sha256", profile["probe"]["sha256"], probe_sha)
        self.row("candidate hook installation",
                 "MATCH" if not (self.candidate.get("failures") or self.candidate.get("error")) else "MISMATCH",
                 "no failures", [f.get("name") for f in self.candidate.get("failures") or []]
                 + ([self.candidate.get("error")] if self.candidate.get("error") else []))
        for side, report in (("retail", self.retail), ("candidate", self.candidate)):
            problems = list(report.get("failures") or [])
            targets = report.get("targets") or []
            if len(targets) != 1:
                problems.append(f"expected one bound main probe, found {len(targets)}")
            if len(report.get("summaries") or []) != 1 or report.get("summary_errors"):
                problems.append("complete final probe summary is unavailable")
            if report.get("error"):
                problems.append(report["error"])
            for hook in report.get("hooks") or []:
                problems.extend(hook.get("failures") or [])
            for summary in report.get("summaries") or []:
                if summary.get("overflowed") or summary.get("failureCount"):
                    problems.append(summary)
            self.compare_equal(f"{side} capture validity", [], problems)
            self.row(f"{side} call stream present", "MATCH" if calls(report) else "MISMATCH",
                     "nonempty call stream", len(calls(report)))
            for index, target in enumerate(report.get("targets") or []):
                path = pathlib.Path(target.get("probe") or "")
                if self.probe_root is not None:
                    path = self.probe_root / path.name
                expected = target.get("probe_sha256")
                actual = hashlib.sha256(path.read_bytes()).hexdigest() if path.is_file() else None
                self.row(f"{side} retained probe {index}",
                         "MATCH" if expected is not None and expected == actual else "MISMATCH",
                         expected, actual, "Missing or changed probe blocks replay.")

    def entry(self):
        entries = self.candidate.get("entries") or []
        self.compare_equal("candidate main entries", 1, len(entries))
        if entries:
            self.row("candidate entry caller",
                     "MATCH" if entries[0].get("callerReturnAddress") else "MISMATCH",
                     "non-null caller return address", entries[0].get("callerReturnAddress"))
            arg_words = [word.get("value") for word in entries[0].get("argWords") or []]
            self.row("candidate entry argument words", "MATCH" if len(arg_words) >= 4 else "MISMATCH",
                     "4 words [hInstance, hPrev, lpCmdLine, nShow]", arg_words)

    def calls_and_boundary(self):
        retail_names = [event.get("name") for event in calls(self.retail)]
        candidate_names = [event.get("name") for event in calls(self.candidate)]
        self.compare_equal("complete ordered prefix calls", retail_names, candidate_names,
                           "No missing call, duplicate API call or substitute is filtered out.")
        if retail_names != candidate_names:
            first = next(index for index in range(max(len(retail_names), len(candidate_names)))
                         if retail_names[index:index + 1] != candidate_names[index:index + 1])
            self.row("first different prefix call", "MISMATCH",
                     retail_names[first:first + 1], candidate_names[first:first + 1],
                     f"zero-based ordered call index {first}")
        self.compare_equal("GetLastError call count",
                           len(returns(self.retail, "GetLastError")),
                           len(returns(self.candidate, "GetLastError")))
        # Entry is a prefix boundary observation, never proof of callee behavior.
        qtask = calls(self.candidate, "QTask_CreateTask")
        self.row("candidate QTask_CreateTask entry", "MATCH" if len(qtask) == 1 else "MISMATCH",
                 "1 entry", f"{len(qtask)} entry")
        self.row("candidate QTask_CreateTask return",
                 "DECLARED_BOUNDARY_UNOBSERVED", returns(self.retail, "QTask_CreateTask"),
                 returns(self.candidate, "QTask_CreateTask"),
                 "outside the entry boundary; absence alone does not establish a throw or its cause")

    def values(self):
        profile = self.profile
        facts = profile["candidate_facts"]
        entry = (self.candidate.get("entries") or [{}])[0]
        entry_words = [word.get("value") for word in entry.get("argWords") or []]
        hinstance = entry_words[0] if entry_words else None

        mutex = calls(self.candidate, "CreateMutexA")
        mutex_words = words(mutex[0]) if mutex else []
        mutex_name = [text for slot, text in strings(mutex[0]) if slot == 2] if mutex else []
        self.row("CreateMutexA called once", "MATCH" if len(mutex) == 1 else "MISMATCH", 1, len(mutex))
        if mutex_name and mutex_name[0] == "IGI1MUTEX":
            self.declared("mutex name", "IGIMUTEX", mutex_name[0],
                          "IGI_TESTING_MUTEX testing build (side-by-side with retail)",
                          "mutex-name")
        else:
            self.compare_equal("mutex name", "IGIMUTEX", mutex_name)
        self.row("CreateMutexA handle non-null",
                 "MATCH" if mutex and returns(self.candidate, "CreateMutexA") and
                 returns(self.candidate, "CreateMutexA")[0] != 0 else "MISMATCH",
                 "non-null handle", returns(self.candidate, "CreateMutexA"))
        self.compare_equal("GetLastError value(s)", returns(self.retail, "GetLastError"),
                           returns(self.candidate, "GetLastError"),
                           "Every observed error-query return is compared in order.")
        self.compare_equal("CoInitialize return", 0, (returns(self.candidate, "CoInitialize") or [None])[0])
        self.compare_equal("GetForegroundWindow return", 0,
                           (returns(self.candidate, "GetForegroundWindow") or [None])[0])
        self.compare_equal("LoadIconA return", 0, (returns(self.candidate, "LoadIconA") or [None])[0])
        self.process_scoped("LoadCursorA return", 0x00B400BE,
                            (returns(self.candidate, "LoadCursorA") or [None])[0],
                            "IDC_ARROW handle; Wine USER handles are process-scoped, so the retail "
                            "0xB400BE and candidate value cannot be equal cross-process; the "
                            "WNDCLASSA.hCursor relation is checked instead")
        register_return = (returns(self.candidate, "RegisterClassA") or [None])[0]
        self.row("RegisterClassA return nonzero",
                 "MATCH" if register_return not in (None, 0) else "MISMATCH", "nonzero", register_return)

        # GetSystemMetrics order and derived geometry.
        metrics = returns(self.candidate, "GetSystemMetrics")
        self.compare_equal("GetSystemMetrics values", [1050, 1680], metrics)
        create = calls(self.candidate, "CreateWindowExA")
        create_words = words(create[0]) if create else []
        self.compare_equal("CreateWindowExA X", 520, create_words[4] if len(create_words) > 4 else None)
        self.compare_equal("CreateWindowExA Y", 285, create_words[5] if len(create_words) > 5 else None)
        self.compare_equal("CreateWindowExA style", 0x37C80000,
                           create_words[3] if len(create_words) > 3 else None)
        self.compare_equal("CreateWindowExA size", [0x280, 0x1E0],
                           create_words[6:8] if len(create_words) > 7 else None)
        self.compare_equal("CreateWindowExA exstyle", 0, create_words[0] if create_words else None)
        self.compare_equal("CreateWindowExA class+title",
                           ["Innerloop", "Innerloop"],
                           [text for slot, text in strings(create[0]) if slot in (1, 2)] if create else [])
        self.compare_equal("CreateWindowExA hInstance", hinstance,
                           create_words[10] if len(create_words) > 10 else None)
        self.compare_equal("CreateWindowExA parent/menu/param",
                           [0, 0, 0],
                           [create_words[8], create_words[9], create_words[11]]
                           if len(create_words) > 11 else None)
        hwnd = (returns(self.candidate, "CreateWindowExA") or [None])[0]
        self.row("CreateWindowExA returned hwnd", "MATCH" if hwnd not in (None, 0) else "MISMATCH",
                 "nonzero hwnd", hexv(hwnd))
        show_cursor = (returns(self.candidate, "ShowCursor") or [None])[0]
        self.compare_equal("ShowCursor(0) return", 0xFFFFFFFF, show_cursor)
        show_window = (returns(self.candidate, "ShowWindow") or [None])[0]
        self.row("ShowWindow(hwnd,10) return", "MATCH" if show_window not in (None, 0) else "MISMATCH",
                 "1", show_window)
        store_handle_words = words(calls(self.candidate, "StoreWindowHandle")[0]) if calls(self.candidate, "StoreWindowHandle") else []
        self.compare_equal("StoreWindowHandle argument is the created hwnd",
                           hwnd, store_handle_words[0] if store_handle_words else None)

        # WNDCLASSA bytes and relations.
        register = calls(self.candidate, "RegisterClassA")
        snapshot = (register[0].get("struct") if register else None) or {}
        fields = {
            "style": 3, "lpfnWndProc": as_int(facts["window_procedure"]),
            "cbClsExtra": 0, "cbWndExtra": 0, "hInstance": hinstance,
            "hIcon": (returns(self.candidate, "LoadIconA") or [None])[0],
            "hCursor": (returns(self.candidate, "LoadCursorA") or [None])[0],
            "hbrBackground": 7,
        }
        for name, offset in (("style", 0), ("cbClsExtra", 8), ("cbWndExtra", 12),
                             ("hInstance", 16), ("hIcon", 20), ("hCursor", 24), ("hbrBackground", 28)):
            self.compare_equal(f"WNDCLASSA.{name}", fields[name], struct_u32(snapshot, offset))
        # The candidate window procedure is a different image address; bind it by
        # the candidate symbol, keep the retail address as the baseline.
        observed_proc = struct_u32(snapshot, 4)
        proc_ok = observed_proc == as_int(facts["window_procedure"])
        self.row("WNDCLASSA.lpfnWndProc bound by symbol", "MATCH" if proc_ok else "MISMATCH",
                 "0x0048F970", hexv(observed_proc),
                 "candidate WindowProcedure at 0x417D20; retail address recorded as baseline, "
                 "not compared across images")
        menu = struct_u32(snapshot, 0x20)
        class_name = struct_u32(snapshot, 0x24)
        self.compare_equal("WNDCLASSA.lpszMenuName == lpszClassName", True, menu == class_name)
        struct_strings = [(entry.get("text"), entry.get("slot")) for entry in (register[0].get("structStrings") or [])] if register else []
        self.compare_equal("WNDCLASSA menu/class strings", ["Innerloop", "Innerloop"],
                           [text for text, slot in struct_strings if slot in ("struct+0x20", "struct+0x24")])
        self.declared("WNDCLASSA.lpfnWndProc relocation", "0x0048F970", hexv(as_int(facts["window_procedure"])),
                      "candidate WindowProcedure is a different image address, bound by symbol",
                      "wndproc-address-relocation")

        # Handler code pointers passed to the CLI parser are candidate addresses.
        expected_handlers = facts["handler_pointers"]
        observed_handlers = []
        for event in calls(self.candidate, "AppMain_ParseCmdLineArgs"):
            event_words = words(event)
            observed_handlers.append(hexv(as_int(event_words[1])) if len(event_words) > 1 else None)
        expected_retail = ["0x0048F350", "0x0048F920", "0x0048F930", "0x0048F950"]
        expected_candidate = [expected_handlers[name] for name in
                              ("Window", "WinInput", "Germany", "USA")]
        self.declared("handler code pointer addresses", expected_retail, observed_handlers,
                      "same four handlers in a different image, bound by symbol", "handler-address-relocation")
        self.compare_equal("handler code pointers bound by symbol",
                           [as_int(value) for value in expected_candidate],
                           [as_int(value) for value in observed_handlers])

        # Side-effect globals that do have a candidate equivalent.
        window_handle = globals_at(self.candidate, "window_handle_g_5c8bc4")
        self.compare_equal("[0x5C8BC4] window handle == hwnd", hwnd,
                           snapshot_u32(window_handle) if window_handle else None,
                           "candidate global g_window_handle at 0x005DCE60")
        loop_byte = globals_at(self.candidate, "loop_status_byte_5c8de8")
        loop_value = None
        if loop_byte:
            raw = snapshot_bytes(loop_byte, 0, 1)
            loop_value = raw[0] if raw else None
        self.compare_equal("[0x5C8DE8] loop status byte before the boundary", 0, loop_value,
                           "candidate Loop_StatusByte at 0x005DCDF0")
        # The chain is a container: head cell, sentinel/terminator node, tail cell.
        # The candidate relocates all three to its own image, so compare the
        # retail shape as relations, not cross-process addresses.
        head_snap = globals_at(self.candidate, "window_handler_chain_5c8df0")
        term_snap = globals_at(self.candidate, "window_handler_terminator_5c8df4")
        tail_snap = globals_at(self.candidate, "window_handler_tail_5c8df8")
        head_value = snapshot_u32(head_snap) if head_snap else None
        term_bytes = snapshot_bytes(term_snap, 0, 12) if term_snap else None
        term_next = struct_u32(term_snap, 0)
        term_prev = struct_u32(term_snap, 4)
        term_handler = struct_u32(term_snap, 8)
        tail_value = snapshot_u32(tail_snap) if tail_snap else None
        chain_cell_va = as_int(facts["globals"]["window_handler_chain"])
        term_va = as_int(facts["globals"]["window_handler_terminator"])
        tail_va = as_int(facts["globals"]["window_handler_tail"])
        self.row("chain head points at sentinel", "MATCH" if head_value == term_va else "MISMATCH",
                 "head == sentinel", hexv(head_value))
        self.row("sentinel.next == 0", "MATCH" if term_next == 0 else "MISMATCH", 0, term_next)
        self.row("sentinel.prev_link == &head", "MATCH" if term_prev == chain_cell_va else "MISMATCH",
                 "prev_link == &head", hexv(term_prev))
        self.row("chain tail == &head", "MATCH" if tail_value == chain_cell_va else "MISMATCH",
                 "tail == &head", hexv(tail_value))
        self.row("sentinel.handler == 0 (empty chain)", "MATCH" if term_handler == 0 else "MISMATCH",
                 0, term_handler,
                 "retail does not initialize the sentinel handler in B3; empty chain skips it")
        self.declared("chain container relocation", "0x005C8DF0/0x005C8DF4/0x005C8DF8",
                      [hexv(chain_cell_va), hexv(term_va), hexv(tail_va)],
                      "candidate uses its own contiguous container; the retail walk shape is compared "
                      "by relation", "chain-container-relocation")

    def run(self):
        self.identity()
        self.entry()
        self.calls_and_boundary()
        self.values()
        undeclared_failures = [row for row in self.rows if row["status"] == "MISMATCH"]
        identity_fail = [row for row in self.rows if row["status"] == "MISMATCH"
                         and (row["observable"].startswith("candidate image sha")
                              or row["observable"].startswith("candidate probe sha")
                              or row["observable"].startswith("retail image sha")
                              or "capture validity" in row["observable"]
                              or "retained probe" in row["observable"])]
        if identity_fail:
            verdict = "PREFIX_CANDIDATE_BLOCKED"
        elif undeclared_failures:
            verdict = "PREFIX_CANDIDATE_MISMATCH"
        else:
            verdict = "PREFIX_CANDIDATE_MATCH"
        return dict(kind="igi-tester-main-prefix-candidate-comparison", schema_version=1,
                    verdict=verdict,
                    verified=False,
                    candidate_sha256=self.profile["candidate_sha256"],
                    probe_sha256=self.profile["probe"]["sha256"],
                    retail_report="retail-main.json", candidate_report="candidate-main.json",
                    first_divergence=self.divergences[0] if self.divergences else None,
                    historical_profile_notes=self.profile["declared_build_divergences"],
                    profile_notes_authorize_exemptions=False,
                    boundary=dict(entry="observed" if calls(self.candidate, "QTask_CreateTask") else "not observed",
                                  candidate_return=returns(self.candidate, "QTask_CreateTask"),
                                  retail_return=returns(self.retail, "QTask_CreateTask")),
                    matches=len([row for row in self.rows if row["status"] == "MATCH"]),
                    mismatches=len(undeclared_failures),
                    rows=self.rows,
                    note="PREFIX_* only. A prefix match is not a native pass and promotes no status. "
                         "Everything after the QTask_CreateTask boundary is UNOBSERVED-until-reachable.")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--profile", type=pathlib.Path, required=True)
    ap.add_argument("--retail", type=pathlib.Path, required=True)
    ap.add_argument("--candidate", type=pathlib.Path, required=True)
    ap.add_argument("--out", type=pathlib.Path, required=True)
    ap.add_argument("--probe-root", type=pathlib.Path,
                    help="Directory holding archived probe basenames; each hash must still match its capture")
    args = ap.parse_args()
    profile = json.loads(args.profile.read_text())
    retail = json.loads(args.retail.read_text())
    candidate = json.loads(args.candidate.read_text())
    result = Comparison(profile, retail, candidate, args.probe_root).run()
    args.out.write_text(json.dumps(result, indent=2) + "\n")
    print(f"{result['verdict']} matches={result['matches']} mismatches={result['mismatches']} "
          f"first_divergence={result['first_divergence']}")
    for row in result["rows"]:
        marker = {"MATCH": "ok  ", "MISMATCH": "FAIL", "DECLARED_BUILD_DIVERGENCE": "decl",
                  "DECLARED_BOUNDARY_UNOBSERVED": "unob",
                  "PROCESS_SCOPED_VALUE": "scoped"}[row["status"]]
        print(f" {marker} {row['observable']}: retail={row['retail']} candidate={row['candidate']}")
    return 0 if result["verdict"] == "PREFIX_CANDIDATE_MATCH" else 1


if __name__ == "__main__":
    raise SystemExit(main())
