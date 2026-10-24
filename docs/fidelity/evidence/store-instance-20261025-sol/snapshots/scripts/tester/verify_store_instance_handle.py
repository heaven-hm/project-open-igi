#!/usr/bin/env python3
"""Run the ordered second game-native verification without gaps between phases.

Does not modify engine sources, deploy a game, or promote the active manifest.
A proposed scoped record is checked separately; acceptance remains reviewable.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from candidate_build_receipt import validate_receipt


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    out = args.out.resolve()
    out.mkdir(parents=True, exist_ok=False)
    source = ROOT / 'engine/src/retail/core/app_startup_natives.cpp'
    candidate = ROOT / 'build-first-native-sol/igi1.exe'
    receipt = ROOT / 'build-first-native-sol/diagnostic-receipt.json'
    packet = ROOT / 'docs/fidelity/evidence/store-instance-20261025-sol'
    expected = '0cd758f2ad54db1e376f379e730f907a4c5dfb986d519302642cb948d41fdc08'
    digest = lambda path: hashlib.sha256(Path(path).read_bytes()).hexdigest()
    if digest(source) != expected:
        raise ValueError('source changed: a fresh semantic review is required')
    env = dict(os.environ, PYTHONDONTWRITEBYTECODE='1',
        IGI_CANDIDATE_EXE=str(candidate), IGI_CANDIDATE_BUILD_RECEIPT=str(receipt))
    phases = []

    def run(name, command):
        with (out / (name + '.log')).open('w') as log:
            result = subprocess.run(command, cwd=ROOT, env=env, stdout=log,
                                    stderr=subprocess.STDOUT, timeout=240)
        phases.append(dict(name=name, command=command, exit_code=result.returncode))
        if result.returncode:
            raise ValueError(name + ' failed; no acceptance')

    def save(name, value):
        path = out / name
        path.write_text(json.dumps(value, indent=2) + '\n')
        return {'evidence_path': str(path.relative_to(ROOT)), 'evidence_sha256': digest(path)}

    # Gate evidence must live under docs/fidelity/evidence, not this run folder.
    def evidence(name, value):
        path = packet / name
        path.write_text(json.dumps(value, indent=2) + '\n')
        return {'evidence_path': str(path.relative_to(ROOT)), 'evidence_sha256': digest(path)}

    def reference(path):
        return {'evidence_path': str(path.relative_to(ROOT)), 'evidence_sha256': digest(path)}

    report = dict(kind='igi-single-native-verification-run', schema_version=1,
                  address='0x0048F0D0', result='blocked', verified=False)
    try:
        run('natural', [sys.executable, 'scripts/tester/capture_store_instance_handle_natural.py',
                        '--out', str(out / 'natural')])
        run('controlled', [sys.executable, 'scripts/frida_app_startup_store_5c8bc0_pair.py',
                          '--candidate', str(candidate), '--receipt', str(receipt),
                          '--out', str(out / 'controlled.json')])
        build = validate_receipt(receipt, ROOT, candidate, [source])
        pair = json.loads((out / 'controlled.json').read_text())
        natural = json.loads((out / 'natural/comparison.json').read_text())
        if (pair['result'] != 'passed' or not natural['observations_match']
                or pair['candidate_sha256'] != digest(candidate)
                or natural['identities'][str(candidate)] != digest(candidate)):
            raise ValueError('paired observations did not match this image')
        identity = dict(schema_version=1, address='0x0048F0D0',
            binary_sha256=pair['binary_sha256'], implementation_sha256=digest(source))
        shutil.copyfile(receipt, packet / 'build-final-receipt.json')
        build_ref = evidence('build-final.json', dict(identity, kind='build-report', result='passed',
            executed=True, command=' '.join(build['command']),
            candidate_executable_sha256=digest(candidate),
            receipt=reference(packet / 'build-final-receipt.json'),
            scope='Executed diagnostic build; no release attestation or deployment'))
        for side in ('retail', 'candidate', 'comparison'):
            shutil.copyfile(out / 'natural' / (side + '.json'), packet / ('natural-accepted-' + side + '.json'))
        shutil.copyfile(out / 'controlled.json', packet / 'controlled-final.json')
        raw = {side: json.loads((out / 'natural' / (side + '.json')).read_text())
               for side in ('retail', 'candidate')}
        frida_ref = evidence('frida-final.json', dict(identity, kind='frida-runtime-report',
            result='passed', candidate_executable_sha256=digest(candidate),
            retail_observed_calls=len(raw['retail']['entries']),
            candidate_observed_calls=len(raw['candidate']['entries']),
            probe_sha256=digest(ROOT / 'scripts/tester/frida_store_instance_handle_natural.js'),
            comparison=natural, scope='One natural WinMain call per side, window level1'))
        manifest = json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())
        native = next(n for n in manifest['natives'] if int(n['address'], 16) == 0x48f0d0)
        # Static/source review is prepared independently of the old manifest's
        # stale source hashes and historical window-observation contract.
        native.update(json.loads((packet / 'candidate-record.json').read_text()))
        verification = native['verification']
        verification.update(status='VERIFIED_WITH_DOCUMENTED_SCOPE', build=build_ref,
            frida_runtime=frida_ref, runtime_tests=reference(packet / 'controlled-final.json'),
            natural_runtime_observation=reference(packet / 'natural-accepted-comparison.json'),
            scope={'path':'docs/fidelity/scopes/store-instance-handle-20261025.md',
                   'sha256':digest(ROOT / 'docs/fidelity/scopes/store-instance-handle-20261025.md')})
        verification['limitations'] = [
            'Proposed scoped record only; this run does not promote the active manifest.',
            'One natural WinMain call and eleven controlled pre-resume ABI calls per side; no full startup/game parity.',
            'Independent human/agent critic not performed; author semantic review and independent static tools retained.',
            'Original private identifiers are not recovered. CRT/compiler startup and Windows SDK code are outside game-native scope.']
        verification['behavioral_runner']['args'] += ['--out', str(out / 'gate-live-pair.json')]
        save('proposed-manifest.json', manifest)
        gate = out / 'gate.json'
        run('gate', [sys.executable, 'scripts/native_gate.py', '--root', str(ROOT),
            '--manifest', str(out / 'proposed-manifest.json'), '--file', str(source),
            '--address', '0x0048F0D0', '--threshold', 'VERIFIED', '--cfg-threshold', '0.85',
            '--require-semantic-evidence', '--require-behavioral-tests', '--require-frida-probes',
            '--report', str(gate)])
        differential = out / 'differential.json'
        run('typed-comparison', [sys.executable, 'scripts/tester/check_store_instance_handle_pair.py',
            '--pair', str(out / 'controlled.json'), '--natural', str(out / 'natural'),
            '--gate-report', str(gate), '--candidate', str(candidate),
            '--receipt', str(receipt), '--out', str(differential)])
        run('runtime-checker', [sys.executable, 'scripts/check_frida_differential.py',
            '--gate-report', str(gate), '--candidate-exe', str(candidate), '--report', str(differential)])
        validate_receipt(receipt, ROOT, candidate, [source])
        report.update(result='passed', verified=True,
            scope='Declared StoreInstanceHandle native scope only; active manifest promotion remains separate',
            candidate_sha256=digest(candidate), implementation_sha256=digest(source),
            gate_sha256=digest(gate), differential_sha256=digest(differential))
    except Exception as error:
        report['error'] = repr(error)
    finally:
        report['phases'] = phases
        save('result.json', report)
        print(json.dumps(report, indent=2))
    return 0 if report['result'] == 'passed' else 1


if __name__ == '__main__':
    raise SystemExit(main())
