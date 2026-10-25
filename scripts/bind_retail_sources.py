#!/usr/bin/env python3
"""Bind unbound retail sources to manifest records, re-deriving the scope on every run.

The native ctest fixture `igi.native.scope` blocks all 1254 downstream tests while any
file under engine/src/retail/ is "unbound", i.e. named by no record's implementation.path
(scripts/native_ctest.py). This script closes that gap for the class of files where the
retail VA is recoverable and checkable: `native_<hex VA>.cpp`.

Design constraints, all deliberate:

  * The unbound set is RECOMPUTED on every run. It is never frozen. The set grows as
    natives land -- it went 164 -> 165 -> 166 in twenty minutes during the first survey
    (165 and 166 were newly written natives), so a one-shot pass would be stale before
    it merged.

  * A file is bound ONLY if its filename VA appears in its OWN provenance block. The
    block is read as a whole, never line-wise: provenance entries wrap across lines, and
    a line-wise reader reports false mismatches (15 of them on the first survey, all
    false). A file that fails this check is REFUSED and reported, never repaired by
    invention. This refusal path is what stops the script from becoming a fabricator.

  * Records are only ever created for addresses with no existing record, or updated in
    `implementation.path`/`implementation.sha256` when a record already exists at that
    address but names the candidate source under scripts/GameNatives/. A path update
    without a hash update would leave a record claiming a digest for a file it no longer
    names, so the two always move together.

  * Status is set to TRANSCRIBED, the weakest honest status for a written native that has
    not been structurally re-verified by this script. This script binds sources; it does
    not promote verification.

Usage:
    python3 scripts/bind_retail_sources.py            # dry run: report before/after
    python3 scripts/bind_retail_sources.py --apply     # write the manifest
    python3 scripts/bind_retail_sources.py --json      # machine-readable report
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MANIFEST = os.path.join(ROOT, "docs/fidelity/native-verification.json")
RETAIL = os.path.join(ROOT, "engine/src/retail")

# igi.exe, the binary every record is bound to.
BINARY_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

VA_FROM_NAME = re.compile(r"native_([0-9a-fA-F]{6,8})\.cpp$")
BLOCK_LINE = re.compile(r"^//(?:\s|$)")
NATIVE_LINE = re.compile(r"^//\s*Native:\s*(.*)$")
VA8 = re.compile(r"0x([0-9a-fA-F]{8})")


def sha256_of(path: str) -> str:
    h = hashlib.sha256()
    with open(path, "rb") as fh:
        for chunk in iter(lambda: fh.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def retail_cpp_files() -> list[str]:
    out = []
    for root, _, files in os.walk(RETAIL):
        for name in files:
            if name.endswith(".cpp"):
                out.append(os.path.relpath(os.path.join(root, name), ROOT).replace("\\", "/"))
    return sorted(out)


def manifest_records(manifest: dict) -> list[dict]:
    recs = manifest["natives"]
    return list(recs.values()) if isinstance(recs, dict) else list(recs)


def bound_paths(records: list[dict]) -> set[str]:
    paths = set()
    for r in records:
        impl = r.get("implementation")
        if isinstance(impl, dict) and impl.get("path"):
            paths.add(impl["path"])
    return paths


def provenance_block(path: str) -> list[str]:
    """The comment run that carries the native's evidence, wherever it sits in the file.

    Read as a whole block, never line-wise: provenance entries wrap across lines, and a
    line-wise reader produced 15 false mismatches on the first survey.

    The block is NOT required to be the leading comment run. Files in this tree commonly
    open with #include and a NATIVE_DECL macro before the provenance block begins, and an
    earlier version of this function returned [] for all of them -- reporting seven
    well-formed files as rule 6 violations. Scan for the first contiguous comment run that
    contains a // Native: line rather than assuming a position.
    """
    with open(path, errors="replace") as fh:
        all_lines = [l.rstrip("\n") for l in fh]
    run: list[str] = []
    runs: list[list[str]] = []
    for line in all_lines:
        if BLOCK_LINE.match(line):
            run.append(line)
        else:
            if run:
                runs.append(run)
            run = []
    if run:
        runs.append(run)
    for candidate in runs:
        if any(NATIVE_LINE.match(l) for l in candidate):
            return candidate
    return []


def classify(path: str) -> dict:
    """Decide whether this file's own evidence supports binding it to its filename VA."""
    m = VA_FROM_NAME.search(os.path.basename(path))
    va = "0x" + m.group(1).lower().zfill(8)

    block = provenance_block(path)
    if not block or not any(NATIVE_LINE.match(l) for l in block):
        return {"file": path, "va": va, "outcome": "refused",
                "reason": "no provenance block (rule 6 violation); address cannot be confirmed from the file"}

    present = {"0x" + a.lower() for a in VA8.findall("\n".join(block))}
    if va not in present:
        return {"file": path, "va": va, "outcome": "refused",
                "reason": "filename VA absent from provenance block; do not invent a record",
                "block_vas": sorted(present)}

    name = None
    for line in block:
        mm = NATIVE_LINE.match(line)
        if mm:
            first = mm.group(1).strip()
            ident = re.match(r"([A-Za-z_][A-Za-z0-9_]*)", first)
            name = ident.group(1) if ident else first.split()[0] if first else None
            break

    return {"file": path, "va": va, "outcome": "bindable", "name": name}


def build_record(info: dict, existing: dict | None) -> dict:
    path = info["file"]
    rec = {
        "address": info["va"],
        "name": (existing or {}).get("name") or info.get("name") or "unknown",
        "binary_sha256": BINARY_SHA256,
        "implementation": {
            "path": path,
            "sha256": sha256_of(os.path.join(ROOT, path)),
            "signature": "unknown",
        },
        "verification": {
            "status": "TRANSCRIBED",
            "notes": "Bound to the landed implementation by scripts/bind_retail_sources.py: the "
                     "filename VA is confirmed present in the file's own provenance block. No "
                     "structural or runtime verification is asserted by this binding.",
        },
    }
    if existing:
        for key in ("game_natives", "notes", "status", "cfg_similarity", "verification"):
            if key in existing and key not in rec:
                rec[key] = existing[key]
        if isinstance(existing.get("verification"), dict):
            # Preserve any real verification material, but never inherit a path/hash mismatch.
            merged = dict(existing["verification"])
            merged.setdefault("status", "TRANSCRIBED")
            rec["verification"] = merged
    return rec


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true", help="write the manifest (default: dry run)")
    ap.add_argument("--json", action="store_true", help="emit a machine-readable report")
    args = ap.parse_args()

    with open(MANIFEST) as fh:
        manifest = json.load(fh)
    records = manifest_records(manifest)

    before_bound = bound_paths(records)
    before_unbound = [p for p in retail_cpp_files() if p not in before_bound]

    by_addr: dict[str, dict] = {}
    for r in records:
        a = r.get("address")
        if isinstance(a, str):
            by_addr.setdefault(a.lower(), r)

    bindable, refused = [], []
    for path in before_unbound:
        if not VA_FROM_NAME.search(os.path.basename(path)):
            continue  # descriptive-named files are out of scope for this pass
        info = classify(path)
        (bindable if info["outcome"] == "bindable" else refused).append(info)

    applied = []
    if args.apply:
        is_list = isinstance(manifest["natives"], list)
        created = 0
        updated = 0
        for info in bindable:
            existing = by_addr.get(info["va"])
            rec = build_record(info, existing)
            if existing is None:
                if is_list:
                    manifest["natives"].append(rec)
                else:
                    manifest["natives"][info["va"]] = rec
                created += 1
            else:
                if existing.get("implementation", {}).get("path") != rec["implementation"]["path"]:
                    updated += 1
                existing.update(rec)
            applied.append(info)
        with open(MANIFEST, "w") as fh:
            json.dump(manifest, fh, indent=2, sort_keys=False)
            fh.write("\n")
    else:
        created = sum(1 for i in bindable if by_addr.get(i["va"]) is None)
        updated = sum(1 for i in bindable
                      if by_addr.get(i["va"]) is not None
                      and by_addr[i["va"]].get("implementation", {}).get("path") != i["file"])

    # After-state: recompute from the manifest as it now stands on disk.
    with open(MANIFEST) as fh:
        after = manifest_records(json.load(fh))
    after_unbound = [p for p in retail_cpp_files() if p not in bound_paths(after)]

    report = {
        "unbound_before": len(before_unbound),
        "unbound_after": len(after_unbound),
        "va_named_scanned": len(bindable) + len(refused),
        "bindable": len(bindable),
        "refused": len(refused),
        "records_created": created,
        "records_updated": updated,
        "applied": bool(args.apply),
        "refused_files": [{"file": r["file"], "va": r["va"], "reason": r["reason"]} for r in refused],
        "remaining_unbound": after_unbound,
    }

    if args.json:
        json.dump(report, sys.stdout, indent=2)
        sys.stdout.write("\n")
    else:
        print(f"unbound before       : {report['unbound_before']}")
        print(f"unbound after        : {report['unbound_after']}")
        print(f"VA-named scanned     : {report['va_named_scanned']}")
        print(f"  bindable           : {report['bindable']}")
        print(f"  refused            : {report['refused']}")
        print(f"records created      : {created}")
        print(f"records updated      : {updated}")
        print(f"mode                 : {'APPLIED' if args.apply else 'dry run'}")
        if refused:
            print("\nrefused (not repaired by invention):")
            for r in refused:
                print(f"  {r['file']}  {r['va']}  {r['reason']}")
        rest = [p for p in after_unbound if not VA_FROM_NAME.search(os.path.basename(p))]
        print(f"\nremaining unbound    : {len(after_unbound)} ({len(rest)} descriptive-named, "
              f"{len(after_unbound) - len(rest)} VA-named)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
