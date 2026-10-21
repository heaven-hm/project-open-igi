#!/usr/bin/env python3
"""Paired typed Frida replay of TextureModifier_Push; no rendering parity claim."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import sys

from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
from verification_scope import stage_candidate
from frida_dynobj_link_dynamic_cube_pair import run_one, process_names, sha256

ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = {
    'entry': '__ZN3igi6retail7terrain20TextureModifier_PushEPv',
    'count': '__ZN3igi6retail7terrain8g_b97df8E',
    'stack': '__ZN3igi6retail7terrain8g_b97da0E',
    'mask': '__ZN3igi6retail7terrain8g_ba5060E',
}
RETAIL = dict(entry='0x00505E40', count='0x00B97DF8', stack='0x00B97DA0', mask='0x00BA5060')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', default='0x00505E40')
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--binary', type=Path, default=Path('/Users/haseeb-mir/Downloads/IGI/igi.exe'))
    parser.add_argument('--binary-sha256')
    parser.add_argument('--implementation', type=Path, default=ROOT / 'engine/src/retail/terrain/terrain_loaders.cpp')
    parser.add_argument('--implementation-sha256')
    parser.add_argument('--candidate', type=Path, default=ROOT / 'build-win32-no-verify/engine/platform/windows/open-igi.exe')
    parser.add_argument('--receipt', type=Path, default=ROOT / 'build/diagnostic-candidate-build.json')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/runtime-traces/0x00505E40-frida-pair.json')
    args = parser.parse_args()
    try:
        import frida
    except ImportError:
        python = Path.home() / 'Library/Application Support/pipx/venvs/frida-tools/bin/python'
        os.execv(str(python), [str(python), str(Path(__file__).resolve()), *sys.argv[1:]])
    if int(args.address, 16) != 0x00505E40 or args.root.resolve() != ROOT:
        raise ValueError('runner is bound to the analyzed texture modifier native and this source tree')
    source = args.implementation.resolve()
    if source != ROOT / 'engine/src/retail/terrain/terrain_loaders.cpp':
        raise ValueError('runner must compile/measure the engine implementation')
    if args.implementation_sha256 and sha256(source) != args.implementation_sha256:
        raise ValueError('implementation differs from requested source identity')
    mode = ROOT / 'engine/include/igi/retail/terrain/terrain_loaders.hpp'
    candidate = args.candidate.resolve()
    receipt = validate_receipt(args.receipt, ROOT, candidate, [source])
    table = subprocess.run(['i686-w64-mingw32-nm', '--defined-only', str(candidate)],
                           capture_output=True, text=True, check=True, timeout=60).stdout
    layout = {}
    for field, symbol in SYMBOLS.items():
        match = re.search(r'^([0-9a-fA-F]+) [A-Za-z] ' + re.escape(symbol) + r'$', table, re.M)
        if not match:
            raise RuntimeError('required candidate symbol is absent: ' + symbol)
        layout[field] = f'0x{int(match.group(1), 16):08X}'
    probes = [ROOT / 'scripts/frida_dynobj_link_dynamic_cube_probe.js',
              ROOT / 'scripts/frida_texture_modifier_push_probe.js']
    probe = '\n'.join(p.read_text() for p in probes)
    fixtures = [dict(index=i, enabled=e, value=v, count=c, mask=m)
                for i,e,v,c,m in ((-1,0,7,0,0x12345678), (0,0,7,0,0x12345678),
                    (2,0,9,19,0xffffffff), (0,1,0,0,0), (0,1,7,0,0x12345678),
                    (2,1,9,19,0xabcdef12), (1,0xffffffff,0xffffffff,3,0xffffffff),
                    (0,1,0xffffff,4,0x12345678))]
    retail = args.binary.resolve()
    if args.binary_sha256 and sha256(retail) != args.binary_sha256:
        raise ValueError('retail differs from requested image identity')
    data_files = [retail.parent / 'config.qvm', retail.parent / 'MISSIONS/location0/LEVEL1/OBJECTS.QVM']
    identities = {str(p): sha256(p) for p in (retail, candidate, source, mode, *probes, *data_files, Path(__file__).resolve(),
                                                         ROOT / 'scripts/frida_dynobj_link_dynamic_cube_pair.py', ROOT / 'scripts/frida_capture_lease.py', args.receipt)}
    if identities[str(retail)] != 'dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':
        raise RuntimeError('retail image differs from the analyzed image')
    device = frida.get_device_manager().add_remote_device(os.environ.get('FRIDA_HOST', '127.0.0.1:27042'))
    with capture_lease(device, process_names):
        report = dict(schema_version=1, kind='behavioral-test-report', address='0x00505E40',
                      scope='texture-modifier-push-pre-resume-nonfatal-controlled-context',
                      result='incomplete', executed=False, oracle='retail-binary-execution', source_identities=identities,
                      runner={'path': 'scripts/frida_texture_modifier_push_pair.py', 'sha256': sha256(Path(__file__).resolve()),
                              'command': [sys.executable, *sys.argv]},
                      float_tolerance={'absolute': 0, 'relative': 0},
                      binary_sha256=sha256(retail), implementation_sha256=sha256(source),
                      candidate_sha256=sha256(candidate), probe_sha256=hashlib.sha256(probe.encode()).hexdigest(),
                      build_receipt_sha256=sha256(args.receipt), candidate_symbols=layout, observable_fields=['return_value', 'state_delta', 'side_effects'])
        args.out.parent.mkdir(parents=True, exist_ok=True)
        try:
            report['retail'] = run_one(device, retail, 'retail', 'igi.exe', json.dumps(RETAIL), probe, fixtures, 8, probe_before_resume=True)
            with stage_candidate(candidate, retail.parent) as staged:
                validate_receipt(args.receipt, ROOT, staged, [source])
                report['candidate'] = run_one(device, staged, 'candidate', staged.name, json.dumps(layout), probe, fixtures, 8, probe_before_resume=True)
            if any(report[k]['probe'].get('globalStateRestored') is not True for k in ('retail', 'candidate')):
                raise RuntimeError('native global state restoration was not verified')
            expected, actual = (report[k]['probe']['calls'] for k in ('retail', 'candidate'))
            if len(expected) != len(fixtures) or len(actual) != len(fixtures):
                raise RuntimeError('incomplete ordered probe sequence')
            cases = []
            for index, fixture in enumerate(fixtures):
                if expected[index]['input'] != fixture or actual[index]['input'] != fixture:
                    raise RuntimeError('probe inputs/order differ')
                a, b = expected[index]['output'], actual[index]['output']
                cases.append(dict(id=f'case-{index}', input=fixture,
                                  categories=['nominal' if fixture['index'] == 0 and fixture['enabled'] == 1 else 'boundary'],
                                  reference_output=a, candidate_output=b,
                                  result='passed' if a == b else 'failed'))
            if any(sha256(Path(path)) != value for path, value in identities.items()):
                raise RuntimeError('source, probe or executable changed during capture')
            failed = sum(c['result'] != 'passed' for c in cases)
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
