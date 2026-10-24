#!/usr/bin/env python3
"""Normalize retained BN/r2 graphs without assigning native acceptance.

Copies the complete existing analysis capture beside each normalized artifact;
requires successful tools and preserves the decompiler's inferred signature.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('capture', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    raw = args.capture.read_bytes()
    document = json.loads(raw)
    if document['result'] != 'analysis_collected' or any(
            a['exit_code'] != 0 for a in document['artifacts'].values()):
        raise RuntimeError('incomplete analysis capture')
    args.output.mkdir(parents=True, exist_ok=False)
    (args.output / 'capture.json').write_bytes(raw)
    artifacts = document['artifacts']
    normalize = lambda value: f'0x{int(value, 16) if isinstance(value, str) else value:08X}'
    nodes, edges = [], []
    table = artifacts['bn_function_basic_blocks']['stdout']
    if 'truncated: true' in table:
        raise RuntimeError('truncated Binary Ninja graph')
    for line in table.splitlines():
        parts = [part.strip() for part in line.split('|')]
        if len(parts) < 7 or not parts[1].isdigit():
            continue
        source = normalize(parts[2].split('-')[0])
        nodes.append({'address': source})
        for kind, target in re.findall(r'(TrueBranch|FalseBranch|UnconditionalBranch)\s*#\d+\s*(0x[0-9a-fA-F]+)-', parts[5]):
            edges.append({'source': source, 'target': normalize(target),
                          'kind': 'unconditional' if kind == 'UnconditionalBranch' else 'conditional'})
    graph = json.loads(artifacts['radare2_cfg']['stdout'])
    if len(graph) != 1 or graph[0]['addr'] != int(document['address'], 16):
        raise RuntimeError('wrong Radare2 entry')
    rnodes, redges = [], []
    for block in graph[0]['blocks']:
        source = normalize(block['addr'])
        rnodes.append({'address': source})
        for key in ('jump', 'fail'):
            if block.get(key) not in (None, -1, 0xffffffffffffffff):
                redges.append({'source': source, 'target': normalize(block[key]),
                               'kind': 'conditional' if 'fail' in block else 'unconditional'})
    for tool, cfg in [('binary_ninja', {'nodes': nodes, 'edges': edges}),
                      ('radare2', {'nodes': rnodes, 'edges': redges})]:
        output = {'kind': 'native-disassembly', 'schema_version': 1, 'tool': tool,
                  'binary_sha256': document['binary_sha256'], 'function_entry': document['address'],
                  'signature': 'See retained decompiler output; source types/names remain inferred',
                  'cfg': cfg, 'raw_capture': 'capture.json',
                  'raw_capture_sha256': hashlib.sha256(raw).hexdigest(), 'verified': False}
        (args.output / (tool + '.json')).write_text(json.dumps(output, indent=2) + '\n')
    (args.output / 'angr.json').write_text(artifacts['angr']['stdout'])
    print(json.dumps(document['structural_summary']))

if __name__ == '__main__':
    main()
