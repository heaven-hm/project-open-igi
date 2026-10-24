#!/usr/bin/env python3
"""Capture an existing candidate's mutex prefix after retail, under the shared lease.

Diagnostic evidence only. Source freshness/acceptance is deliberately not inferred.
Resolves WinMain from the exact candidate, hashes both images, and rechecks them
after capture. Refuses to overlap any pre-existing game or another lease owner.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from frida_capture_lease import capture_lease
import frida

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--candidate', type=Path, required=True)
    parser.add_argument('--receipt', type=Path, help='Validate the freshly compiled candidate before and after capture')
    args = parser.parse_args()
    args.out.mkdir(parents=True, exist_ok=False)
    retail = Path('/Users/haseeb-mir/Downloads/IGI/igi.exe')
    probe = ROOT / 'scripts/tester/frida_main_mutex_prefix.js'
    digest = lambda path: hashlib.sha256(path.read_bytes()).hexdigest()
    identities = {str(path): digest(path) for path in (retail, args.candidate, probe)}
    if args.receipt:
        from candidate_build_receipt import validate_receipt
        validate_receipt(args.receipt, ROOT, args.candidate,
                         required_sources=[ROOT / 'engine/src/retail/core/main.cpp'])
        identities[str(args.receipt)] = digest(args.receipt)
    if identities[str(retail)] != 'dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':
        raise RuntimeError('retail image mismatch')
    symbols = subprocess.check_output(['i686-w64-mingw32-nm', str(args.candidate)], text=True)
    matches = [int(line.split()[0], 16) for line in symbols.splitlines()
               if line.split()[-1:] == ['_WinMain@16']]
    if len(matches) != 1:
        raise RuntimeError('candidate WinMain symbol is ambiguous or absent')
    device = frida.get_device_manager().add_remote_device('127.0.0.1:27042')
    names = lambda dev: [p.name for p in dev.enumerate_processes()
                         if p.name.lower() in ('igi.exe', 'igi1.exe', 'open-igi.exe')]
    reports = {}
    with capture_lease(device, names, timeout=2):
        for side, image, address in [('retail', retail, 0x48F530),
                                      ('candidate', args.candidate, matches[0])]:
            output = args.out / (side + '.json')
            command = [sys.executable, str(ROOT / 'scripts/tester/frida_probe_driver.py'),
                       '--exe', str(image), '--probe', str(probe), '--target', hex(address),
                       '--name', 'WinMain', '--observe', '3', '--level', '1',
                       '--out', str(output), '--expect-exe-sha256', identities[str(image)]]
            result = subprocess.run(command, capture_output=True, text=True, timeout=50)
            (args.out / (side + '-driver.log')).write_text(result.stdout + result.stderr)
            reports[side] = json.loads(output.read_text())
            if result.returncode or names(device):
                raise RuntimeError(f'{side} capture or cleanup failed')
        for path, expected in identities.items():
            if digest(Path(path)) != expected:
                raise RuntimeError('image/probe changed during pair: ' + path)
        if args.receipt:
            validate_receipt(args.receipt, ROOT, args.candidate,
                             required_sources=[ROOT / 'engine/src/retail/core/main.cpp'])
    order = lambda report: [e['name'] for e in report.get('events', [])
                             if e.get('type') == 'mutex_prefix' and e.get('phase') == 'entry']
    summary = {'kind': 'igi-main-mutex-prefix-diagnostic', 'identities': identities,
               'candidate_main_va': hex(matches[0]), 'orders': {s: order(r) for s, r in reports.items()},
               'scope': 'natural successful mutex path, entry through CoInitialize',
               'verified': False, 'candidate_source_freshness':
                   'receipt validated before and after capture' if args.receipt else
                   'not established; existing binary diagnostic'}
    summary['same_api_order'] = summary['orders']['retail'] == summary['orders']['candidate']
    expected = ['WinMain', 'CreateMutexA', 'GetLastError', 'GetLastError', 'CoInitialize']
    checks = []
    for side, report in reports.items():
        stream = [e for e in report.get('events', []) if e.get('type') == 'mutex_prefix']
        checks.append({'side': side, 'check': 'exact successful-path API order',
                       'pass': order(report) == expected})
        mutex = [e for e in stream if e['name'] == 'CreateMutexA' and e['phase'] == 'entry']
        checks.append({'side': side, 'check': 'CreateMutexA typed arguments',
                       'pass': len(mutex) == 1 and mutex[0].get('securityIsNull') is True and
                           mutex[0].get('initialOwner') == 1 and mutex[0].get('mutexName') ==
                           ('IGIMUTEX' if side == 'retail' else 'IGI1MUTEX')})
        returns = [e for e in stream if e['name'] == 'CreateMutexA' and e['phase'] == 'return']
        checks.append({'side': side, 'check': 'successful non-null handle',
                       'pass': len(returns) == 1 and returns[0]['value'] != 0})
        errors = [e['value'] for e in stream if e['name'] == 'GetLastError' and e['phase'] == 'return']
        checks.append({'side': side, 'check': 'both observed error returns are zero',
                       'pass': errors == [0, 0], 'observed': errors})
        coinit = [e for e in stream if e['name'] == 'CoInitialize' and e['phase'] == 'entry']
        checks.append({'side': side, 'check': 'CoInitialize null argument',
                       'pass': len(coinit) == 1 and coinit[0].get('reservedIsNull') is True})
        checks.append({'side': side, 'check': 'capture has no probe failures and owned process exited',
                       'pass': not report.get('failures') and report.get('cleanup', {}).get('owned_process_exited') is True})
    summary['checks'] = checks
    summary['prefix_checks_passed'] = all(c['pass'] for c in checks)
    summary['limits'] = ['Failure/null/duplicate-handle paths are not exercised.',
                        'IGI_TESTING_MUTEX changes only the candidate mutex name.',
                        'Whole WinMain, later startup, frame dispatch and gameplay remain unverified.']
    (args.out / 'comparison.json').write_text(json.dumps(summary, indent=2) + '\n')
    print(json.dumps(summary, indent=2))

if __name__ == '__main__':
    main()
