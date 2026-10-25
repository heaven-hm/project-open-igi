#!/usr/bin/env python3
"""Report how much of a retail function's direct-call closure is transcribed.

Why this exists: counts of "how much is left" drifted because the manifest's
``implementation.path`` points at external ``scripts/GameNatives`` candidate
files for many records, and a candidate file is not an engine transcription.
This tool answers the narrow question instead - does an engine source implement
this retail VA - and keeps candidate-only matches in their own bucket so the
distinction is visible rather than folded into a total.

It reads the retail disassembly listing (``build/retail-dis.txt``) for the
function's own ``call`` instructions, so it counts the retail call sites, not a
decompiler's idea of the callee list.

Usage:
    python3 scripts/native_coverage.py 0x00402930 --size 0x30C
    python3 scripts/native_coverage.py 0x0048F530 --size 0x3E2 --json
    python3 scripts/native_coverage.py --spans spans.txt      # "name lo hi" lines

Status buckets per distinct callee:
    engine          an engine source implements it (file name or provenance VA)
    import          a PE import table slot (indirect call through the IAT, not a game native)
    crt             a known statically linked CRT routine (no implementation unit)
    candidate-only  only an external scripts/GameNatives candidate mentions it
    absent          nothing in the tree implements it
"""

from __future__ import annotations

import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Statically linked CRT routines identified in evidence; these need no unit.
CRT = {
    0x004A5000: "MSVC VC20 exception-registration record builder",
    0x004A53B3: "sprintf",
    0x004A54A0: "MSVC __chkstk 4KB stack probe (compiler-emitted, never hand-transcribed)",
    0x004A5500: "strncpy",
    0x004A5AD5: "exit (doexit wrapper)",
    0x005169B0: "strncat",
}

INSN = re.compile(r"^\s*([0-9a-f]+):\s+([0-9a-f ]+?)\s{2,}(\S+)\s*(.*)$")
TARGET = re.compile(r"0x([0-9a-f]+)")


def load_listing(path: Path) -> list[tuple[int, str, str]]:
    rows: list[tuple[int, str, str]] = []
    for line in path.read_text(errors="replace").splitlines():
        m = INSN.match(line)
        if m:
            rows.append((int(m.group(1), 16), m.group(3), m.group(4)))
    return rows


def calls_in(rows: list[tuple[int, str, str]], lo: int, hi: int) -> list[int]:
    targets: list[int] = []
    for va, mnemonic, rest in rows:
        if lo <= va < hi and mnemonic == "call":
            m = TARGET.search(rest)
            if m:
                targets.append(int(m.group(1), 16))
    return targets


def engine_definitions(root: Path) -> dict[int, str]:
    """Map retail VA -> engine source that implements it."""
    out: dict[int, str] = {}
    for path in (root / "engine/src").rglob("*.cpp"):
        m = re.match(r"native_([0-9a-fA-F]{6,8})\.cpp$", path.name)
        if m:
            out.setdefault(int("0x" + m.group(1), 16), str(path.relative_to(root)))
        text = path.read_text(errors="replace").lower()
        for hit in re.finditer(r"retail va (0x[0-9a-f]{8})", text):
            out.setdefault(int(hit.group(1), 16), str(path.relative_to(root)))
        # Files written before the provenance block was standardised head a definition with
        # "// 0x0048F1E0 Name" instead. Treat that as implementation evidence only when the heading
        # is immediately followed by code (so an inline "// 0x... call" note cannot match).
        lines = text.splitlines()
        for index, line in enumerate(lines):
            m = re.match(r"// 0x([0-9a-f]{8})\b", line.strip())
            if not m:
                continue
            for follower in lines[index + 1:index + 5]:
                stripped = follower.strip()
                if not stripped or stripped.startswith("//"):
                    continue
                out.setdefault(int(m.group(1), 16), str(path.relative_to(root)))
                break
    return out


def import_slots(pe_path: Path) -> dict[int, str]:
    """PE import IAT slots, so an indirect call through the IAT is not read as a missing native."""
    try:
        out = subprocess.run(["rabin2", "-i", str(pe_path)], capture_output=True, text=True, timeout=120).stdout
    except (OSError, subprocess.SubprocessError):
        return {}
    slots: dict[int, str] = {}
    for line in out.splitlines():
        parts = line.split()
        if len(parts) >= 6 and parts[1].startswith("0x") and parts[3] == "FUNC":
            try:
                slots[int(parts[1], 16)] = f"{parts[4]}!{parts[5]}"
            except ValueError:
                continue
    return slots


def candidate_mentions(root: Path) -> dict[int, str]:
    out: dict[int, str] = {}
    base = root / "scripts/GameNatives"
    if not base.is_dir():
        return out
    for path in base.rglob("*"):
        if not path.is_file():
            continue
        for hit in re.finditer(r"[0-9A-Fa-f]{8}", path.name):
            out.setdefault(int(hit.group(0), 16), str(path.relative_to(root)))
    return out


def report(root: Path, listing: list[tuple[int, str, str]], name: str, lo: int, hi: int,
           engine: dict[int, str], candidates: dict[int, str], imports: dict[int, str]) -> dict:
    sites = calls_in(listing, lo, hi)
    distinct = sorted(set(sites))
    rows = []
    for va in distinct:
        if va in engine:
            status, where = "engine", engine[va]
        elif va in imports:
            status, where = "import", imports[va]
        elif va in CRT:
            status, where = "crt", CRT[va]
        elif va in candidates:
            status, where = "candidate-only", candidates[va]
        else:
            status, where = "absent", ""
        rows.append({"va": f"0x{va:08X}", "status": status, "where": where,
                     "sites": sum(1 for s in sites if s == va)})
    counts = {k: sum(1 for r in rows if r["status"] == k)
              for k in ("engine", "import", "crt", "candidate-only", "absent")}
    return {"name": name, "lo": f"0x{lo:08X}", "hi": f"0x{hi:08X}",
            "call_sites": len(sites), "distinct_callees": len(distinct),
            "counts": counts, "callees": rows}


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("address", nargs="?", help="retail function start, e.g. 0x00402930")
    ap.add_argument("--size", help="extent in bytes or 0x-hex, e.g. 0x30C")
    ap.add_argument("--spans", help="file with 'name lo hi' lines, one function per line")
    ap.add_argument("--root", default=str(ROOT))
    ap.add_argument("--listing", default="build/retail-dis.txt")
    ap.add_argument("--pe", default=str(Path("/Users/haseeb-mir/Downloads/IGI/igi.exe")),
                    help="retail image, used to classify IAT import slots")
    ap.add_argument("--json", action="store_true")
    args = ap.parse_args()

    root = Path(args.root)
    listing_path = Path(args.listing)
    if not listing_path.is_absolute():
        listing_path = root / listing_path
    if not listing_path.exists():
        print(f"no disassembly listing at {listing_path}", file=sys.stderr)
        return 2

    listing = load_listing(listing_path)
    engine = engine_definitions(root)
    candidates = candidate_mentions(root)
    imports = import_slots(Path(args.pe)) if Path(args.pe).exists() else {}

    spans: list[tuple[str, int, int]] = []
    if args.spans:
        for line in Path(args.spans).read_text().splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) != 3:
                print(f"bad span line: {line}", file=sys.stderr)
                return 2
            spans.append((parts[0], int(parts[1], 0), int(parts[2], 0)))
    elif args.address and args.size:
        lo = int(args.address, 0)
        spans.append((f"function@{args.address}", lo, lo + int(args.size, 0)))
    else:
        ap.error("give ADDRESS --size, or --spans FILE")

    results = [report(root, listing, *span, engine, candidates, imports) for span in spans]
    if args.json:
        print(json.dumps(results, indent=2))
        return 0

    for r in results:
        c = r["counts"]
        total = r["distinct_callees"]
        done = c["engine"] + c["crt"] + c["import"]
        print(f"\n{r['name']}  {r['lo']}..{r['hi']}")
        print(f"  {r['call_sites']} call sites, {total} distinct callees")
        print(f"  engine ................. {c['engine']}")
        print(f"  import (IAT slot) ...... {c['import']}")
        print(f"  crt (no unit needed) ... {c['crt']}")
        print(f"  candidate-only ......... {c['candidate-only']}")
        print(f"  absent ................. {c['absent']}")
        if total:
            print(f"  closure accounted for .. {done}/{total} ({100.0 * done / total:.1f}%), "
                  f"absent {100.0 * c['absent'] / total:.1f}%")
        for row in r["callees"]:
            if row["status"] in ("candidate-only", "absent"):
                print(f"    {row['status']:<15}{row['va']}  x{row['sites']}"
                      + (f"  {row['where']}" if row["where"] else ""))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
