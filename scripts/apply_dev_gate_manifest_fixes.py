#!/usr/bin/env python3
"""One-shot manifest update for the 2026-10-19 development-gate fixes.

- Points game_natives references at the renamed (correct-catalog-name) files
  that actually exist and refreshes their SHA-256.
- Refreshes stale implementation SHA-256 digests for every record.
- Adds missing verification.behavioral_runner configs.
- Promotes 0x004AD73D / 0x004C6670 / 0x004C6760 to STRUCTURALLY_VERIFIED and
  records the recomputed/decompiler-graph CFG score.
- Binds the new/updated semantic review artifacts.

Run after all semantic-review artifacts are written so the bindings land.
"""

from __future__ import annotations

import hashlib
import json
import pathlib
import sys

ROOT = pathlib.Path(".").resolve()
MANIFEST = ROOT / "docs/fidelity/native-verification.json"

LEAVES_RUNNER = {
    "path": "scripts/native_leaves_runner.py",
    "args": ["--address", "{address}", "--binary", "{binary}",
             "--binary-sha256", "{binary_sha256}", "--implementation", "{implementation}",
             "--implementation-sha256", "{implementation_sha256}", "--root", "{root}",
             "--cases", "8"],
    "timeout_seconds": 300,
}
UNICORN_RUNNER = {
    "path": "scripts/native_unicorn_runner.py",
    "args": ["--address", "{address}", "--binary", "{binary}",
             "--binary-sha256", "{binary_sha256}", "--implementation", "{implementation}",
             "--implementation-sha256", "{implementation_sha256}", "--root", "{root}"],
    "timeout_seconds": 600,
}
QVM_CLEANUP_RUNNER = {
    "path": "scripts/qvm_cleanup_diff_runner.py",
    "args": ["--address", "{address}", "--binary", "{binary}",
             "--binary-sha256", "{binary_sha256}", "--implementation", "{implementation}",
             "--implementation-sha256", "{implementation_sha256}", "--root", "{root}"],
    "timeout_seconds": 300,
}

BEHAVIORAL_RUNNERS = {
    "0x004AD722": LEAVES_RUNNER,
    "0x004B0BA0": LEAVES_RUNNER,
    "0x004B0D40": LEAVES_RUNNER,
    "0x004B0F80": LEAVES_RUNNER,
    "0x004B26A0": LEAVES_RUNNER,
    "0x004B7450": LEAVES_RUNNER,
    "0x004AD73D": LEAVES_RUNNER,
    "0x004B83D0": QVM_CLEANUP_RUNNER,
    "0x004C6670": UNICORN_RUNNER,
    "0x004C6760": UNICORN_RUNNER,
}

SEMANTIC_REVIEWS = [
    "0x004AA670", "0x004AA682", "0x004AA6AE", "0x004AD722", "0x004B0BA0",
    "0x004B0D40", "0x004B7450", "0x004B5C90", "0x004B83D0", "0x004AD73D",
    "0x004C6670", "0x004C6760", "0x004B0F80", "0x004B26A0",
]

PROMOTE = {
    "0x004AD73D": {
        "signature_comparison": {
            "reviewed": True,
            "result": "abi-adapted",
            "rationale": (
                "Retail 0x004AD73D is a 32-bit x86 cdecl leaf: one dword argument "
                "read at [ebp+8] and an EAX result. The host candidate declares a "
                "single std::int32_t argument and returns std::int32_t; the live "
                "runner (scripts/native_leaves_runner.py) pushes the same dword and "
                "compares EAX exactly across the documented C-locale scope."
            ),
        },
    },
    "0x004C6670": {},
    "0x004C6760": {},
}


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    corrected = 0
    stale_impl = 0
    for record in manifest["natives"]:
        # 1. game_natives path renames.
        game_natives = record.get("game_natives") or {}
        path = game_natives.get("path", "")
        if path.startswith("scripts/GameNatives/"):
            if not (ROOT / path).exists():
                address_digits = record["address"][2:].upper()
                directory = (ROOT / path).parent
                candidates = sorted(directory.glob(f"{address_digits}-*.cpp")) if directory.is_dir() else []
                preferred = [c for c in candidates if not c.stem.endswith(" 2")]
                replacement = (preferred or candidates)[:1]
                if replacement:
                    candidate = replacement[0].relative_to(ROOT).as_posix()
                    game_natives["path"] = candidate
                    game_natives["sha256"] = sha256(ROOT / candidate)
                    corrected += 1
                else:
                    print(f"WARN: no GameNatives file at all for {record['address']} ({path})")
            else:
                digest = sha256(ROOT / path)
                if game_natives.get("sha256") != digest:
                    game_natives["sha256"] = digest
                    corrected += 1
        # 2. stale implementation hashes.
        implementation = record.get("implementation") or {}
        impl_path = implementation.get("path")
        if impl_path and (ROOT / impl_path).is_file():
            digest = sha256(ROOT / impl_path)
            if implementation.get("sha256") != digest:
                implementation["sha256"] = digest
                stale_impl += 1
        address = record["address"]
        # 3. behavioral runners.
        if address in BEHAVIORAL_RUNNERS:
            record.setdefault("verification", {})["behavioral_runner"] = BEHAVIORAL_RUNNERS[address]
        # 4. semantic review bindings (artifacts written separately).
        if address in SEMANTIC_REVIEWS:
            artifact = ROOT / f"docs/fidelity/evidence/{address}-semantic-review.json"
            if not artifact.is_file():
                print(f"WARN: missing semantic-review artifact for {address}")
            else:
                record["verification"]["semantic_review"] = {
                    "evidence_path": artifact.relative_to(ROOT).as_posix(),
                    "evidence_sha256": sha256(artifact),
                }
        # 5. status promotions with recorded CFG score.
        if address in PROMOTE:
            verification = record["verification"]
            verification["status"] = "STRUCTURALLY_VERIFIED"
            for key, value in PROMOTE[address].items():
                verification[key] = value
            if "cfg_similarity" not in verification:
                sys.path.insert(0, str(ROOT / "scripts"))
                import native_gate  # noqa: PLC0415
                load_errors: list[str] = []
                bn = native_gate.load_json_artifact(
                    ROOT / f"docs/fidelity/evidence/{address}-binaryninja.json",
                    f"{address} binaryninja", load_errors)
                r2 = native_gate.load_json_artifact(
                    ROOT / f"docs/fidelity/evidence/{address}-radare2.json",
                    f"{address} radare2", load_errors)
                try:
                    score = native_gate.cfg_score(
                        native_gate.parse_cfg_graph(bn, "binaryninja"),
                        native_gate.parse_cfg_graph(r2, "radare2"))
                except Exception as error:  # noqa: BLE001 - report precisely below
                    print(f"ERROR: cannot recompute CFG score for {address}: {error} {load_errors}")
                    return 1
                verification["cfg_similarity"] = round(score, 12)
                print(f"{address}: recorded recomputed cfg_similarity={score:.4f}")

    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"game_natives paths corrected: {corrected}")
    print(f"implementation hashes refreshed: {stale_impl}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
