#!/usr/bin/env python3
"""Compare a candidate QTask_RegisterClasses trace against the retail registrar oracle.

Evidence tooling only: reads the retail per-registrar oracle and a candidate
registration-trace report, compares the executed registrar positions and their
ordered TaskType_Register results, and fails on the first divergence.

Accepted input is the typed registration probe report (``events`` with
``registrar_enter``/``registrar_leave`` and ``register`` enter/leave phases),
the same shape produced for the retail baseline. It is deliberately not a
generic raw-trace reader: fields that are not present are reported as invalid,
never guessed.

Addresses are not compared. The candidate is a separate process with relocated
code/data (including the declared IGI1MUTEX layout and handler relocations), so
registrar identity is by executed position order and class identity is by
``classId``/name/object-size. The oracle already ends at the QTask tail jump
(``0x00402C37``), so only positions before that jump are compared; later
post-startup replay calls to the same functions are ignored. The final
``0x4C1780`` position is not in the oracle's executed list (single-byte ret,
no registration).

Exit codes: 0 match, 1 mismatch, 2 invalid input.
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any


ORDER_MATCH = "REGISTRAR_ACCEPTANCE_MATCH"
ORDER_MISMATCH = "REGISTRAR_ACCEPTANCE_MISMATCH"


def printable_name(value: Any) -> str | None:
    """Trim the probe's over-long C-string read at the first non-printable byte.

    The retail/candidate probes read a bounded byte window from the name
    pointer; neighbouring strings can follow the terminator. The name itself
    is ASCII, so a non-printable character ends it. This is a decoding
    normalisation, not a semantic allowance: name equality is still exact
    over the normalised prefix.
    """
    if not isinstance(value, str):
        return None
    out = []
    for ch in value:
        code = ord(ch)
        if 0x20 <= code <= 0x7E:
            out.append(ch)
        else:
            break
    return "".join(out)


def event_positions(events: list[dict]) -> list[dict]:
    """Group registration events into executed registrar positions.

    A position is one outermost registrar entry. Nested registrar entries (if a
    registrar calls another one) belong to the same outermost position, matching
    the retail oracle's grouping.
    """
    if not isinstance(events, list):
        raise ValueError("trace 'events' is not a list")
    positions: list[dict] = []
    stack: list[dict] = []
    pending: dict[int, dict] = {}
    for event in events:
        if not isinstance(event, dict):
            continue
        kind = event.get("type")
        if kind == "registrar_enter":
            if not stack:
                positions.append({"registrar": event.get("registrar"),
                                  "qtask_callsite": event.get("callsite"),
                                  "classes": []})
                event["__position"] = positions[-1]
            stack.append(event)
        elif kind == "registrar_leave":
            if stack:
                stack.pop()
        elif kind == "register" and event.get("phase") == "enter":
            pending[event.get("seq")] = event
        elif kind == "register" and event.get("phase") == "leave":
            entry = pending.pop(event.get("seq"), None)
            if entry is None:
                continue
            root = stack[0] if stack else None
            position = root.get("__position") if root is not None else None
            if position is None:
                continue
            position["classes"].append({
                "classId": event.get("classId"),
                "name": printable_name(entry.get("name")),
                "objectSize": entry.get("objectSize"),
                "tasktype_callsite": entry.get("callsite"),
            })
    return positions


def first_value_divergence(expected: dict, observed: dict, path: str) -> dict | None:
    for field in ("classId", "name", "objectSize"):
        if expected.get(field) != observed.get(field):
            return {"path": f"{path}.{field}", "expected": expected.get(field),
                    "observed": observed.get(field)}
    return None


def compare(baseline: dict, trace: dict, ignore_extra: bool = True) -> dict:
    registrars = baseline.get("registrars")
    if not isinstance(registrars, list) or not registrars:
        raise ValueError("baseline has no non-empty 'registrars' list")
    events = trace.get("events")
    if events is None:
        raise ValueError("trace has no 'events' list; expected the typed registration probe report")
    positions = event_positions(events)

    mismatches: list[dict] = []
    compared = 0
    class_count = 0
    for index, expected_position in enumerate(registrars):
        if index >= len(positions):
            mismatches.append({"path": f"positions[{index}]", "kind": "missing_position",
                               "expected_registrar": expected_position.get("registrar"),
                               "expected_class_ids": expected_position.get("class_ids")})
            break
        observed_position = positions[index]
        compared += 1
        expected_classes = expected_position.get("classes") or []
        observed_classes = observed_position.get("classes") or []
        for class_index, expected_class in enumerate(expected_classes):
            if class_index >= len(observed_classes):
                mismatches.append({"path": f"positions[{index}].classes[{class_index}]",
                                   "kind": "missing_class",
                                   "expected": {"classId": expected_class.get("classId"),
                                                "name": expected_class.get("name"),
                                                "objectSize": expected_class.get("objectSize")}})
                break
            diff = first_value_divergence(expected_class, observed_classes[class_index],
                                          f"positions[{index}].classes[{class_index}]")
            if diff is not None:
                diff["kind"] = "class_divergence"
                diff["registrar"] = expected_position.get("registrar")
                mismatches.append(diff)
                break
        if mismatches:
            break
        if len(observed_classes) > len(expected_classes):
            extra = observed_classes[len(expected_classes)]
            mismatches.append({"path": f"positions[{index}].classes[{len(expected_classes)}]",
                               "kind": "extra_class",
                               "observed": {"classId": extra.get("classId"),
                                            "name": extra.get("name"),
                                            "objectSize": extra.get("objectSize")}})
            break
        class_count += len(expected_classes)

    extra_positions = max(0, len(positions) - len(registrars))
    if extra_positions and not ignore_extra and not mismatches:
        mismatches.append({"path": f"positions[{len(registrars)}]", "kind": "extra_position",
                           "observed_registrar": positions[len(registrars)].get("registrar")})

    return {
        "verdict": ORDER_MATCH if not mismatches else ORDER_MISMATCH,
        "baseline_positions": len(registrars),
        "observed_positions": len(positions),
        "compared_positions": compared,
        "classes_compared": class_count,
        "extra_positions_ignored": extra_positions,
        "mismatch_count": len(mismatches),
        "first_divergence": mismatches[0] if mismatches else None,
        "mismatches": mismatches,
        "accepted_divergences": [
            "relocated code/data addresses are not compared; registrar identity is executed position order",
            "IGI1MUTEX and other declared startup-layout differences are not part of the class oracle",
            "the tail (Flow_DeleteHandler path) is outside the oracle's executed QTask_RegisterClasses positions",
        ],
        "baseline": baseline.get("scope", "retail-class-registration-per-registrar-v1"),
        "retail_sha256": baseline.get("retail_sha256"),
        "trace_retail_sha256": trace.get("executable", {}).get("sha256") if isinstance(trace.get("executable"), dict) else None,
    }


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--baseline", default="docs/fidelity/evidence/retail-registration-per-registrar.json")
    parser.add_argument("--run", required=True, help="candidate registration-probe report JSON")
    parser.add_argument("--out", help="write the comparison JSON here")
    parser.add_argument("--strict-extra-positions", action="store_true",
                        help="fail on registrar positions after the oracle's last executed position")
    args = parser.parse_args(argv)

    try:
        baseline = json.loads(Path(args.baseline).read_text())
        trace = json.loads(Path(args.run).read_text())
        report = compare(baseline, trace, ignore_extra=not args.strict_extra_positions)
    except (OSError, ValueError, json.JSONDecodeError) as error:
        print(f"invalid: {error}", file=sys.stderr)
        return 2

    if args.out:
        Path(args.out).write_text(json.dumps(report, indent=2) + "\n")
    if report["verdict"] == ORDER_MATCH:
        print(f"{ORDER_MATCH}: positions={report['compared_positions']} classes={report['classes_compared']} "
              f"divergences=0 (ignored post-oracle positions: {report['extra_positions_ignored']})")
        return 0
    print(f"{ORDER_MISMATCH}: {json.dumps(report['first_divergence'], sort_keys=True)}", file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
