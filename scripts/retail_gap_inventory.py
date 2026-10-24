#!/usr/bin/env python3
"""Inventory engine sources requiring retail-fidelity review; never certify equivalence."""
import argparse
import hashlib
import json
from pathlib import Path
import re

MARKERS = re.compile(r'host helper|host adapter|heuristic|approximation|synthetic|fallback|not transcribed|not implemented|stub|provisional|placeholder', re.I)
EXTENSIONS = {'.c', '.cc', '.cpp', '.cxx', '.h', '.hpp', '.hxx'}

def inventory(root):
    root = root.resolve()
    manifest_path = root / 'docs/fidelity/native-verification.json'
    manifest_bytes = manifest_path.read_bytes()
    manifest = json.loads(manifest_bytes)
    by_path = {}
    for native in manifest.get('natives', []):
        path = native.get('implementation', {}).get('path')
        if path:
            by_path.setdefault(path, []).append({'address': native.get('address'),
                'status': native.get('verification', {}).get('status')})
    files = []
    for path in sorted((root / 'engine').rglob('*')):
        if not path.is_file() or path.suffix.lower() not in EXTENSIONS:
            continue
        relative = path.relative_to(root).as_posix()
        data = path.read_bytes()
        lines = data.decode('utf-8', errors='replace').splitlines()
        markers = [{'line': i, 'text': line.strip()} for i, line in enumerate(lines, 1) if MARKERS.search(line)]
        host_names = sorted(set(re.findall(r'\bHost_[A-Za-z0-9_]+', data.decode('utf-8', errors='replace'))))
        files.append({'path': relative, 'sha256': hashlib.sha256(data).hexdigest(),
                      'manifest_natives': by_path.get(relative, []), 'markers': markers, 'host_identifiers': host_names,
                      'classification': 'test' if '/tests/' in '/' + relative else 'source',
                      'review': 'required; address binding and marker matches are not proof of equivalence'})
    return {'schema_version': 1, 'manifest_sha256': hashlib.sha256(manifest_bytes).hexdigest(),
            'scope': 'Static inventory only; no call reachability or retail parity inferred',
            'files': files, 'file_count': len(files),
            'marker_count': sum(len(f['markers']) for f in files),
            'host_identifier_count': len({name for f in files for name in f['host_identifiers']}),
            'source_without_manifest_count': sum(f['classification'] == 'source' and not f['manifest_natives'] for f in files)}

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path('.'))
    parser.add_argument('--output', type=Path, default=Path('build/retail-gap-inventory.json'))
    args = parser.parse_args()
    result = inventory(args.root)
    output = (args.root / args.output).resolve()
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps({key: result[key] for key in ('file_count', 'marker_count', 'host_identifier_count', 'source_without_manifest_count')} | {'output': str(output)}))

if __name__ == '__main__':
    main()
