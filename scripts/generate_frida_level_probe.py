#!/usr/bin/env python3
"""Refresh the Frida probe's target list from address-bound implementations."""

from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys


TARGETS_RE = re.compile(r"const targets = \[.*?\n\];", re.DOTALL)


def implemented_targets(root: pathlib.Path, records: list[object]) -> list[dict[str, str]]:
    root = root.resolve()
    targets = []
    for record in records:
        if not isinstance(record, dict):
            continue
        implementation = record.get("implementation")
        if not isinstance(implementation, dict) or not isinstance(implementation.get("path"), str):
            continue
        try:
            source = (root / implementation["path"]).resolve(strict=True)
            source.relative_to(root)
            if not source.is_file():
                continue
            address = int(record["address"], 16)
            if not 0x00400000 <= address < 0x01000000:
                continue
        except (OSError, KeyError, TypeError, ValueError):
            continue
        targets.append({"name": str(record.get("name") or implementation["path"]),
                        "va": f"0x{address:08X}"})
    return sorted(targets, key=lambda item: (item["va"], item["name"]))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=pathlib.Path, default=pathlib.Path("."))
    parser.add_argument("--manifest", type=pathlib.Path, default=pathlib.Path("docs/fidelity/native-verification.json"))
    parser.add_argument("--template", type=pathlib.Path, default=pathlib.Path("scripts/frida_level_probe.js"))
    parser.add_argument("--output", type=pathlib.Path, default=pathlib.Path("build/frida_level_probe.generated.js"))
    args = parser.parse_args()
    root = args.root.resolve()
    manifest_path = args.manifest if args.manifest.is_absolute() else root / args.manifest
    template_path = args.template if args.template.is_absolute() else root / args.template
    output_path = args.output if args.output.is_absolute() else root / args.output
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        template = template_path.read_text(encoding="utf-8")
    except (OSError, json.JSONDecodeError) as error:
        print(f"generate_frida_level_probe: {error}", file=sys.stderr)
        return 2
    records = manifest.get("natives") if isinstance(manifest, dict) else None
    if not isinstance(records, list):
        print("generate_frida_level_probe: manifest has no natives list", file=sys.stderr)
        return 2
    targets = implemented_targets(root, records)
    replacement = "const targets = " + json.dumps(targets, indent=2) + ";"
    rendered, substitutions = TARGETS_RE.subn(replacement, template, count=1)
    if substitutions != 1:
        print("generate_frida_level_probe: template target list was not found exactly once", file=sys.stderr)
        return 2
    try:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(rendered, encoding="utf-8")
    except OSError as error:
        print(f"generate_frida_level_probe: cannot write probe: {error}", file=sys.stderr)
        return 2
    print(f"generated Frida target list: {len(targets)} implemented native(s) -> {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
