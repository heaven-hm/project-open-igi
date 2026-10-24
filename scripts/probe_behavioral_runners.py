#!/usr/bin/env python3
"""Try each candidate behavioral runner against the natives that need one.

A native only gets a behavioral_runner config when a runner actually produces a
passing differential report for it, so this probes the available runners and
reports which ones can serve each native.
"""

import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"
IGI = "/Users/haseeb-mir/Downloads/IGI/igi.exe"
IGI_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

RUNNERS = [
    "scripts/native_leaves_runner.py",
    "scripts/native_diff_runner.py",
    "scripts/native_unicorn_runner.py",
    "scripts/hash_lookup_diff_runner.py",
    "scripts/resource_register_buffer_diff_runner.py",
    "scripts/qvm_load_diff_runner.py",
    "scripts/resource_lifecycle_diff_runner.py",
    "scripts/resource_sys_diff_runner.py",
    "scripts/texture_loader_diff_runner.py",
    "scripts/native_heaplist_diff_runner.py",
    "scripts/format_diff_runner.py",
    "scripts/frame_diff_runner.py",
    "scripts/attachment_diff_runner.py",
    "scripts/error_show_diff_runner.py",
    "scripts/native_mem_alloc_diff_runner.py",
    "scripts/mtp_package_diff_runner.py",
    "scripts/model_mesh_diff_runner.py",
    "scripts/native_abi_diff_runner.py",
]

ARGS = ["--address", "{address}", "--binary", "{binary}",
        "--binary-sha256", "{binary_sha256}",
        "--implementation", "{implementation}",
        "--implementation-sha256", "{implementation_sha256}",
        "--root", "{root}"]


def sha256(path: Path) -> str:
    import hashlib
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def main() -> None:
    manifest = json.loads(MANIFEST.read_text())
    wanted = sys.argv[1:]
    results = {}
    for rec in manifest["natives"]:
        if rec["verification"].get("behavioral_runner"):
            continue
        addr = rec["address"]
        if wanted and addr.upper() not in {w.upper() for w in wanted}:
            continue
        impl = ROOT / rec["implementation"]["path"]
        if not impl.exists():
            continue
        impl_sha = sha256(impl)
        found = None
        for runner in RUNNERS:
            rp = ROOT / runner
            if not rp.exists():
                continue
            cmd = [sys.executable, str(rp)]
            for a in ARGS:
                cmd.append(a.format(address=addr, binary=IGI, binary_sha256=IGI_SHA,
                                    implementation=str(impl),
                                    implementation_sha256=impl_sha, root=str(ROOT)))
            try:
                p = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True,
                                   timeout=300)
            except subprocess.TimeoutExpired:
                continue
            out = p.stdout.strip()
            if not out.startswith("{"):
                continue
            try:
                data = json.loads(out)
            except json.JSONDecodeError:
                continue
            if data.get("result") == "passed":
                found = (runner, data.get("tests_passed"), data.get("tests_total"))
                break
        results[addr] = {"name": rec["name"], "runner": found[0] if found else None,
                         "tests": f"{found[1]}/{found[2]}" if found else None}
        print(f"{addr} {rec['name']:<38} -> {found[0] if found else 'NO RUNNER'} "
              f"{results[addr]['tests'] or ''}", flush=True)
    (ROOT / "assets" / "runner_probe.json").write_text(json.dumps(results, indent=2))


if __name__ == "__main__":
    main()
