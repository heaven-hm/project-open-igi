#!/usr/bin/env python3
"""Decode a registrar-shaped retail body into its ordered call/argument sequence.

Why: the startup sequence's remaining callees are almost all QTask class registrars of the form
TaskType_Register(...) + a run of handler links + optional script-name registration. Reading those
out of a linear listing by hand is where argument-order mistakes come from. This tool walks one body
from its entry and prints, for every call, the immediate arguments pushed since the previous call and
the cleanup that follows, with the retail address of every push and call so a transcription can cite
the instruction behind each line.

It models only what a registrar uses: `push $imm`, `push $addr`, `push reg/mem` (recorded as an
expression), `call`, and `add esp,N`. Anything else is printed as-is and marked, because a body with
real control flow is not a registrar and should be counted per block instead.

Validation is built in: `--self-test` decodes two registrars whose transcriptions already exist in
the tree and prints the decoded sequence next to the expected call order, so a regression in the
decoder is visible instead of silent.

Usage:
    python3 scripts/registrar_sequence.py 0x0046DD00
    python3 scripts/registrar_sequence.py 0x00467D90 --cap 4096
    python3 scripts/registrar_sequence.py --self-test
"""

from __future__ import annotations

import argparse
import importlib.util
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parent.parent
LISTING = ROOT / "build/retail-dis.txt"

PUSH_IMM = re.compile(r"^(?:push)\s+\$(0x[0-9a-f]+)$")
PUSH_EXPR = re.compile(r"^push\s+(.+)$")
ADD_ESP = re.compile(r"^add\s+\$?(0x[0-9a-f]+),%esp$")
CALL_TARGET = re.compile(r"^(?:call)\s+(?:0x([0-9a-f]+)|\*0x([0-9a-f]+))$")
CLEANUPS = {"ret", "retn", "retf"}

SELF_TEST = {
    # Expected call targets come from the transcriptions already in engine/src/retail/core/, so the
    # decoder is checked against committed work rather than against its own output.
    "0x0046DD00": [0x004D9FA0, 0x00401900, 0x00401400, 0x00401530, 0x00401530, 0x004F1A70,
                   0x00401400, 0x004C56F0, 0x00401400, 0x00416830, 0x00401400, 0x004B8810],
    "0x0046BA60": [0x004C48C0, 0x00401900, 0x00401400, 0x00401530, 0x00401530, 0x00416830,
                   0x00401400, 0x00416850, 0x00401400, 0x00416880, 0x00401400],
}


def load_coverage_module():
    spec = importlib.util.spec_from_file_location("nc", ROOT / "scripts/native_coverage.py")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def decode(rows, index_by_va, entry: int, cap: int) -> list[dict]:
    start = index_by_va.get(entry)
    if start is None:
        raise SystemExit(f"entry 0x{entry:08X} is not in the listing")
    calls: list[dict] = []
    pending: list[tuple[int, str, str]] = []   # (address, push text, role)
    cleanup = None
    noted = []
    code_flow = False
    index = start
    while index < len(rows) and rows[index][0] < entry + cap:
        address, mnemonic, rest = rows[index]
        text = f"{mnemonic} {rest}".strip()
        if mnemonic == "push":
            match = PUSH_IMM.match(text)
            if match:
                pending.append((address, text, match.group(1)))
            else:
                expr = PUSH_EXPR.match(text)
                pending.append((address, text, expr.group(1) if expr else "?"))
        elif mnemonic == "call":
            target = CALL_TARGET.match(text)
            target_va = None
            if target:
                target_va = int(target.group(1) or target.group(2), 16)
            calls.append({
                "address": address,
                "target": target_va,
                "indirect": target is None,
                "args": list(pending),
                "text": text,
            })
            pending.clear()
        elif mnemonic == "add":
            match = ADD_ESP.match(text)
            if match:
                cleanup = (address, int(match.group(1), 16))
                pending.clear()
        elif mnemonic in CLEANUPS:
            calls.append({"address": address, "target": None, "indirect": False,
                          "args": list(pending), "text": "ret", "return": True})
            break
        elif mnemonic.startswith("j"):
            code_flow = True
            noted.append(f"0x{address:08X}: {text}")
            if mnemonic == "jmp":
                break
        elif mnemonic in ("mov", "lea", "xor", "sub", "and", "or", "test", "cmp", "inc", "dec",
                          "movsx", "movzx", "shl", "shr", "sar", "imul", "idiv", "neg", "not", "nop", "xchg"):
            pass
        else:
            noted.append(f"0x{address:08X}: {text}")
        index += 1
    return {"calls": calls, "cleanup": cleanup, "noted": noted, "code_flow": code_flow}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("address", nargs="?", help="registrar entry VA, e.g. 0x0046DD00")
    parser.add_argument("--cap", type=int, default=2048, help="maximum body extent to walk (bytes)")
    parser.add_argument("--listing", default=str(LISTING))
    parser.add_argument("--self-test", action="store_true", help="decode the two known registrars and print the call sequence")
    args = parser.parse_args()

    coverage = load_coverage_module()
    rows = coverage.load_listing(pathlib.Path(args.listing))
    index_by_va = {va: i for i, (va, _, _) in enumerate(rows)}

    if args.self_test:
        failed = 0
        for va_text, expected in SELF_TEST.items():
            decoded = decode(rows, index_by_va, int(va_text, 16), 2048)
            got = [call["target"] for call in decoded["calls"] if not call.get("return") and not call["indirect"]]
            print(f"{va_text}: {len(got)} calls, code_flow={decoded['code_flow']}, "
                  f"cleanup={'0x%X' % decoded['cleanup'][1] if decoded['cleanup'] else 'none'}")
            for target in got:
                print(f"    0x{target:08X}" if target is not None else "    indirect")
            if got != expected:
                failed += 1
                print(f"    FAIL: expected {[hex(t) for t in expected]}")
        print("self-test:", "FAIL" if failed else "OK (decoded call targets match the committed transcriptions)")
        return 1 if failed else 0

    if not args.address:
        parser.error("give an address, or --self-test")
    entry = int(args.address, 0)
    decoded = decode(rows, index_by_va, entry, args.cap)
    print(f"body 0x{entry:08X}: {len(decoded['calls'])} calls, "
          f"code_flow={'yes' if decoded['code_flow'] else 'no'}, "
          f"cleanup={'add esp,0x%X at 0x%08X' % (decoded['cleanup'][1], decoded['cleanup'][0]) if decoded['cleanup'] else 'none'}")
    if decoded["code_flow"]:
        print("  NOTE: conditional jumps or a tail jmp -> not a straight-line registrar; count per block.")
    for call in decoded["calls"]:
        if call.get("return"):
            print(f"  0x{call['address']:08X}  ret")
            continue
        target = "indirect" if call["indirect"] else f"0x{call['target']:08X}"
        args_text = ", ".join(f"{arg[2]}@0x{arg[0]:08X}" for arg in call["args"])
        print(f"  0x{call['address']:08X}  call {target:<12} args: {args_text if args_text else '(none)'}")
    if decoded["noted"]:
        print("  other instructions:")
        for note in decoded["noted"]:
            print(f"    {note}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
