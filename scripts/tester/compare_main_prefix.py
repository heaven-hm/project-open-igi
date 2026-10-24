#!/usr/bin/env python3
"""Typed comparator for the retail main prefix (0x0048F530..0x0048F7AA).

Reads the generated contract plus a driver report and checks the ordered call
stream, typed arguments, WNDCLASSA snapshot, CreateWindowExA slots, derived
metrics/window geometry, side-effect snapshots, proven-string bytes and the
QTask_CreateTask boundary. Post-boundary behaviour is never scored.

Verdicts: PREFIX_MATCH (all in-scope rows observed and equal), MISMATCH, or
BLOCKED (identity/probe/hook failure). A missing side is UNOBSERVED. This tool
never edits manifests, engine sources or statuses.
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

UNAVAILABLE = "unavailable"


def norm(value):
    if value is None:
        return None
    text = str(value).strip().lower()
    if text.startswith("0x"):
        try:
            return hex(int(text, 16))
        except ValueError:
            return text
    try:
        return hex(int(text, 0))
    except ValueError:
        return text


def as_int(value):
    if value is None:
        return None
    if isinstance(value, int):
        return value
    try:
        return int(str(value), 0)
    except (TypeError, ValueError):
        return None


class Checker:
    def __init__(self):
        self.checks = []

    def check(self, name, ok, detail="", side="retail"):
        self.checks.append(dict(side=side, check=name, ok=bool(ok), detail=str(detail)))
        return bool(ok)

    def failed(self, side=None):
        return [c for c in self.checks if not c["ok"] and (side is None or c["side"] == side)]


def events(report, kind):
    return [e for e in report.get("events") or [] if e.get("type") == kind]


def returns_by_name(report):
    result = {}
    for event in events(report, "api_return"):
        result.setdefault(event.get("name"), []).append(event)
    return result


def struct_u32(snapshot, offset):
    hex_text = (snapshot or {}).get("hex") or ""
    start = offset * 2
    chunk = hex_text[start:start + 8]
    if len(chunk) != 8:
        return None
    byte = [int(chunk[i:i + 2], 16) for i in range(0, 8, 2)]
    return byte[0] | (byte[1] << 8) | (byte[2] << 16) | (byte[3] << 24)


def struct_bytes(snapshot, offset, length):
    hex_text = (snapshot or {}).get("hex") or ""
    start = offset * 2
    chunk = hex_text[start:start + length * 2]
    if len(chunk) != length * 2:
        return None
    return bytes(int(chunk[i:i + 2], 16) for i in range(0, len(chunk), 2))


class MainPrefixComparator:
    def __init__(self, contract_path: pathlib.Path, retail_path: pathlib.Path,
                 candidate_path: pathlib.Path | None = None,
                 expected_candidate_sha: str | None = None):
        self.contract = json.loads(pathlib.Path(contract_path).read_text())
        self.retail_path = pathlib.Path(retail_path)
        self.retail = json.loads(self.retail_path.read_text()) if self.retail_path.exists() else None
        self.candidate_path = pathlib.Path(candidate_path) if candidate_path else None
        self.candidate = (json.loads(self.candidate_path.read_text())
                          if self.candidate_path and self.candidate_path.exists() else None)
        self.expected_candidate_sha = expected_candidate_sha
        self.checks = Checker()

    # ---- identity ---------------------------------------------------------
    def identity(self, report, side):
        if report is None:
            self.checks.check(f"identity.{side}_report", False,
                              f"missing report {self.retail_path if side == 'retail' else self.candidate_path}", side)
            return False
        contract_sha = (self.contract.get("identity") or {}).get(f"{side if side == 'candidate' else 'retail'}_sha256")
        observed_sha = ((report.get("executable") or {}).get("sha256")
                        or ((report.get("identity") or {}).get("sha256")))
        ok_sha = self.checks.check(f"identity.{side}_sha", observed_sha == contract_sha,
                                   f"{observed_sha} == {contract_sha}", side)
        targets = report.get("targets") or []
        target = targets[0] if targets else {}
        probe_sha = target.get("probe_sha256")
        expected_probe = (self.contract.get("probe") or {}).get("sha256")
        ok_probe = self.checks.check(f"identity.{side}_probe_sha", probe_sha == expected_probe,
                                     f"{probe_sha} == {expected_probe}", side)
        hooks = report.get("hooks") or []
        error = report.get("error")
        ok_hook = self.checks.check(f"identity.{side}_hook", bool(hooks) and not error,
                                    f"hooks={len(hooks)} error={error}", side)
        if side == "candidate" and self.expected_candidate_sha:
            ok_sha = self.checks.check("identity.candidate_expected_sha",
                                       observed_sha == self.expected_candidate_sha,
                                       f"{observed_sha} == {self.expected_candidate_sha}", side) and ok_sha
        failures = report.get("failures") or []
        ok_failures = self.checks.check(f"identity.{side}_hook_failures", not failures,
                                        f"failures={failures}", side)
        return ok_sha and ok_probe and ok_hook and ok_failures

    # ---- typed argument comparison ---------------------------------------
    def compare_argument(self, side, row_number, expected, observed_words, observed_strings, context):
        slot = expected.get("slot")
        word = observed_words[slot] if slot is not None and slot < len(observed_words) else None
        status = (word or {}).get("status")
        value = (word or {}).get("value")
        kind = expected.get("kind")
        prefix = f"{side}.row[{row_number}].arg[{slot}]"
        if status not in ("read",):
            return self.checks.check(prefix, False, f"argument status={status} kind={kind}", side)
        if kind == "int":
            return self.checks.check(prefix, value == expected.get("value"),
                                     f"observed {value} == {expected.get('value')}", side)
        if kind == "string":
            matches = [s for s in observed_strings
                       if s.get("slot") == slot and s.get("text") == expected.get("text")]
            if expected.get("text") == "":
                ok = bool(matches) or value == as_int(expected.get("va"))
                return self.checks.check(prefix, ok,
                                         f"empty-string pointer {value} / {matches}", side)
            return self.checks.check(prefix, bool(matches),
                                     f"strings {observed_strings} expected slot {slot}='{expected.get('text')}'", side)
        if kind == "relation":
            relation = expected.get("relation")
            want = context.get(relation)
            ok = value is not None and want is not None and as_int(value) == as_int(want)
            return self.checks.check(prefix, ok, f"observed {value} ~{relation} {want}", side)
        if kind == "code-pointer":
            want = as_int(expected.get("va") or expected.get("value"))
            return self.checks.check(prefix, value == want, f"observed {value} == code pointer {want}", side)
        if kind in ("stack-struct", "stack-out"):
            return self.checks.check(prefix, value is not None, f"{kind} pointer {value} (contents checked separately)", side)
        if kind.startswith("derived"):
            return self.checks.check(prefix, True, f"derived {kind} validated by the window check", side)
        return self.checks.check(prefix, False, f"unknown expected argument kind {kind}", side)

    # ---- retail/candidate prefix validation -------------------------------
    def validate_side(self, side):
        report = self.retail if side == "retail" else self.candidate
        if report is None:
            self.checks.check(f"{side}.report", False, "report missing", side)
            return False
        if not self.identity(report, side):
            return False
        static = self.contract["static_expectations"]
        expected_rows = static["expected_sequence"]
        entry_events = report.get("entries") or []
        entry_ok = self.checks.check(f"{side}.entry_count", len(entry_events) == 1,
                                     f"entries {len(entry_events)} == 1", side)
        if not entry_ok:
            return False
        entry_words = entry_events[0].get("argWords") or []
        self.checks.check(f"{side}.entry_arg_words", len(entry_words) >= 4,
                          f"words {[w.get('hex') for w in entry_words]}", side)
        entry_numeric = []
        for word in entry_words:
            entry_numeric.append(as_int(word.get("value")) if isinstance(word, dict) else as_int(word))
        api_calls = events(report, "api_call")
        self.checks.check(f"{side}.sequence_count", len(api_calls) == len(expected_rows),
                          f"api calls {len(api_calls)} == {len(expected_rows)}", side)
        api_returns = returns_by_name(report)
        resolved = {
            "entry_arg1": entry_numeric[0] if len(entry_numeric) > 0 else None,
            "entry_arg2": entry_numeric[2] if len(entry_numeric) > 2 else None,
        }
        rows = min(len(api_calls), len(expected_rows))
        for index in range(rows):
            observed = api_calls[index]
            expected = expected_rows[index]
            number = expected.get("order")
            name_ok = self.checks.check(f"{side}.row[{number}].name",
                                        observed.get("name") == expected.get("name"),
                                        f"observed {observed.get('name')} == {expected.get('name')}", side)
            if not name_ok:
                continue
            # The probe records the return address (callsite + instruction length).
            expected_callsite = as_int(expected.get("callsite"))
            observed_callsite = as_int(observed.get("callsite"))
            delta = None if None in (expected_callsite, observed_callsite) else observed_callsite - expected_callsite
            self.checks.check(f"{side}.row[{number}].callsite", delta is not None and 0 <= delta <= 6,
                              f"callsite {observed.get('callsite')} - {expected.get('callsite')} = {delta}", side)
            words = observed.get("argWords") or []
            strings = observed.get("strings") or []
            for expected_arg in expected.get("expected_args") or []:
                self.compare_argument(side, number, expected_arg, words, strings, resolved)
            # Record return values referenced by later relations.
            if expected.get("name") in ("LoadIconA", "LoadCursorA", "CreateWindowExA", "Display_GetScreenMode(?)"):
                returned = (api_returns.get(expected.get("name")) or [{}])[-1].get("returnValue")
                key = {"LoadIconA": "LoadIconA_return", "LoadCursorA": "LoadCursorA_return",
                       "CreateWindowExA": "createwindow_return",
                       "Display_GetScreenMode(?)": "screenmode"}[expected.get("name")]
                resolved[key] = returned
        # WNDCLASSA snapshot
        register = next((e for e in api_calls if e.get("name") == "RegisterClassA"), None)
        self.check_wndclass(side, register, resolved)
        # CreateWindowExA typed slots and derived geometry
        self.check_create_window(side, api_calls, api_returns, resolved, entry_numeric)
        # metrics order already part of sequence; verify returned values feed X/Y in create window
        # QTask boundary
        qtask = next((e for e in api_calls if e.get("name") == "QTask_CreateTask"), None)
        self.checks.check(f"{side}.boundary.entry", qtask is not None,
                          f"QTask_CreateTask call {qtask.get('callsite') if qtask else None}", side)
        qtask_return = (api_returns.get("QTask_CreateTask") or [{}])[-1].get("returnValue")
        self.checks.check(f"{side}.boundary.retail_return", qtask_return == 1,
                          f"QTask_CreateTask EAX {qtask_return} == 1", side)
        # absences
        names = {e.get("name") for e in api_calls}
        for absence in static.get("static_absence") or []:
            what = absence.get("what")
            self.checks.check(f"{side}.absence.{what}", what not in names,
                              f"{what} present" if what in names else f"{what} absent", side)
        # side effects
        self.check_globals(side, report, resolved)
        return len(self.checks.failed(side)) == 0

    def check_wndclass(self, side, register, resolved):
        expected = self.contract["static_expectations"]["wndclass"]
        if register is None:
            self.checks.check(f"{side}.wndclass", False, "RegisterClassA not observed", side)
            return
        snapshot = register.get("struct") or {}
        self.checks.check(f"{side}.wndclass.snapshot",
                          snapshot.get("status") == "read" and len(snapshot.get("hex") or "") == 0x28 * 2,
                          f"status={snapshot.get('status')} bytes={len(snapshot.get('hex') or '') // 2}", side)
        for field in expected["fields"]:
            offset = field["offset"]
            value = struct_u32(snapshot, offset)
            name = field["name"]
            if "value" in field:
                self.checks.check(f"{side}.wndclass.{name}", value == as_int(field["value"]),
                                  f"observed {value} == {field['value']}", side)
            elif field.get("relation") == "entry_arg1":
                want = resolved.get("entry_arg1")
                self.checks.check(f"{side}.wndclass.{name}", value is not None and value == want,
                                  f"observed {value} == entry arg1 {want}", side)
            else:
                want = resolved.get(field["relation"])
                self.checks.check(f"{side}.wndclass.{name}", value is not None and value == want,
                                  f"observed {value} == {field['relation']} {want}", side)
        menu = struct_u32(snapshot, 0x20)
        class_name = struct_u32(snapshot, 0x24)
        self.checks.check(f"{side}.wndclass.menu_equals_class", menu == class_name,
                          f"menu {menu} == class {class_name}", side)
        strings = register.get("structStrings") or []
        innerloop = [s for s in strings if s.get("text") == "Innerloop"]
        self.checks.check(f"{side}.wndclass.innerloop", len(innerloop) >= 2,
                          f"struct strings {strings}", side)

    def check_create_window(self, side, api_calls, api_returns, resolved, entry_numeric):
        expected = self.contract["static_expectations"]["create_window"]
        call = next((e for e in api_calls if e.get("name") == "CreateWindowExA"), None)
        if call is None:
            self.checks.check(f"{side}.create_window", False, "CreateWindowExA not observed", side)
            return
        words = call.get("argWords") or []
        values = [(w.get("value") if isinstance(w, dict) else None) for w in words]
        strings = call.get("strings") or []
        for slot in expected["slots"]:
            index = slot["slot"]
            name = slot["name"]
            value = values[index] if index < len(values) else None
            if "value" in slot:
                self.checks.check(f"{side}.create_window.{name}", value == as_int(slot["value"]),
                                  f"observed {value} == {slot['value']}", side)
            elif "text" in slot:
                found = [s for s in strings if s.get("slot") == index and s.get("text") == slot["text"]]
                self.checks.check(f"{side}.create_window.{name}", bool(found),
                                  f"strings {strings} expected '{slot['text']}' at slot {index}", side)
            elif "relation" in slot:
                want = resolved.get(slot["relation"])
                self.checks.check(f"{side}.create_window.{name}",
                                  value is not None and want is not None and value == want,
                                  f"observed {value} ~{slot['relation']} {want}", side)
        # derived style
        screenmode = resolved.get("screenmode")
        style = values[3] if len(values) > 3 else None
        expected_style = 0x37C80000 if screenmode == 0 else 0x96000000
        self.checks.check(f"{side}.create_window.style_derived",
                          screenmode is not None and style == expected_style,
                          f"getter {screenmode} -> style {style} expected {hex(expected_style)}", side)
        # derived X/Y from GetSystemMetrics returns, 1 then 0 order is enforced by sequence
        metrics = [e for e in api_calls if e.get("name") == "GetSystemMetrics"]
        metric_returns = api_returns.get("GetSystemMetrics") or []
        if len(metrics) >= 2 and len(metric_returns) >= 2:
            cy = metric_returns[0].get("returnValue")
            cx = metric_returns[1].get("returnValue")
            expected_x = int((cx - 0x280) / 2) if isinstance(cx, int) else None
            expected_y = int((cy - 0x1E0) / 2) if isinstance(cy, int) else None
            self.checks.check(f"{side}.create_window.x",
                              expected_x is not None and values[4] == expected_x,
                              f"X {values[4]} == trunc(({cx}-0x280)/2) {expected_x}", side)
            self.checks.check(f"{side}.create_window.y",
                              expected_y is not None and values[5] == expected_y,
                              f"Y {values[5]} == trunc(({cy}-0x1E0)/2) {expected_y}", side)
        else:
            self.checks.check(f"{side}.create_window.metrics", False,
                              f"GetSystemMetrics calls {len(metrics)} returns {len(metric_returns)}", side)
        # ShowCursor(0) / ShowWindow(hwnd, 10)
        show_cursor = next((e for e in api_calls if e.get("name") == "ShowCursor"), None)
        if show_cursor:
            words_cursor = show_cursor.get("argWords") or []
            self.checks.check(f"{side}.show_cursor", words_cursor and words_cursor[0].get("value") == 0,
                              f"ShowCursor arg {words_cursor[0].get('value') if words_cursor else None} == 0", side)
        show_window = next((e for e in api_calls if e.get("name") == "ShowWindow"), None)
        hwnd = (api_returns.get("CreateWindowExA") or [{}])[-1].get("returnValue")
        if show_window:
            words_show = show_window.get("argWords") or []
            ok = (len(words_show) >= 2 and words_show[0].get("value") == hwnd
                  and words_show[1].get("value") == 10)
            self.checks.check(f"{side}.show_window", bool(ok),
                              f"ShowWindow args {[w.get('value') for w in words_show]} expected [hwnd,10]", side)
        store = next((e for e in api_calls if e.get("name") == "StoreWindowHandle"), None)
        if store:
            words_store = store.get("argWords") or []
            self.checks.check(f"{side}.store_window_handle",
                              bool(words_store) and words_store[0].get("value") == hwnd,
                              f"StoreWindowHandle arg {words_store[0].get('value') if words_store else None} == hwnd {hwnd}", side)

    def check_globals(self, side, report, resolved):
        snapshots = events(report, "globals")
        qtask = [s for s in snapshots if s.get("tag") == "qtask" and s.get("phase") == "call"]
        if not qtask:
            self.checks.check(f"{side}.globals.qtask", False, "no QTask call snapshot", side)
            return
        snap = {item["label"]: item for item in qtask[-1].get("snapshots") or []}
        clear = snap.get("clear_range_5c8d28") or {}
        clear_bytes = struct_bytes(clear, 0, clear.get("length") or 0)
        self.checks.check(f"{side}.globals.clear_range",
                          clear.get("status") == "read" and clear_bytes is not None and set(clear_bytes) == {0},
                          f"status={clear.get('status')} bytes set={sorted(set(clear_bytes))[:4] if clear_bytes else None}", side)
        chain = snap.get("ptr_chain_5c8df0") or {}
        if chain.get("status") == "read":
            first = struct_u32(chain, 0)
            second = struct_u32(chain, 4)
            third = struct_u32(chain, 8)
            self.checks.check(f"{side}.globals.ptr_chain",
                              first == 0x005C8DF4 and second == 0 and third == 0x005C8DF0,
                              f"[0x5C8DF0]={first:#x} [0x5C8DF4]={second:#x} [0x5C8DF8]={third:#x}", side)
        else:
            self.checks.check(f"{side}.globals.ptr_chain", False, f"status={chain.get('status')}", side)
        window = snap.get("window_handle_5c8bc4") or {}
        hwnd_value = struct_u32(window, 0) if window.get("status") == "read" else None
        self.checks.check(f"{side}.globals.window_handle",
                          hwnd_value is not None and hwnd_value == resolved.get("createwindow_return"),
                          f"[0x5C8BC4]={hwnd_value} == hwnd {resolved.get('createwindow_return')}", side)

    # ---- candidate vs retail --------------------------------------------
    def compare_cross_side(self):
        if self.candidate is None:
            self.checks.check("candidate.report", False, "candidate report missing", "candidate")
            return False
        retail_calls = events(self.retail, "api_call")
        candidate_calls = events(self.candidate, "api_call")
        ok = self.checks.check("candidate.sequence_count", len(candidate_calls) == len(retail_calls),
                               f"candidate {len(candidate_calls)} == retail {len(retail_calls)}", "candidate")
        resolved = {}
        retail_returns = returns_by_name(self.retail)
        candidate_returns = returns_by_name(self.candidate)
        # Compare only values that are process-independent: names, ordered callsites,
        # numeric constants, proven strings and derived geometry. Pointer values are
        # compared through their in-process relations in validate_side, never across
        # processes.
        for index, (left, right) in enumerate(zip(retail_calls, candidate_calls)):
            ok = self.checks.check(f"candidate.row[{index}].name", left.get("name") == right.get("name"),
                                   f"{right.get('name')} == {left.get('name')}", "candidate") and ok
            delta_ok = self.checks.check(
                f"candidate.row[{index}].callsite",
                as_int(left.get("callsite")) == as_int(right.get("callsite")),
                f"{right.get('callsite')} == {left.get('callsite')}", "candidate") and delta_ok
            left_strings = [(s.get("slot"), s.get("text")) for s in left.get("strings") or []]
            right_strings = [(s.get("slot"), s.get("text")) for s in right.get("strings") or []]
            ok = self.checks.check(f"candidate.row[{index}].strings", left_strings == right_strings,
                                   f"{right_strings} == {left_strings}", "candidate") and ok
            expected_row = self.contract["static_expectations"]["expected_sequence"][index]
            for expected_arg in expected_row.get("expected_args") or []:
                if expected_arg.get("kind") == "int":
                    slot = expected_arg["slot"]
                    left_word = (left.get("argWords") or [])
                    right_word = (right.get("argWords") or [])
                    lv = left_word[slot].get("value") if slot < len(left_word) else None
                    rv = right_word[slot].get("value") if slot < len(right_word) else None
                    ok = self.checks.check(f"candidate.row[{index}].arg[{slot}]", lv == rv,
                                           f"{rv} == retail {lv}", "candidate") and ok
        for name in ("QTask_CreateTask",):
            retail_value = (retail_returns.get(name) or [{}])[-1].get("returnValue")
            candidate_value = (candidate_returns.get(name) or [{}])[-1].get("returnValue")
            ok = self.checks.check(f"candidate.{name}.return",
                                   candidate_value is not None and candidate_value == retail_value,
                                   f"{candidate_value} == retail {retail_value}", "candidate") and ok
        return ok

    def run(self):
        retail_ok = self.validate_side("retail")
        if retail_ok:
            self.validate_side("candidate") if self.candidate is not None else self.checks.check(
                "candidate.report", False, "candidate report missing", "candidate")
            if self.candidate is not None:
                self.compare_cross_side()
        return self.result()

    def result(self):
        retail_fail = self.checks.failed("retail")
        candidate_fail = self.checks.failed("candidate")
        identity_fail = [c for c in self.checks.checks
                         if not c["ok"] and c["check"].startswith("identity.")]
        if identity_fail:
            verdict = "BLOCKED"
        elif self.retail is None:
            verdict = "UNOBSERVED"
        elif retail_fail:
            verdict = "MISMATCH"
        elif self.candidate is None:
            verdict = "PREFIX_RETAIL_MATCH"
        elif candidate_fail:
            verdict = "MISMATCH"
        else:
            verdict = "PREFIX_MATCH"
        passed = len([c for c in self.checks.checks if c["ok"]])
        return dict(kind="igi-tester-main-prefix-comparison", verdict=verdict,
                    retail_report=str(self.retail_path),
                    candidate_report=str(self.candidate_path) if self.candidate_path else None,
                    expected_candidate_sha=self.expected_candidate_sha,
                    passed=passed, total=len(self.checks.checks), checks=self.checks.checks)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--contract", type=pathlib.Path, required=True)
    ap.add_argument("--retail", type=pathlib.Path, required=True)
    ap.add_argument("--candidate", type=pathlib.Path)
    ap.add_argument("--expected-candidate-sha")
    ap.add_argument("--out", type=pathlib.Path)
    args = ap.parse_args()
    comparator = MainPrefixComparator(args.contract, args.retail, args.candidate, args.expected_candidate_sha)
    result = comparator.run()
    text = json.dumps(result, indent=2) + "\n"
    if args.out:
        pathlib.Path(args.out).write_text(text)
    print(f"{result['verdict']} ({result['passed']}/{result['total']} checks) -> {args.out}")
    for check in result["checks"]:
        if not check["ok"]:
            print(f" FAIL [{check['side']}] {check['check']}: {check['detail']}")
    return 0 if result["verdict"] in ("PREFIX_MATCH", "PREFIX_RETAIL_MATCH") else 1


if __name__ == "__main__":
    raise SystemExit(main())
