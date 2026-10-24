#!/usr/bin/env python3
"""Mine the retail script-native registrar (name string, function pointer) pairs.

Retail IGI registers script natives by pushing a (name, function) pair into one of four registrar entry points.
Backwards-decoding each call site recovers the pair, which is the only place the retail gives a function its own
name. This mirrors the push-pair mining described by the project's reverse-engineering skills, run against the
image this repository treats as authority (/Users/haseeb-mir/Downloads/IGI/igi.exe).

Output: docs/fidelity/evidence/registrar-pairs.md plus build/registrar-pairs.json, cross-referenced with the
manifest so every pair shows whether the function is already implemented here.
"""
import json
import re
import struct
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
IMAGE = Path('/Users/haseeb-mir/Downloads/IGI/igi.exe')
REGISTRARS = (0x4B8890, 0x4BAAC0, 0x4B88C0, 0x4B88F0)


def read_pe_sections(image):
    data = image.read_bytes()
    e_lfanew = struct.unpack_from('<I', data, 0x3C)[0]
    section_count = struct.unpack_from('<H', data, e_lfanew + 6)[0]
    optional_size = struct.unpack_from('<H', data, e_lfanew + 20)[0]
    table = e_lfanew + 24 + optional_size
    sections = []
    for index in range(section_count):
        offset = table + index * 40
        name = data[offset:offset + 8].rstrip(b'\0').decode('ascii', 'replace')
        virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from('<IIII', data, offset + 8)
        sections.append((name, virtual_address + 0x400000, virtual_size, raw_pointer, raw_size))
    return data, sections


def read_cstring(data, sections, va):
    for _, start, size, pointer, raw_size in sections:
        if start <= va < start + max(size, raw_size):
            offset = pointer + (va - start)
            end = data.find(b'\0', offset, offset + 256)
            if end < 0:
                return None
            raw = data[offset:end]
            if not raw or any(byte < 0x20 or byte > 0x7E for byte in raw):
                return None
            return raw.decode('ascii')
    return None


def main():
    data, sections = read_pe_sections(IMAGE)
    instructions = []
    for line in (ROOT / 'build/retail-dis.txt').read_text().splitlines():
        match = re.match(r'\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)', line)
        if match:
            instructions.append((int(match.group(1), 16), match.group(2).strip()))

    manifest = json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())
    records = {int(record['address'], 16): record for record in manifest['natives']}

    pairs, unresolved = [], []
    for index, (address, text) in enumerate(instructions):
        match = re.search(r'call\s+0x([0-9a-f]+)', text)
        if not match or int(match.group(1), 16) not in REGISTRARS:
            continue
        pushes = []
        for previous in range(index - 1, max(-1, index - 12), -1):
            prior_text = instructions[previous][1]
            push_match = re.search(r'push\s+\$0x([0-9a-f]+)', prior_text)
            if push_match:
                pushes.append(int(push_match.group(1), 16))
            elif re.match(r'(mov|movl|xor|call|ret|jmp|j[a-z]+)\b', prior_text):
                break
        if len(pushes) < 2:
            unresolved.append({'site': address, 'registrar': int(match.group(1), 16), 'pushes': pushes})
            continue
        # cdecl pushes arguments right-to-left: the registrar's LAST argument (the name string) is pushed
        # first and the FIRST argument (the function pointer) last, so the push nearest the call is the name.
        name_va, function_va = pushes[0], pushes[1]
        name = read_cstring(data, sections, name_va)
        if name is None or not (0x401000 <= function_va <= 0x53250F):
            unresolved.append({'site': address, 'registrar': int(match.group(1), 16), 'pushes': pushes,
                               'name_address': name_va, 'function': function_va})
            continue
        record = records.get(function_va)
        pairs.append({
            'site': address,
            'registrar': int(match.group(1), 16),
            'function': function_va,
            'name_address': name_va,
            'name': name,
            'implemented': bool(record and record.get('implementation', {}).get('path')),
            'candidate_name': record['name'] if record else None,
            'implementation': record.get('implementation', {}).get('path') if record else None,
        })

    unique = {}
    for pair in pairs:
        unique.setdefault(pair['function'], pair)
    rows = sorted(unique.values(), key=lambda item: item['function'])
    implemented = [row for row in rows if row['implemented']]
    missing = [row for row in rows if not row['implemented']]

    output = ROOT / 'docs/fidelity/evidence/registrar-pairs.md'
    lines = [
        '# Retail script-native registrar pairs (name string, function pointer)',
        '',
        f'- image: `{IMAGE}`, base `0x00400000`',
        f'- registrars scanned: {", ".join(f"`0x{value:06X}`" for value in REGISTRARS)}',
        '- method: `scripts/registrar_pairs.py` - for every `call` into a registrar, decode the preceding pushes backwards,',
        '  read the name string from the image, and cross-reference the function against `docs/fidelity/native-verification.json`.',
        '',
        f'- call sites decoded: **{len(pairs)}**, unique functions: **{len(rows)}**',
        f'- already implemented here: **{len(implemented)}**, not implemented: **{len(missing)}**',
        f'- call sites whose pushes did not decode: **{len(unresolved)}**',
        '',
        '## Unique functions, in address order',
        '',
        '| retail VA | retail registered name | implemented | candidate name here |',
        '| --- | --- | --- | --- |',
    ]
    for row in rows:
        mark = 'yes' if row['implemented'] else '**no**'
        lines.append(f"| `0x{row['function']:06X}` | `{row['name'] or '?'}` | {mark} | {row['candidate_name'] or '-'} |")
    if unresolved:
        lines += ['', '## Undecoded call sites', '']
        for item in unresolved:
            lines.append(f"- `0x{item['site']:06X}` -> registrar `0x{item['registrar']:06X}`, pushes decoded: {len(item['pushes'])}")
    output.write_text('\n'.join(lines) + '\n')
    (ROOT / 'build/registrar-pairs.json').write_text(json.dumps({'pairs': rows, 'unresolved': unresolved}, indent=1))
    print(f'call sites={len(pairs)} unique={len(rows)} implemented={len(implemented)} missing={len(missing)} unresolved={len(unresolved)}')
    for row in missing[:15]:
        print(f"  MISSING 0x{row['function']:06X} {row['name']}")
    print(f'-> {output}')


if __name__ == '__main__':
    main()
