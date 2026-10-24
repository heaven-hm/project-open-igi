#!/usr/bin/env python3
"""Hash a candidate binary in a build-independent form.

The Win32 link stamps the PE header with the current time (TimeDateStamp at 0x88) and a derived
checksum (0xD8). Two builds from identical sources therefore differ in exactly those four bytes, which
means the plain sha256 of the candidate is a per-build identity, not a content identity. That is fine
for pinning a frozen pair, but it hides the fact that the code is reproducible.

This script reports both hashes and fails if the difference is anywhere other than those two fields,
so a genuine code difference can never be mistaken for a timestamp.
"""
from __future__ import annotations

import hashlib
import pathlib
import sys

TIME_DATE_STAMP = slice(0x88, 0x8C)
CHECKSUM = slice(0xD8, 0xDC)


def normalised_digest(data: bytes) -> str:
    scrubbed = bytearray(data)
    scrubbed[TIME_DATE_STAMP] = b"\x00\x00\x00\x00"
    scrubbed[CHECKSUM] = b"\x00\x00\x00\x00"
    return hashlib.sha256(bytes(scrubbed)).hexdigest()


def main(argv: list[str]) -> int:
    if len(argv) < 2:
        print("usage: candidate_normalised_hash.py <binary> [<other-binary>]", file=sys.stderr)
        return 2
    first = pathlib.Path(argv[1]).read_bytes()
    print("plain       ", hashlib.sha256(first).hexdigest())
    print("normalised  ", normalised_digest(first))
    if len(argv) >= 3:
        second = pathlib.Path(argv[2]).read_bytes()
        if len(first) != len(second):
            print("SIZE DIFFERS: code is not identical")
            return 1
        diffs = [i for i in range(len(first)) if first[i] != second[i]]
        allowed = set(range(TIME_DATE_STAMP.start, TIME_DATE_STAMP.stop)) | set(range(CHECKSUM.start, CHECKSUM.stop))
        unexpected = [i for i in diffs if i not in allowed]
        print(f"differing bytes: {len(diffs)}")
        if unexpected:
            print("CODE DIFFERS at", [hex(i) for i in unexpected[:16]])
            return 1
        print("only TimeDateStamp/checksum differ: code is identical, normalised hashes match"
              if normalised_digest(first) == normalised_digest(second) else "normalised hashes still differ")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
