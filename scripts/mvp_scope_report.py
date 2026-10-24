#!/usr/bin/env python3
"""Inventory MVP engine native identities and reference candidates; never attest parity.

Source membership does not prove runtime reachability. --run-gate checks every
address-bound engine entry with the strict per-native gate, including entries
that have not yet been registered. build.sh retains its --changed default.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

import native_gate as gate

ROOT = Path(__file__).resolve().parent.parent
# These are subsystem scopes, not a hand-maintained native/address allowlist.
ENGINE_EXCLUDED = {'audio', 'diagnostics'}
REFERENCE_FAMILIES = (
    'config', 'human', 'locomotion', 'body', 'bone', 'collision', 'physics',
    'rigid', 'forceaccumulator', 'terrain', 'heightmap', 'texture', 'material',
    'mipmap', 'dynobj', 'model', 'mesh', 'ilff', 'iff', 'resource', 'device',
    'render', 'display', 'transcontext', 'origo', 'vec3', 'mat33', 'qvm',
    'qtask', 'qobj', 'script', 'symbol', 'qfile', 'input', 'mouse', 'keyboard',
    'joypad', 'mem', 'worldobject', 'magicobj', 'ladder', 'door', 'switch',
)


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def inventory(root):
    manifest_path = root / 'docs/fidelity/native-verification.json'
    manifest = json.loads(manifest_path.read_text())
    records = gate.index_manifest(manifest)
    engine = {}
    unbound = []
    for path in sorted((root / 'engine/src/retail').rglob('*.cpp')):
        relative = path.relative_to(root).as_posix()
        if path.relative_to(root / 'engine/src/retail').parts[0] in ENGINE_EXCLUDED:
            continue
        identities = gate.natives_with_manifest(path, relative, records)
        if not identities:
            unbound.append(relative)
        for native in identities:
            va = native['address']
            item = engine.setdefault(va, {'address': va, 'name': native['name'], 'source_paths': []})
            item['source_paths'].append(relative)
    rows = []
    for va, source in sorted(engine.items()):
        record = records.get(va)
        verification = (record or {}).get('verification', {})
        implementation = (record or {}).get('implementation', {})
        path = implementation.get('path')
        exists = bool(path and (root / path).is_file())
        rows.append({**source, 'name': (record or {}).get('name', source['name']),
                     'has_record': record is not None,
                     'implementation': path,
                     'implementation_hash_matches': exists and digest(root / path) == implementation.get('sha256'),
                     'status': verification.get('status'),
                     'runner': (verification.get('behavioral_runner') or {}).get('path'),
                     'gameNatives': (record or {}).get('game_natives', {}).get('path'),
                     'runtime_reachability': 'not established by this inventory'})
    references = {}
    for path in sorted((root / 'scripts/GameNatives').rglob('*.cpp')):
        relative = path.relative_to(root).as_posix()
        match = gate.GAME_NATIVE_ADDRESS.match(path.name)
        if not match:
            continue
        family = path.parent.name.lower()
        name = path.stem.split('-', 1)[-1]
        if not (family.startswith(REFERENCE_FAMILIES) or name.lower().startswith(REFERENCE_FAMILIES)):
            continue
        va = gate.normalize_address(match.group('address'))
        if va in engine:
            continue
        item = references.setdefault(va, {'address': va, 'candidate_name': name, 'paths': [],
                                         'has_record': va in records,
                                         'engine_implementation_identified': False,
                                         'runtime_requirement': 'unresolved; candidate inventory only'})
        item['paths'].append(relative)
    return {'schema_version': 2, 'scope': 'MVP source coverage; not runtime reachability or acceptance',
            'manifest_sha256': digest(manifest_path), 'count': len(rows), 'natives': rows,
            'missing_record_count': sum(not row['has_record'] for row in rows),
            'stale_implementation_count': sum(row['has_record'] and not row['implementation_hash_matches'] for row in rows),
            'reference_only_count': len(references), 'reference_only_candidates': list(references.values()),
            'unbound_source_files': unbound}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--output', type=Path)
    parser.add_argument('--run-gate', action='store_true')
    args = parser.parse_args()
    root = args.root.resolve()
    report = inventory(root)
    output = args.output or root / 'docs/fidelity/evidence/mvp-scope-natives.json'
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({key: report[key] for key in ('count', 'missing_record_count',
                     'stale_implementation_count', 'reference_only_count')}, indent=2))
    print(f'Inventory: {output}')
    if args.run_gate:
        if not report['natives']:
            parser.error('no engine native identities discovered; refusing an empty pass')
        command = [sys.executable, str(root / 'scripts/native_gate.py'), '--root', str(root),
                   '--threshold', 'VERIFIED', '--cfg-threshold', '0.85',
                   '--require-semantic-evidence', '--require-behavioral-tests',
                   '--report', str(root / 'build/mvp-native-verification.json')]
        for row in report['natives']:
            command.extend(['--address', row['address']])
        return subprocess.run(command, cwd=root, check=False).returncode
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
