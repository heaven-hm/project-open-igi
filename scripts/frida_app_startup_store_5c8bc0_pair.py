#!/usr/bin/env python3
"""Typed pre-resume paired Frida replay for retail native 0x0048F0D0."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import pathlib

from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
from verification_scope import stage_candidate
from frida_dynobj_link_dynamic_cube_pair import run_one, process_names, sha256

ROOT = Path(__file__).resolve().parents[1]
RETAIL = {'entry': '0x0048F0D0', 'state': '0x005C8BC0'}
SYMBOLS = {'entry': '_Native_0048F0D0', 'state': '_Native_005C8BC0'}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', default='0x0048F0D0')
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--binary', type=Path, default=Path('/Users/haseeb-mir/Downloads/IGI/igi.exe'))
    parser.add_argument('--binary-sha256')
    parser.add_argument('--implementation', type=Path, default=ROOT / 'engine/src/retail/core/native_0048f0d0.cpp')
    parser.add_argument('--implementation-sha256')
    parser.add_argument('--candidate', type=Path, default=None)
    parser.add_argument('--receipt', type=Path, default=None)
    parser.add_argument('--out', type=Path, default=None)
    args = parser.parse_args()
    # The gate passes the candidate, its receipt and the output path through
    # the environment when it drives the runner.
    if args.candidate is None:
        candidate_from_env = os.environ.get('IGI_CANDIDATE_EXE', '')
        args.candidate = pathlib.Path(candidate_from_env) if candidate_from_env \
            else ROOT / 'build-win32-no-verify-gate/engine/platform/windows/open-igi.exe'
    if args.receipt is None:
        receipt_from_env = os.environ.get('IGI_CANDIDATE_BUILD_RECEIPT', '')
        args.receipt = pathlib.Path(receipt_from_env) if receipt_from_env \
            else ROOT / 'build/diagnostic-candidate-build.json'
    if args.out is None:
        args.out = ROOT / 'build/runtime-traces/0x0048F0D0-frida-pair.json'
    try:
        import frida
    except ImportError:
        python = Path.home() / 'Library/Application Support/pipx/venvs/frida-tools/bin/python'
        os.execv(str(python), [str(python), str(Path(__file__).resolve()), *sys.argv[1:]])
    if int(args.address, 16) != 0x0048F0D0 or args.root.resolve() != ROOT:
        raise ValueError('runner is bound to 0x0048F0D0 and this source tree')
    source = args.implementation.resolve()
    if source != ROOT / 'engine/src/retail/core/native_0048f0d0.cpp':
        raise ValueError('runner must measure the engine implementation')
    if args.implementation_sha256 and sha256(source) != args.implementation_sha256:
        raise ValueError('implementation differs from requested source identity')
    candidate = args.candidate.resolve()
    validate_receipt(args.receipt, ROOT, candidate, [source])
    table = subprocess.run(['i686-w64-mingw32-nm', '--defined-only', str(candidate)],
                           capture_output=True, text=True, check=True, timeout=60).stdout
    layout = {}
    for field, symbol in SYMBOLS.items():
        match = re.search(r'^([0-9a-fA-F]+) [A-Za-z] ' + re.escape(symbol) + r'$', table, re.M)
        if not match:
            raise RuntimeError('required candidate symbol is absent: ' + symbol)
        layout[field] = f'0x{int(match.group(1), 16):08X}'
    probes = [ROOT / 'scripts/frida_dynobj_link_dynamic_cube_probe.js',
              ROOT / 'scripts/frida_app_startup_store_5c8bc0_probe.js']
    probe = '\n'.join(path.read_text() for path in probes)
    raw_values = [0, 1, 2, 10, 0x7f, 0x80, 0xff, 0x100, 0x7fffffff, -0x80000000, -1]
    fixtures = [{'value': value, 'initial_state': 0x13570000 + index}
                for index, value in enumerate(raw_values)]
    retail = args.binary.resolve()
    if args.binary_sha256 and sha256(retail) != args.binary_sha256:
        raise ValueError('retail differs from requested image identity')
    device = frida.get_device_manager().add_remote_device(os.environ.get('FRIDA_HOST', '127.0.0.1:27042'))
    with capture_lease(device, process_names):
        report = {'schema_version': 1, 'kind': 'behavioral-test-report', 'address': '0x0048F0D0',
                  'scope': 'startup-scalar-store', 'result': 'incomplete', 'executed': False,
                  'oracle': 'retail-binary-execution', 'binary_sha256': sha256(retail),
                  'implementation_sha256': sha256(source), 'candidate_sha256': sha256(candidate),
                  'probe_sha256': hashlib.sha256(probe.encode()).hexdigest(),
                  'candidate_symbols': layout,
                  'observable_fields': ['return_value', 'state_delta', 'side_effects'],
                  'runner': {'path': 'scripts/frida_app_startup_store_5c8bc0_pair.py',
                             'sha256': sha256(Path(__file__).resolve()), 'command': [sys.executable, *sys.argv]}}
        args.out.parent.mkdir(parents=True, exist_ok=True)
        try:
            report['retail'] = run_one(device, retail, 'retail', 'igi.exe', json.dumps(RETAIL),
                                       probe, fixtures, 8, probe_before_resume=True)
            with stage_candidate(candidate, retail.parent) as staged:
                validate_receipt(args.receipt, ROOT, staged, [source])
                report['candidate'] = run_one(device, staged, 'candidate', staged.name, json.dumps(layout),
                                              probe, fixtures, 8, probe_before_resume=True)
            expected, actual = (report[key]['probe']['calls'] for key in ('retail', 'candidate'))
            if len(expected) != len(fixtures) or len(actual) != len(fixtures):
                raise RuntimeError('incomplete ordered probe sequence')
            cases = []
            for index, fixture in enumerate(fixtures):
                if expected[index]['input'] != fixture or actual[index]['input'] != fixture:
                    raise RuntimeError('probe inputs/order differ')
                reference, observed = expected[index]['output'], actual[index]['output']
                cases.append({'id': f'case-{index}', 'input': fixture,
                              'categories': ['boundary' if index in (0, 1, 8, 9, 10) else 'nominal'],
                              'reference_output': reference, 'candidate_output': observed,
                              'result': 'passed' if reference == observed else 'failed'})
            failed = sum(case['result'] != 'passed' for case in cases)
            report.update(cases=cases, executed=True, result='failed' if failed else 'passed',
                          tests_total=len(cases), tests_failed=failed, tests_passed=len(cases)-failed)
        except Exception as error:
            report.update(result='failed', error=repr(error), failed_capture=getattr(error, 'record', None))
            raise
        finally:
            args.out.write_text(json.dumps(report, indent=2) + '\n')
        print(json.dumps(report))
        return int(report['result'] != 'passed')


if __name__ == '__main__':
    raise SystemExit(main())
