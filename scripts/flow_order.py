#!/usr/bin/env python3
"""Chronological native order: retail call flow from the entry, in call order.

Walks the retail call graph depth-first from a start VA, visiting each callee in the
order its call site appears in the caller's instructions. Compiler/CRT ranges are
recorded as excluded rather than expanded. Implementation status comes from
docs/fidelity/native-verification.json, the single owner of implemented addresses.
"""
import argparse
import bisect
import hashlib
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
# 0x00401000..0x00402000 contains game QTask functions, not compiler startup.
# In particular 0x401000 creates the task context and 0x401050 dispatches it.
CRT_RANGES = ((0x4A5000, 0x4AB000), (0x4AF000, 0x4B0000), (0x4CEF00, 0x4CF000))


def is_crt(va):
    return any(lo <= va < hi for lo, hi in CRT_RANGES)


def load_instructions(path):
    ins = {}
    for line in path.read_text().splitlines():
        m = re.match(r'\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)', line)
        if m:
            ins[int(m.group(1), 16)] = m.group(2).strip()
    return ins


def load_spans(path):
    fns = {}
    for f in json.loads(path.read_text()):
        fns[f['addr']] = (f['addr'], f['addr'] + f.get('size', 0))
    return fns


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--start', type=lambda v: int(v, 0), default=0x0048F530)
    ap.add_argument('--limit', type=int, default=20000)
    ap.add_argument('--depth-limit', type=int, default=64)
    ap.add_argument('--out', type=Path, default=ROOT / 'docs/fidelity/evidence/chronological-native-order.md')
    ap.add_argument('--binary', type=Path, default=Path('/Users/haseeb-mir/Downloads/IGI/igi.exe'))
    ap.add_argument('--capture', action='store_true', help='Capture private image-bound objdump/r2 inputs')
    args = ap.parse_args()

    digest = lambda p: hashlib.sha256(p.read_bytes()).hexdigest()
    image_hash = digest(args.binary)
    folder = ROOT / 'build/native-flow' / image_hash
    disassembly, functions, identity = folder / 'disassembly.txt', folder / 'functions.json', folder / 'identity.json'
    if args.capture:
        folder.mkdir(parents=True, exist_ok=True)
        for path, command in [(disassembly, ['i686-w64-mingw32-objdump', '-d', '-M', 'intel', str(args.binary)]),
                              (functions, ['r2', '-2', '-q', '-c', 'aaa; aflj; q', str(args.binary)])]:
            with path.open('w') as output:
                subprocess.run(command, stdout=output, check=True, timeout=300)
        if digest(args.binary) != image_hash:
            raise ValueError('retail changed during flow capture')
        identity.write_text(json.dumps({'binary_sha256':image_hash,
            'disassembly_sha256':digest(disassembly), 'functions_sha256':digest(functions)},indent=2)+'\n')
    if not identity.is_file():
        ap.error('missing private flow inputs; run with --capture (shared /tmp inputs are not accepted)')
    proof = json.loads(identity.read_text())
    if proof != {'binary_sha256':image_hash,'disassembly_sha256':digest(disassembly),'functions_sha256':digest(functions)}:
        ap.error('flow input hash mismatch; recapture')
    ins, spans = load_instructions(disassembly), load_spans(functions)
    instruction_addresses = sorted(ins)
    callee_cache = {}
    manifest = json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())
    impl = {int(r['address'], 16): r['name'] for r in manifest['natives'] if r.get('implementation', {}).get('path')}
    order, seen, unresolved = [], set(), set()

    def calls_of(va):
        if va in callee_cache:
            return callee_cache[va]
        span = spans.get(va)
        if not span:
            unresolved.add(va)
            return []
        first = bisect.bisect_left(instruction_addresses, span[0])
        last = bisect.bisect_left(instruction_addresses, span[1])
        addrs = instruction_addresses[first:last]
        out = []
        for a in addrs:
            m = re.search(r'call\s+(?:0x)?([0-9a-f]+)(?:\s|$)', ins[a])
            if m:
                out.append(int(m.group(1), 16))
        callee_cache[va] = out
        return out

    def walk(va, depth):
        # The entry point and the runtime bootstrap are compiler territory, but their
        # game-code callees are not: expand CRT functions without listing them.
        if len(order) >= args.limit or depth > args.depth_limit or va in seen:
            return
        seen.add(va)
        if not is_crt(va):
            order.append((va, depth))
        for target in calls_of(va):
            walk(target, depth + 1)

    walk(args.start, 0)
    missing = [(va, d) for va, d in order if va not in impl]
    rows = []
    for index, (va, depth) in enumerate(order, 1):
        span = spans.get(va)
        size = (span[1] - span[0]) if span else 0
        count = len(calls_of(va))
        name = impl.get(va, '**MISSING**')
        rows.append(f'| {index} | `0x{va:06X}` | {name} | {size:#x} | {count} | {depth} |')
    text = [
        '# Chronological native order from the retail entry point',
        '',
        'Generated by `scripts/flow_order.py` (depth-first, callee order = order of the call sites in the caller).',
        'Legacy compiler/CRT exclusions: 0x004A5000-0x004AB000, 0x004AF000-0x004B0000, 0x004CEF00-0x004CF000. These ranges remain provisional classifications; QTask 0x00401000-0x00402000 is game code and is included.',
        'This is a static direct-call walk, not an observed runtime sequence or complete coverage: indirect calls, branch selection and tail transfers require separate evidence.',
        f'Inputs are bound to retail SHA-256 `{image_hash}`; `{identity.relative_to(ROOT)}` records both capture hashes.',
        '',
        f'- start: `0x{args.start:06X}` (WinMain by default; compiler startup is outside the game-native work order)',
        f'- functions visited: **{len(order)}**',
        f'- missing (no implementation in the manifest): **{len(missing)}**',
        f'- implemented: **{len(order) - len(missing)}**',
        f'- unresolved function extents: **{len(unresolved)}** (these paths are not fully expanded)',
        '- Manifest source-path presence is inventory only, not linkage or native acceptance.',
        '',
        '| order | retail VA | implemented as | bytes | callees | depth |',
        '| --- | --- | --- | --- | --- | --- |',
        *rows,
        '',
        '## First missing natives in flow order',
        '',
    ]
    for index, (va, depth) in enumerate([m for m in missing][:80], 1):
        text.append(f'{index}. `0x{va:06X}` (depth {depth}, {len(calls_of(va))} callees)')
    args.out.write_text('\n'.join(text) + '\n')
    print(f'visited={len(order)} missing={len(missing)} implemented={len(order)-len(missing)} -> {args.out}')


if __name__ == '__main__':
    main()
