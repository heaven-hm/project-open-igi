#!/usr/bin/env python3
"""Merge manifest shards into the verification manifest.

The evidence generator writes the whole manifest, so two agents working at once
must not write the same file. Each writes a shard instead, and this merges the
shards into the main manifest.

    python3 scripts/merge_manifest_shards.py build/manifest-shard-*.json

A shard is a manifest with a "natives" list. The merge takes the record from
the shard for every address it holds, so the shard wins over the main manifest,
and leaves every other record alone.
"""

import json
import pathlib
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
MANIFEST = ROOT / "docs" / "fidelity" / "native-verification.json"


def main() -> int:
    if len(sys.argv) < 2 or sys.argv[1] in ("-h", "--help"):
        print("usage: merge_manifest_shards.py <shard.json> [...]")
        print("  merges the shards into docs/fidelity/native-verification.json")
        return 0 if len(sys.argv) > 1 else 1
    manifest = json.loads(MANIFEST.read_text())
    natives = manifest.setdefault("natives", [])
    index = {record.get("address"): position for position, record in enumerate(natives)}

    merged = 0
    for shard_path in sys.argv[1:]:
        shard = json.loads(pathlib.Path(shard_path).read_text())
        for record in shard.get("natives", []):
            address = record.get("address")
            if address in index:
                natives[index[address]] = record
            else:
                index[address] = len(natives)
                natives.append(record)
            merged += 1
        print(f"merged {shard_path}")

    MANIFEST.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    print(f"records merged: {merged}; manifest now holds {len(natives)}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
