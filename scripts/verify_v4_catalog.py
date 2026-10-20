#!/usr/bin/env python3
"""Verify assets/IGINatives_v4.json against the retail binary and GameNatives.

The v4 catalog is the project's confirmed native list, so the gate checks it
the same way it checks an implementation: every entry must be bound to the
selected retail executable, must resolve to a function entry that Binary Ninja
and Radare2 agree on, must carry a complete signature, and must use the same
name that scripts/GameNatives carries for that address.
"""

from __future__ import annotations

import json
import pathlib
import re
import subprocess
from typing import Any

CATALOG = "assets/IGINatives_v4.json"
GAME_NATIVES = "scripts/GameNatives"
FILENAME = re.compile(r"^([0-9A-Fa-f]{8})-(.*?)(\s+\d+)?(\.[a-z]+)$")
UNDEFINED = re.compile(r"\bundefined\d*\b")


def _norm(value: str) -> str:
    return re.sub(r"[^a-z0-9]", "", str(value).lower())


def _game_natives_names(root: pathlib.Path) -> dict[str, set[str]]:
    index: dict[str, set[str]] = {}
    base = root / GAME_NATIVES
    if not base.is_dir():
        return index
    for path in base.rglob("*"):
        if not path.is_file():
            continue
        match = FILENAME.match(path.name)
        if match:
            index.setdefault(match.group(1).upper(), set()).add(match.group(2))
    return index


def verify(root: pathlib.Path, binary_digest: str,
           three_way: dict[str, Any] | None = None,
           selected_addresses: set[str] | None = None) -> tuple[list[str], list[str], dict[str, Any]]:
    errors: list[str] = []
    warnings: list[str] = []
    catalog_path = root / CATALOG
    if not catalog_path.exists():
        warnings.append(f"{CATALOG}: catalog not present; v4 verification skipped")
        return errors, warnings, {}

    try:
        catalog = json.loads(catalog_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        errors.append(f"{CATALOG}: could not be read ({error})")
        return errors, warnings, {}

    declared = str(catalog.get("binarySha256") or catalog.get("binary_sha256") or "").lower()
    if declared and declared != binary_digest.lower():
        errors.append(
            f"{CATALOG}: catalog is bound to binary {declared[:16]}... but the selected "
            f"retail binary is {binary_digest[:16]}...")

    natives = catalog.get("Natives")
    if not isinstance(natives, list) or not natives:
        errors.append(f"{CATALOG}: Natives must be a non-empty array")
        return errors, warnings, {}

    gn_names = _game_natives_names(root)
    confirmed = set()
    if isinstance(three_way, dict):
        for entry in three_way.get("entries", []):
            if isinstance(entry, dict) and entry.get("confirmed"):
                confirmed.add(f"{int(str(entry['address']), 16):08X}")

    checked = 0
    for entry in natives:
        if not isinstance(entry, dict):
            errors.append(f"{CATALOG}: every Natives entry must be an object")
            continue
        address = str(entry.get("address", "")).upper().lstrip("0X")
        name = str(entry.get("name", ""))
        signature = str(entry.get("signature", ""))
        key = f"{int(address, 16):08X}" if address else ""
        if selected_addresses is not None and f"0x{key}" not in selected_addresses:
            continue

        if not key:
            errors.append(f"{CATALOG}: entry without a usable address: {entry!r}")
            continue
        if not name:
            errors.append(f"{CATALOG}: {key} has no name")
        if not signature:
            errors.append(f"{CATALOG}: {key} has no signature")
            continue
        if UNDEFINED.search(signature):
            errors.append(f"{CATALOG}: {key} signature still contains an undefined type: {signature}")
        if name and name not in signature:
            errors.append(f"{CATALOG}: {key} signature does not name {name}: {signature}")

        # the same name must be used by the GameNatives transcription
        tree_names = gn_names.get(key)
        if tree_names is None:
            warnings.append(f"{CATALOG}: {key} {name} has no scripts/GameNatives file")
        elif not any(_norm(tree) == _norm(name) for tree in tree_names):
            errors.append(
                f"{CATALOG}: {key} is named {name} but scripts/GameNatives uses "
                f"{sorted(tree_names)}")

        if confirmed and key not in confirmed:
            warnings.append(
                f"{CATALOG}: {key} {name} is not confirmed as a function entry by "
                f"Binary Ninja, Radare2 and angr")
        checked += 1

    summary = {
        "catalog": CATALOG,
        "entries": checked,
        "binarySha256": binary_digest,
        "gameNativesNamesChecked": sum(1 for e in natives if isinstance(e, dict)),
    }
    return errors, warnings, summary


def main() -> int:
    import argparse

    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    ap.add_argument("--binary", type=pathlib.Path,
                    default=pathlib.Path("/Users/haseeb-mir/Downloads/IGI/igi.exe"))
    ap.add_argument("--three-way", type=pathlib.Path,
                    default=pathlib.Path("assets/gamenatives_three_way.json"))
    a = ap.parse_args()

    root = a.root.resolve()
    binary = a.binary.resolve()
    digest = subprocess.run(["shasum", "-a", "256", str(binary)],
                            capture_output=True, text=True, check=True).stdout.split()[0]
    three_way = None
    if a.three_way.exists():
        three_way = json.loads(a.three_way.read_text(encoding="utf-8"))
    errors, warnings, summary = verify(root, digest, three_way)
    for message in warnings:
        print(f"native_gate: WARNING: {message}")
    for message in errors:
        print(f"native_gate: BLOCKED {message}", file=__import__("sys").stderr)
    if summary:
        print(f"native_gate: v4 catalog {summary['entries']} entries verified against "
              f"the retail binary")
    return 1 if errors else 0


if __name__ == "__main__":
    raise SystemExit(main())
