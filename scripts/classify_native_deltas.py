#!/usr/bin/env python3
"""Classify structural natives by differential outcome and ABI shape."""

from __future__ import annotations

import hashlib
import json
import pathlib
import re
import subprocess
import sys

ROOT = pathlib.Path("/Users/haseeb-mir/Documents/Code/project-open-igi")
BINARY = "/Users/haseeb-mir/Downloads/IGI/igi.exe"


def sha256(path: pathlib.Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for b in iter(lambda: f.read(1 << 20), b""):
            h.update(b)
    return h.hexdigest()


def comment_for(path: pathlib.Path, address: str) -> tuple[str, str]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
    for i, line in enumerate(lines):
        if line.strip().startswith("// Native:") and address.upper() in line.upper():
            head = line
            note = ""
            for off in range(1, 5):
                if i + off < len(lines) and "Behavior/CFG:" in lines[i + off]:
                    note = lines[i + off]
            return head, note
    return "", ""


def main() -> int:
    manifest = json.loads((ROOT / "docs/fidelity/native-verification.json").read_text())
    binary_sha = sha256(pathlib.Path(BINARY))
    rows = []
    for record in sorted(manifest["natives"], key=lambda r: r["address"]):
        if record["verification"]["status"] != "STRUCTURALLY_VERIFIED":
            continue
        address = record["address"]
        impl = ROOT / record["implementation"]["path"]
        head, note = comment_for(impl, address)
        signature = ""
        match = re.search(r"signature/calling convention ([^.;]+)", head)
        if match:
            signature = match.group(1).strip()
        cmd = [sys.executable, str(ROOT / "scripts/native_leaves_runner.py"),
               "--address", address, "--binary", BINARY, "--binary-sha256", binary_sha,
               "--implementation", str(impl), "--implementation-sha256", sha256(impl),
               "--root", str(ROOT)]
        p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
        out = (p.stdout or "").strip()
        if out.startswith("{"):
            report = json.loads(out)
            failed = [c for c in report["cases"] if c["reference_output"] != c["candidate_output"]]
            if not failed:
                outcome = "PASS"
            else:
                details = "; ".join(f"{c['id']}:ref={c['reference_output']['return_value']},cand={c['candidate_output']['return_value']}" for c in failed[:3])
                outcome = f"MISMATCH({len(failed)}/{report['tests_total']}) {details}"
        else:
            err = (p.stderr or "").strip().splitlines()
            outcome = "ERROR " + (err[-1][:120] if err else "no output")
        rows.append((address, record["name"], signature or "?", outcome, note[:110]))
    for address, name, signature, outcome, note in rows:
        print(f"{address} | {name} | {signature} | {outcome}")
        if note:
            print(f"        note: {note}")
    print(f"total structural: {len(rows)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
