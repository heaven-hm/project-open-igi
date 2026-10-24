#!/usr/bin/env python3
"""Rebind the candidate section of existing static captures to the current
deployed candidate binary.

The retail static structure (BN/r2/angr/evidence) does not change when the
candidate is rebuilt. What does change is the candidate symbol address and the
candidate image identity. This re-runs `nm`/`objdump` on the exact deployed
binary and rewrites only `candidate_facts`, so a record never claims a symbol
address from a superseded build.

Usage:
  rebind_candidate_facts.py --run-dir build/tester/<run> \
      --candidate build-win32-no-verify-gate/igi1.exe \
      --map '{"0x004C1800": "QTaskList_New", ...}'
"""
from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import re
import subprocess
from pathlib import Path

NM = ["i686-w64-mingw32-nm", "-C"]
OBJDUMP = ["i686-w64-mingw32-objdump", "-d", "-M", "intel"]


def nm_symbols(path: Path) -> list[tuple[str, str, str]]:
    out = subprocess.run(NM + [str(path)], capture_output=True, text=True)
    rows = []
    for line in out.stdout.splitlines():
        parts = line.split(None, 2)
        if len(parts) == 3 and re.fullmatch(r"[0-9a-fA-F]{8}", parts[0]) and parts[1] in "TtWwVv":
            rows.append((parts[2].strip(), parts[0].lower(), parts[1]))
    return rows


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--run-dir", type=Path, required=True)
    ap.add_argument("--candidate", type=Path, required=True)
    ap.add_argument("--map", required=True, help="JSON object retail VA -> nm symbol name")
    ap.add_argument("--addresses", nargs="*", help="limit to these retail VAs")
    args = ap.parse_args()

    names = {f"0x{int(key, 0):08x}": value for key, value in json.loads(args.map).items()}
    path = args.candidate.resolve()
    sha = hashlib.sha256(path.read_bytes()).hexdigest()
    rows = nm_symbols(path)
    stamp = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    updated = []
    missing = []
    for static_path in sorted((args.run_dir / "static").glob("*-static.json")):
        doc = json.loads(static_path.read_text())
        address = doc.get("address", "").lower()
        address = f"0x{int(address, 0):08x}"
        if args.addresses and address not in {f"0x{int(a, 0):08x}" for a in args.addresses}:
            continue
        wanted = names.get(address)
        if not wanted:
            continue
        hits = [row for row in rows if row[0] == wanted or row[0].endswith("_" + wanted)
                or wanted in row[0]]
        if not hits:
            missing.append((address, wanted))
            doc["candidate_facts"] = dict(
                candidate=str(path), sha256=sha, symbols={},
                note=f"symbol {wanted} not present in this candidate build (nm -C, {stamp})",
            )
        else:
            symbols = []
            for demangled, va, kind in hits:
                start = int(va, 16)
                disasm = subprocess.run(
                    OBJDUMP + [f"--start-address=0x{start:08x}",
                               f"--stop-address=0x{start + 0x60:08x}", str(path)],
                    capture_output=True, text=True)
                symbols.append(dict(symbol=demangled, address=va, type=kind, demangled=demangled,
                                    candidate_caller_lines=[],
                                    disassembly=dict(command=disasm.args, exit_code=disasm.returncode,
                                                     stdout=disasm.stdout)))
            doc["candidate_facts"] = dict(candidate=str(path), sha256=sha,
                                          symbols={wanted: symbols},
                                          note=f"re-resolved by nm -C on the exact deployed image, {stamp}")
        doc.setdefault("candidate_rebind", []).append(dict(sha256=sha, stamp=stamp,
                                                           symbol=wanted,
                                                           present=bool(hits)))
        static_path.write_text(json.dumps(doc, indent=2) + "\n")
        updated.append((address, wanted, hits[0][1] if hits else None))

    print(f"candidate {sha}")
    for address, wanted, va in updated:
        print(f"  {address} {wanted}: " + (f"0x{va}" if va else "ABSENT"))
    if missing:
        print("absent symbols:", missing)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
