#!/usr/bin/env python3
"""Read-only corpus checks for layouts observed at retail 0x4C6240 and 0x510010.

This is verification tooling, not a terrain loader or rendering implementation.
No editor/converter/C# parser implementation is imported.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct

RETAIL_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"


def grid_table(path, heightmap):
    if path is None:
        return {"present": False, "status": "optional_absent"}
    content = path.read_bytes()
    result = {"present": True, "bytes": len(content), "sha256": hashlib.sha256(content).hexdigest()}
    if len(content) < 0xC00:
        result.update(status="retail_zero_table_fallback", occupied=0)
        return result
    cursor = 0xC00
    occupied = 0
    for record_index in range(256):
        dimension = struct.unpack_from("<I", content, record_index * 12 + 8)[0]
        if dimension:
            occupied += 1
            if heightmap:
                cursor += (dimension + 1) * (dimension + 1)
            else:
                # The native truncates dimension^2 / 8; it does not round upward.
                square = (dimension * dimension) & 0xFFFFFFFF
                signed_square = square if square < 0x80000000 else square - 0x100000000
                cursor += (signed_square + ((signed_square >> 31) & 7)) >> 3
        if cursor > len(content) or cursor < 0xC00:
            result.update(status="outside_source_bytes", record_index=record_index, cursor=cursor)
            return result
    result.update(status="exact" if cursor == len(content) else "trailing_source_bytes", occupied=occupied,
                  consumed=cursor, remaining=len(content)-cursor)
    return result


def audit_level(directory):
    files = {path.name.casefold(): path for path in directory.iterdir() if path.is_file()}
    contours = files["terrain.ctr"].read_bytes()
    commands = files["terrain.cmd"].read_bytes()
    if len(contours) % 32:
        raise ValueError(f"{directory}: CTR length is not a multiple of native record stride")
    invalid_offsets = 0
    invalid_blocks = 0
    unique_offsets = set()
    for record_index in range(len(contours) // 32):
        offset = struct.unpack_from("<I", contours, record_index * 32 + 28)[0]
        unique_offsets.add(offset)
        if offset > len(commands) or len(commands) - offset < 8:
            invalid_offsets += 1
            continue
        triangle_count, triangle_bytes, base_count, extra_count = struct.unpack_from("<hhhh", commands, offset)
        vertex_start = offset + triangle_bytes + 8
        vertex_end = vertex_start + (base_count + extra_count) * 4
        # Bounds/consistency are corpus checks; the original trusted its input and did not add these guards.
        if min(triangle_count, triangle_bytes, base_count, extra_count) < 0 or \
                triangle_bytes != triangle_count * 4 or vertex_start < offset + 8 or vertex_end > len(commands):
            invalid_blocks += 1
    return {"ctr_records": len(contours)//32, "cmd_bytes": len(commands),
            "ctr_sha256": hashlib.sha256(contours).hexdigest(),
            "cmd_sha256": hashlib.sha256(commands).hexdigest(),
            "unique_command_offsets": len(unique_offsets), "invalid_offsets": invalid_offsets,
            "invalid_command_blocks": invalid_blocks,
            "bit": grid_table(files.get("terrain.bit"), False),
            "hmp": grid_table(files.get("terrain.hmp"), True),
            "scope": "source layout/corpus consistency only; no rendering or retail parity claim"}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--game", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    arguments = parser.parse_args()
    executable = arguments.game / "igi.exe"
    digest = hashlib.sha256(executable.read_bytes()).hexdigest()
    if digest != RETAIL_SHA256:
        parser.error("retail binary identity differs; revalidate native addresses")
    records = {}
    for level in range(1, 15):
        directory = arguments.game / "missions" / "location0" / f"level{level}" / "terrain"
        records[f"level{level}"] = audit_level(directory)
    report = {"retail_sha256": digest, "loader_va": "0x004C6240", "consumer_va": "0x00510010",
              "loader_evidence": "ev_84a279844258af41f55848d87719fc5845164a3660c0620c2dc309565b7707f3",
              "consumer_evidence": "ev_ba370d099419afc676d7082c9828a35e206e9a7de421482b56f8582b67ffb169",
              "levels": records}
    arguments.output.parent.mkdir(parents=True, exist_ok=True)
    arguments.output.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({name: {key: value[key] for key in ["ctr_records", "invalid_offsets", "invalid_command_blocks"]}
                      for name, value in records.items()}))


if __name__ == "__main__":
    main()
