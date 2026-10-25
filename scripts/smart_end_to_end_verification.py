#!/usr/bin/env python3
"""Strict per-native verification of the current Windows x86 candidate.

Build binding -> VERIFIED native gate (CFG >= .85, angr, semantics, live
behavior) -> paired natural-call capture -> typed runtime-contract checker.
No phase promotes manifest statuses. Partial checks never print True.
"""
from __future__ import annotations
import argparse
from contextlib import ExitStack
from native_test_runtime import runtime_endpoint
from datetime import datetime, timezone
import hashlib
import json
import os
import re
from native_test_config import load_config
from pathlib import Path
import subprocess
import sys
import uuid
from candidate_build_receipt import validate_receipt

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def read_config():
    return load_config(ROOT)


def invoke(command, env, timeout=1200):
    result = subprocess.run(command, cwd=ROOT, env=env, text=True, capture_output=True, timeout=timeout)
    return dict(command=command, exit_code=result.returncode, stdout=result.stdout, stderr=result.stderr)


def build_evidence_manifest(root,address,record,binary_hash,source_hash,candidate,receipt,folder):
    """Per-run catalog overlay containing an actual validated build, no statuses changed."""
    data=validate_receipt(receipt,root,candidate,[root/record['implementation']['path']])
    saved=folder/'candidate-receipt.json';saved.write_bytes(receipt.read_bytes())
    target=root/'docs/fidelity/evidence'/('executed-build-'+address+'-'+digest(candidate)[:16]+'-'+folder.name+'.json')
    target.parent.mkdir(parents=True,exist_ok=True)
    value=dict(schema_version=1,kind='build-report',address=address,result='passed',executed=True,
        binary_sha256=binary_hash,implementation_sha256=source_hash,
        candidate_executable_sha256=digest(candidate),command=' '.join(data['command']),
        receipt=dict(path=str(saved.relative_to(root)),sha256=digest(saved)),
        scope='Actual source-bound compiler execution only; not native acceptance')
    target.write_text(json.dumps(value,indent=2)+'\n')
    document=json.loads((root/'docs/fidelity/native-verification.json').read_text())
    rows=[row for row in document['natives'] if row['address']==address]
    if len(rows)!=1 or rows[0]!=record:raise ValueError('native record changed before build evidence overlay')
    rows[0]['verification']['build']=dict(evidence_path=str(target.relative_to(root)),evidence_sha256=digest(target))
    overlay=folder/'build-evidence-manifest.json'
    overlay.write_text(json.dumps(document,indent=2)+'\n')
    return overlay


def run(address: str, args) -> tuple[dict, int]:
    config = read_config()
    run_id = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ-') + uuid.uuid4().hex[:8]
    folder = ROOT / 'build/native-one' / address / run_id
    folder.mkdir(parents=True)
    output = args.report or folder / 'report.json'
    static_report, runtime_report = folder / 'static-gate.json', folder / 'frida-pair.json'
    report = dict(schema_version=1, kind='igi-one-native-verification', address=address,
                  run_id=run_id, result='blocked', verified=False, phases={})
    env = dict(os.environ, LC_ALL='C', LANG='C')
    env.setdefault('IGI_ANGR_PYTHON', config['angr_python'])
    env.setdefault('FRIDA_HOST', config['frida_host'])
    runtime = ExitStack()
    try:
        binary = (args.binary or Path(config['retail_exe'])).resolve()
        candidate = (args.candidate or ROOT / config['candidate']).resolve()
        build = ROOT / config['build_directory']
        receipt = args.receipt or build / 'candidate-receipt.json'
        records = json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())['natives']
        matches = [r for r in records if int(r['address'], 16) == int(address, 16)]
        if len(matches) != 1:
            raise ValueError('exactly one address-bound manifest record is required')
        record = matches[0]
        implementation = (ROOT / record['implementation']['path']).resolve()
        if not implementation.is_relative_to(ROOT / 'engine') or not implementation.is_file():
            raise ValueError('native is not implemented in this clean branch; historical GameNatives cannot approve it')
        if digest(binary) != config['retail_sha256'] or digest(binary) != record.get('binary_sha256'):
            raise ValueError('retail image does not match the selected binary identity')
        if digest(implementation) != record['implementation'].get('sha256'):
            raise ValueError('current source differs from its reviewed manifest record')
        report.update(binary_sha256=digest(binary), implementation_sha256=digest(implementation))
        if not args.static_only:
            if not args.skip_build:
                configure = invoke(['cmake', '-S', str(ROOT), '-B', str(build), '-DCMAKE_SYSTEM_NAME=Windows',
                    '-DCMAKE_CXX_COMPILER=' + os.environ.get('IGI_WIN32_CXX', 'i686-w64-mingw32-g++'), '-DCMAKE_BUILD_TYPE=Release'], env, 120)
                report['phases']['configure'] = configure
                if configure['exit_code']:
                    raise ValueError('candidate configuration failed')
                compiled = invoke([sys.executable, str(ROOT / 'scripts/candidate_build_receipt.py'), '--root', str(ROOT),
                    '--build-dir', str(build), '--candidate', str(candidate), '--receipt', str(receipt)], env)
                report['phases']['build'] = compiled
                if compiled['exit_code']:
                    raise ValueError('current candidate build or receipt failed')
            validate_receipt(receipt, ROOT, candidate, [implementation])
            env.update(IGI_CANDIDATE_EXE=str(candidate), IGI_CANDIDATE_BUILD_RECEIPT=str(receipt))
            report['candidate_sha256'] = digest(candidate)
        manifest=ROOT/'docs/fidelity/native-verification.json'
        if not args.static_only:
            manifest=build_evidence_manifest(ROOT,address,record,digest(binary),digest(implementation),candidate,receipt,folder)
            report['build_evidence_manifest']=str(manifest)
        if not args.static_only:
            env['FRIDA_HOST'] = runtime.enter_context(runtime_endpoint(config, folder))
        command = [sys.executable, str(ROOT / 'scripts/native_gate.py'), '--root', str(ROOT),
            '--manifest', str(manifest), '--address', address, '--file', str(implementation.relative_to(ROOT)), '--binary', str(binary),
            '--threshold', 'VERIFIED', '--cfg-threshold', '0.85', '--require-semantic-evidence',
            '--require-behavioral-tests', '--report', str(static_report)]
        if getattr(args,'angr_evidence',None):
            command += ['--angr-evidence',str(args.angr_evidence),'--angr-evidence-sha256',getattr(args,'angr_evidence_sha256',None) or '']
        gate = invoke(command, env, 1800)
        report['phases']['static_semantic_behavioral'] = gate
        if gate['exit_code']:
            raise ValueError('strict native gate failed; see static-gate.json and retained command output')
        from verification_scope import load_scope
        required, _, gate_data = load_scope(static_report)
        if required != {address}:
            raise ValueError('gate must check exactly the requested nonempty native scope')
        if args.static_only:
            # Fail closed: a static-only gate pass can never be accepted. Runtime
            # capture and typed comparison are mandatory before any native can pass.
            report['result'] = 'blocked'
            report['error'] = 'static-only verification cannot be accepted; runtime capture and typed comparison are required'
            return report, 1
        probe = args.probe
        if probe is None:
            probe_path = record.get('verification', {}).get('runtime_probe', {}).get('path')
            probe = ROOT / probe_path if probe_path else None
        if probe is None or not probe.is_file():
            raise ValueError('missing reviewed typed gameplay probe; direct-call replay alone does not prove wiring')
        contracts = [row.get('runtime_contract', {}) for row in gate_data['checked_natives']]
        if any(c.get('probe_sha256') != digest(probe) for c in contracts):
            raise ValueError('probe hash differs from the current source-semantic runtime contract')
        capture = invoke([sys.executable, str(ROOT / 'scripts/frida_scoped_capture.py'),
            '--gate-report', str(static_report), '--candidate-exe', str(candidate), '--probe', str(probe),
            '--level', str(args.level), '--out', str(runtime_report)], env)
        report['phases']['paired_runtime_capture'] = capture
        if capture['exit_code']:
            raise ValueError('paired runtime capture failed')
        captured = json.loads(runtime_report.read_text())
        if captured.get('kind') != 'igi-paired-native-runtime' or captured.get('result') != 'passed':
            raise ValueError('paired capture report did not pass')
        checked = invoke([sys.executable, str(ROOT / 'scripts/check_frida_differential.py'),
            '--report', str(runtime_report), '--gate-report', str(static_report), '--candidate-exe', str(candidate)], env)
        report['phases']['typed_runtime_comparison'] = checked
        if checked['exit_code']:
            raise ValueError('typed runtime comparison failed')
        validate_receipt(receipt, ROOT, candidate, [implementation])
        if digest(implementation) != report['implementation_sha256'] or digest(binary) != report['binary_sha256']:
            raise ValueError('source or retail changed during verification')
        report.update(result='passed', verified=True, scope=f'Native {address}, level {args.level}, reviewed probe observations only')
        return report, 0
    except (OSError, ValueError, KeyError, TypeError, subprocess.SubprocessError) as error:
        report['error'] = str(error)
        return report, 1
    finally:
        runtime.close()
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(report, indent=2) + '\n')
        print('True' if report['verified'] else 'False')
        print(f"{report['result']}: {report.get('error', report.get('scope', 'partial gate only'))}")
        print(f'report: {output}')


def scan_changed_native_addresses() -> list[str]:
    from native_ctest import discover, require_scope
    scope = discover(ROOT, scope='changed')
    require_scope(scope)
    return [row['address'] for row in scope['natives']]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', type=lambda value: f'0x{int(value, 0):08X}')
    parser.add_argument('--angr-evidence',type=Path)
    parser.add_argument('--angr-evidence-sha256')
    parser.add_argument('--level', type=int, default=1, choices=range(1, 15))
    parser.add_argument('--binary', type=Path)
    parser.add_argument('--candidate', type=Path)
    parser.add_argument('--receipt', type=Path)
    parser.add_argument('--probe', type=Path, help='Reviewed typed gameplay probe, used unchanged in both images')
    parser.add_argument('--skip-build', action='store_true', help='Require and revalidate an existing build receipt')
    parser.add_argument('--static-only', action='store_true', help='Run strict native gate only; never full feature acceptance')
    parser.add_argument('--auto', action='store_true', help='Scan changed/new source files for address-bound natives and run the full end-to-end static+runtime verification for each changed native')
    parser.add_argument('--report', type=Path)
    args = parser.parse_args()
    if args.auto:
        # One captain/worker lock and one chronological queue for every entry.
        # The lower-level one-address strict checker is called by that worker.
        if args.static_only or args.probe or args.skip_build or args.binary or args.candidate or args.receipt:
            parser.error('--auto uses the canonical configured build and per-native probes; use --address for low-level overrides')
        command = [sys.executable, str(ROOT / 'scripts/native_autoverify.py'), 'run',
                   '--scope', 'changed', '--level', str(args.level), '--retry']
        code = subprocess.run(command, cwd=ROOT).returncode
        if args.report:
            state = ROOT / 'build/native-auto/state.json'
            args.report.parent.mkdir(parents=True,exist_ok=True)
            args.report.write_bytes(state.read_bytes())
        return code
    addresses = [args.address] if args.address else []
    if not addresses:
        report = dict(schema_version=1, kind='igi-one-native-verification', address=args.address, run_id='', result='blocked', verified=False, phases={}, error='no address-bound changed native detected; new code must map to a retail address before static+runtime comparison')
        output = args.report or (ROOT / 'build/native-one' / 'auto' / 'report.json')
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(report, indent=2) + '\n')
        print('False')
        print(f"blocked: {report.get('error', 'no address-bound changed native')}")
        print(f'report: {output}')
        return 1
    if len(addresses) == 1:
        return run(addresses[0], args)[1]
    aggregate = dict(schema_version=1, kind='igi-one-native-e2e-changed-scope', addresses=addresses, result='blocked', verified=False, phases={}, differences=[])
    failed = False
    saved_report = args.report
    args.report = None
    for address in addresses:
        report, code = run(address, args)
        aggregate.setdefault('results', []).append(report)
        if code:
            failed = True
            aggregate['differences'].append(f"{address}: {report.get('error', 'verification failed')}")
    args.report = saved_report
    aggregate['result'] = 'blocked' if failed else 'passed'
    aggregate['verified'] = not failed
    output = args.report or (ROOT / 'build/native-one' / 'auto' / 'report.json')
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(aggregate, indent=2) + '\n')
    print('True' if aggregate['verified'] else 'False')
    print(f"{aggregate['result']}: changed-native end-to-end scope with {len(addresses)} address(es)")
    print(f'report: {output}')
    return 0 if aggregate['verified'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
