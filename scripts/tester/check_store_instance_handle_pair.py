#!/usr/bin/env python3
"""Check recorded StoreInstanceHandle calls and bind them to a focused gate.

Controlled ABI replay and a natural WinMain call are separate required scopes.
This does not accept the enclosing startup sequence or deploy an executable.
"""
import argparse
import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from candidate_build_receipt import validate_receipt
from verification_scope import file_digest, load_scope, pair_typed_sequences, validate_runtime_contract
from check_frida_differential import behavior_matches


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    for name in ('pair', 'natural', 'gate-report', 'candidate', 'receipt', 'out'):
        ap.add_argument('--' + name, type=Path, required=True)
    args = ap.parse_args()
    required, gate_digest, gate = load_scope(args.gate_report)
    if required != {'0x0048F0D0'}:
        raise ValueError('checker supports exactly one native: 0x0048F0D0')
    source = ROOT / 'engine/src/retail/core/app_startup_natives.cpp'
    validate_receipt(args.receipt, ROOT, args.candidate, [source])
    contract = gate['checked_natives'][0]['runtime_contract']
    pair = json.loads(args.pair.read_text())
    probe = ROOT / 'scripts/frida_app_startup_store_5c8bc0_probe.js'
    if (pair.get('result') != 'passed' or pair.get('executed') is not True
            or pair['implementation_sha256'] != file_digest(source)
            or pair['binary_sha256'] != gate['binary_sha256']
            or pair['candidate_sha256'] != file_digest(args.candidate)
            or pair['probe_sha256'] != file_digest(probe)):
        raise ValueError('controlled capture identity/result is invalid')
    runner = ROOT / pair['runner']['path']
    if file_digest(runner) != pair['runner']['sha256']:
        raise ValueError('controlled runner changed after capture')
    calls = {}
    for side, digest in [('retail', pair['binary_sha256']), ('candidate', pair['candidate_sha256'])]:
        record = pair[side]
        if (record['executable_sha256'] != digest or record['owned_process_terminated'] is not True
                or record['probe']['globalStateRestored'] is not True):
            raise ValueError('controlled capture image/cleanup/state restore failed')
        raw = record['probe']['calls']
        if len(raw) != 11 or len(raw) != len(pair['cases']):
            raise ValueError('incomplete controlled sequence')
        calls[side] = []
        for index, call in enumerate(raw):
            fixture = pair['cases'][index]['input']
            if call['input'] != fixture:
                raise ValueError('controlled sequence input/order changed')
            word = fixture['value'] & 0xffffffff
            signed = word if word < 0x80000000 else word - 0x100000000
            expected = dict(return_value=signed,
                state_delta={'global_0x005C8BC0_before': fixture['initial_state'],
                             'global_0x005C8BC0_after': signed},
                side_effects={'global_0x005C8BC0_after': signed})
            if call['output'] != expected:
                raise ValueError('store/return/state differs from the reviewed instructions')
            calls[side].append({'typedState': {'scope': contract['scope'],
                'inputs': call['input'],
                'outputs': {key: call['output'][key] for key in ('return_value', 'state_delta')},
                'sideEffects': call['output']['side_effects']}})
    # The separate natural capture proves the actual WinMain path invokes it.
    from capture_store_instance_handle_natural import compare
    natural = {side: json.loads((args.natural / (side + '.json')).read_text())
               for side in ('retail', 'candidate')}
    natural_comparison = compare(natural)
    if not natural_comparison['observations_match']:
        raise ValueError('natural WinMain invocation differs or was not completely captured')
    for side, digest in [('retail', pair['binary_sha256']), ('candidate', pair['candidate_sha256'])]:
        if natural[side]['executable']['sha256'] != digest:
            raise ValueError('natural capture tested a different image')
        targets = natural[side].get('targets', [])
        # probe_template_sha256 identifies the driver's generic template even
        # when a custom probe is injected. targets records the injected file.
        if (len(targets) != 1 or targets[0].get('probe_sha256') !=
                file_digest(ROOT / 'scripts/tester/frida_store_instance_handle_natural.js')):
            raise ValueError('natural probe changed after capture')
    row = dict(address='0x0048F0D0', name='StoreInstanceHandle', retailCalled=True,
        candidateCalled=True, candidateReachable=True, scalarReturn=False,
        semanticComparator=pair['probe_sha256'], retailCalls=calls['retail'],
        candidateCalls=calls['candidate'],
        comparison=pair_typed_sequences(calls['retail'], calls['candidate']))
    row['argumentsMatch'] = row['comparison']['inputs']['retail'] == row['comparison']['inputs']['candidate']
    row['semanticMatch'] = behavior_matches(row)
    validate_runtime_contract(row, contract, pair['probe_sha256'])
    if not row['semanticMatch']:
        raise ValueError('typed replay comparison failed')
    validate_receipt(args.receipt, ROOT, args.candidate, [source])
    report = dict(kind='igi-paired-native-runtime', schema_version=1, result='passed',
        binarySha256=pair['binary_sha256'], candidateSha256=pair['candidate_sha256'],
        staticGateSha256=gate_digest, probeSha256=pair['probe_sha256'], results=[row],
        naturalComparison=natural_comparison,
        scope='Eleven controlled ABI calls per game plus one natural WinMain invocation per game',
        limitations=['Controlled calls are explicit Frida NativeFunction replay, before main resumes.',
                     'The natural startup capture is separate and uses its own probe.',
                     'No full WinMain, level-load, rendering, or playable-game acceptance.'],
        artifacts={str(p):file_digest(p) for p in [args.pair, args.gate_report, args.receipt,
            source, probe, *(args.natural / (side + '.json') for side in ('retail','candidate'))]})
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(report, indent=2) + '\n')
    print('StoreInstanceHandle: complete controlled and natural observations matched; scope limited to this native')


if __name__ == '__main__':
    main()
