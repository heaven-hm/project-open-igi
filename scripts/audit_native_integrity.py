#!/usr/bin/env python3
"""Check that every MVP native's evidence points at real, built code.

Three failure modes are distinguished, because they need different fixes:

  missing-file        the implementation path does not exist on disk
  missing-definition  the path exists but contains no definition of the native
  not-in-build        an engine implementation exists but CMake never compiles it

Canonical sources under scripts/GameNatives are reference transcriptions and are not
expected to be compiled, so they are checked for a definition only.
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent


def load_json(path: pathlib.Path):
    return json.loads(path.read_text())


def cmake_sources() -> set[str]:
    text = (ROOT / "CMakeLists.txt").read_text()
    return {m for m in re.findall(r"engine/[A-Za-z0-9_./-]+\.cpp", text)}


def spellings(name: str) -> set[str]:
    """The catalog spells some IFF natives Iff_* where the port spells them Ilff_*."""
    out = {name}
    if name.startswith("Iff_"):
        out.add("Ilff_" + name[4:])
    return out


def definition_pattern(name: str) -> re.Pattern[str]:
    alternatives = "|".join(re.escape(s) for s in sorted(spellings(name)))
    return re.compile(rf"^[A-Za-z_][^\n;{{}}]*\b(?:{alternatives})\s*\(", re.M)


def defined_anywhere(name: str, sources: dict[pathlib.Path, str]) -> list[str]:
    pattern = definition_pattern(name)
    return [str(p.relative_to(ROOT)) for p, text in sources.items() if pattern.search(text)]


RANK = {"ANALYZED": 0, "TRANSCRIBED": 1, "STRUCTURALLY_VERIFIED": 2, "BUILD_VERIFIED": 3,
        "RUNTIME_TESTED": 4, "VERIFIED": 5, "VERIFIED_WITH_DOCUMENTED_SCOPE": 5}


def check(record: dict, built: set[str], sources: dict[pathlib.Path, str]) -> tuple[str, str]:
    impl = (record.get("implementation") or {})
    path_text = impl.get("path") or ""
    if not path_text:
        return "missing-file", "record names no implementation path"
    path = ROOT / path_text
    if not path.exists():
        return "missing-file", f"{path_text} does not exist"
    name = record["name"]
    text = path.read_text(errors="replace")
    status = record["verification"]["status"]
    if definition_pattern(name).search(text) is None and RANK.get(status, 0) == 0:
        # An ANALYZED record only claims the retail body was read; the transcription may be a
        # non-compiling stub, so the address-bound provenance line is evidence enough.
        address = record["address"][2:].upper()
        if address in text.upper():
            return "ok", f"ANALYZED; {path_text} mentions the address but has no compilable definition"
        return "missing-definition", f"{path_text} neither defines {name} nor mentions {record['address']}"
    if not definition_pattern(name).search(text):
        elsewhere = defined_anywhere(name, sources)
        if not elsewhere:
            return "missing-definition", f"no definition of {name} anywhere under engine/src"
        return "wrong-impl-path", f"{path_text} does not define it; the definition is in {elsewhere[0]}"
    canonical = path_text.startswith("scripts/GameNatives/")
    if canonical:
        return "ok", f"canonical reference source {path_text}"
    if path_text.startswith("engine/") and path_text not in built:
        return "not-in-build", f"{path_text} is not in the CMake source list"
    return "ok", f"{path_text} is compiled into the engine"


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", type=pathlib.Path, help="write the full report here")
    args = parser.parse_args()

    targets = load_json(ROOT / "scripts/mvp_native_targets.json")
    manifest = {
        r["address"].lower(): r for r in load_json(ROOT / "docs/fidelity/native-verification.json")["natives"]
    }
    built = cmake_sources()
    sources = {p: p.read_text(errors="replace") for p in (ROOT / "engine/src").rglob("*.cpp")}
    failures: list[dict] = []
    counts: dict[str, int] = {}
    for target in targets:
        address = target["address"].lower()
        record = manifest.get(address)
        if record is None:
            counts["no-record"] = counts.get("no-record", 0) + 1
            failures.append({"address": target["address"], "name": target["name"],
                             "problem": "no-record", "detail": "no verification record"})
            continue
        problem, detail = check(record, built, sources)
        counts[problem] = counts.get(problem, 0) + 1
        if problem != "ok":
            failures.append({"address": target["address"], "name": target["name"],
                             "status": record["verification"]["status"],
                             "problem": problem, "detail": detail})
    report = {"schema_version": 1, "kind": "native-integrity-audit",
              "checked": len(targets), "counts": counts, "failures": failures}
    if args.json:
        args.json.write_text(json.dumps(report, indent=2) + "\n")
    for failure in failures:
        print(f'{failure["address"]} {failure["name"][:34]:34s} {failure["status"]:22s} '
              f'{failure["problem"]:18s} {failure["detail"]}')
    print(f"\nchecked {len(targets)}: " + ", ".join(f"{k}={v}" for k, v in sorted(counts.items())))
    return 0


if __name__ == "__main__":
    sys.exit(main())
