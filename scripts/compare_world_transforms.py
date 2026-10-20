#!/usr/bin/env python3
"""Compare the explicitly observed rigid-object subset; never attest a release."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
import struct


def compare(folder):
    retail = json.loads((folder / 'retail.json').read_text())
    candidate = json.loads((folder / 'candidate.json').read_text())
    metadata = json.loads((folder / 'comparison.json').read_text())
    scene = candidate['scene_log']
    if (scene['path'] != 'candidate-scene.log' or
            hashlib.sha256((folder / scene['path']).read_bytes()).hexdigest()
            != scene['sha256']):
        raise ValueError(f'{folder}: candidate scene-log identity differs')
    if hashlib.sha256((folder / 'probe.js').read_bytes()).hexdigest() != metadata['probe_sha256']:
        raise ValueError(f'{folder}: frozen probe identity differs')
    for name, capture in [('retail', retail), ('candidate', candidate)]:
        if capture.get('result') != 'observed' or capture.get('script_errors'):
            raise ValueError(f'{folder}: {name} capture is incomplete or errored')
        snapshot = capture['snapshots'][-1]
        if snapshot.get('failures') or snapshot.get('overflow'):
            raise ValueError(f'{folder}: {name} probe failed or overflowed')
    if retail['command'] != candidate['command']:
        raise ValueError(f'{folder}: launch arguments differ')
    expected = Counter(
        (item['modelInstanceName'], item['positionFloat64Bits'],
         item['orientationFloat32Bits'])
        for item in retail['snapshots'][-1]['objectPositions']
        if item.get('modelInstanceName'))
    observed = Counter()
    for match in re.finditer(
            r'type=EditRigidObj model=(\S+) position=([^ ]+) orientation=(\S+)',
            (folder / 'candidate-scene.log').read_text()):
        position = struct.pack('<3d', *map(float, match[2].split(','))).hex()
        orientation = struct.pack(
            '<9f', *map(float, match[3].rstrip(',').split(','))).hex()
        observed[(match[1], position, orientation)] += 1
    if not expected or not observed:
        raise ValueError(f'{folder}: rigid-object population was not observed')
    return dict(
        capture=str(folder), command=retail['command'],
        retail_records=expected.total(), candidate_records=observed.total(),
        retail_opaque_records=sum(not item.get('modelInstanceName') for item in
                                  retail['snapshots'][-1]['objectPositions']),
        exact_matches=(expected & observed).total(),
        retail_unmatched=(expected - observed).total(),
        candidate_unmatched=(observed - expected).total(),
        retail_sha256=retail['sha256'], candidate_sha256=candidate['sha256'],
        artifact_sha256={name: hashlib.sha256((folder / name).read_bytes()).hexdigest()
                         for name in ['retail.json', 'candidate.json',
                                      'candidate-scene.log', 'probe.js', 'comparison.json']},
        scope='Captured EditRigidObj model/position/orientation multisets only; '
              'other classes, scales, materials, textures, ordering and native ABI '
              'parity are not established.')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('captures', type=Path, nargs='+')
    args = parser.parse_args()
    try:
        reports = [compare(folder) for folder in args.captures]
    except (OSError, ValueError, KeyError, IndexError, struct.error) as error:
        parser.exit(2, f'inconclusive: {error}\n')
    print(json.dumps(reports, indent=2))
    return int(any(item['retail_unmatched'] or item['candidate_unmatched']
                   for item in reports))


if __name__ == '__main__':
    raise SystemExit(main())
