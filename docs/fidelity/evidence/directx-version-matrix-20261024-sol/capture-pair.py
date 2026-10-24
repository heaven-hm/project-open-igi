#!/usr/bin/env python3
"""Retain a sequential, typed 0x498480 diagnostic pair, never native acceptance."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
import frida

SCENARIOS = {
    'none': dict(capability=0x700, platform=2, eax=1,
                 modules={'DDRAW.DLL': 0, 'DINPUT.DLL': 0}, versions=0,
                 scope='Natural DirectX 7 successful path'),
    'version-api-zero': dict(capability=0, platform=0, eax=0, modules={}, versions=0,
                             scope='GetVersionExA return overridden to zero after real API execution'),
    'version-major-3': dict(capability=0, platform=2, eax=None, modules={}, versions=1,
                            scope='GetVersionExA output overridden to platform 2, major 3'),
    'version-major-4': dict(capability=0x300, platform=2, eax=1,
                            modules={'DINPUT.DLL': 0}, versions=1,
                            scope='GetVersionExA output overridden to platform 2, major 4'),
    'version-platform-1': dict(capability=0x700, platform=1, eax=1,
                               modules={'DDRAW.DLL': 0, 'DINPUT.DLL': 0}, versions=1,
                               scope='GetVersionExA platform output overridden to 1'),
}


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def stream(report):
    return sorted(report.get('entries', []) + report.get('returns', []) +
                  [e for e in report.get('events', [])
                   if e.get('type') in ('directx_api', 'directx_com')],
                  key=lambda e: e.get('sequence', -1))


def compare(reports, scenario='none'):
    if scenario not in SCENARIOS:
        raise ValueError('unknown scenario')
    expected = SCENARIOS[scenario]
    checks = []
    streams = {side: stream(report) for side, report in reports.items()}
    for side, report in reports.items():
        events = streams[side]
        terminals = [e for e in report.get('events', []) if e.get('type') == 'directx_summary']
        complete = (len(terminals) == 1 and terminals[0].get('nativeComplete') is True and
                    terminals[0].get('count') == terminals[0].get('completed') == 1 and
                    terminals[0].get('sequenceLength') == len(events) and len(events) > 2 and
                    [e.get('sequence') for e in events] == list(range(1, len(events) + 1)) and
                    len(report.get('entries', [])) == len(report.get('returns', [])) == 1)
        checks.append(dict(side=side, check='complete native sequence and terminal marker', passed=complete))
        checks.append(dict(side=side, check='no probe failures; owned process exited',
                           passed=not report.get('failures') and report.get('result') == 'observed' and
                           report.get('cleanup', {}).get('owned_process_exited') is True))
        checks.append(dict(side=side, check='observed DLL references balanced',
                           passed=len(terminals) == 1 and
                           terminals[0].get('ownedModuleReferences') == expected['modules']))
        checks.append(dict(side=side, check='requested failure injection count',
                           passed=len(terminals) == 1 and terminals[0].get('failureMode') == scenario and
                           terminals[0].get('injectedFailures') == (1 if scenario == 'version-api-zero' else 0) and
                           terminals[0].get('injectedVersions') == expected['versions']))
        checks.append(dict(side=side, check='scenario output values',
                           passed=len(report.get('returns', [])) == 1 and
                           report['returns'][0].get('capability') == expected['capability'] and
                           report['returns'][0].get('platform') == expected['platform'] and
                           (report['returns'][0].get('returnEqualsCapabilityPointer') is True
                            if scenario == 'version-major-3' else
                            report['returns'][0].get('eax') == expected['eax'])))
    # Only code locations differ across separately linked binaries. Every typed
    # value and every event remains in the comparison, including output states.
    def normalize(events):
        normalized = []
        for event in events:
            record = {k: v for k, v in event.items() if k not in ('address', 'callerRva', 'scope')}
            if scenario == 'version-major-3' and event['type'] == 'observed' and event['phase'] == 'return':
                # Retail 0x498503..516 returns the first output's address. Retain
                # raw EAX in the capture, compare its proven pointer role here.
                record['eax'] = {'role': 'capability-output-pointer',
                                 'matches': event.get('returnEqualsCapabilityPointer')}
            normalized.append(record)
        return normalized
    left, right = normalize(streams['retail']), normalize(streams['candidate'])
    differences = [dict(index=i + 1, retail=left[i] if i < len(left) else None,
                        candidate=right[i] if i < len(right) else None)
                   for i in range(max(len(left), len(right)))
                   if i >= len(left) or i >= len(right) or left[i] != right[i]]
    checks.append(dict(check='all ordered typed observations equal', passed=not differences))
    terminal = {side: [{k: v for k, v in e.items() if k != 'scope'}
                       for e in report.get('events', []) if e.get('type') == 'directx_summary']
                for side, report in reports.items()}
    checks.append(dict(check='terminal identity counts and DLL effects equal',
                       passed=terminal['retail'] == terminal['candidate']))
    return dict(kind='igi-directx-native-diagnostic-pair', retail_va='0x00498480',
                checks=checks, differences=differences,
                observations_match=all(c['passed'] for c in checks), verified=False,
                scope=expected['scope'] + ' on CrossOver, Level 1 startup',
                scenario=scenario,
                limits=['Other failure branches and other OS versions are untested.',
                        'A native terminal marker covers only this native; later startup can fail.',
                        'No production intake, independent semantic approval or full WinMain acceptance.'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--candidate', type=Path, required=True)
    parser.add_argument('--receipt', type=Path, required=True)
    parser.add_argument('--scenario', choices=SCENARIOS, default='none')
    args = parser.parse_args()
    args.candidate = args.candidate.resolve()
    args.out.mkdir(parents=True, exist_ok=False)
    retail = Path('/Users/haseeb-mir/Downloads/IGI/igi.exe')
    probe_source = ROOT / 'scripts/tester/frida_directx_probe.js'
    probe = (args.out / 'probe.js').resolve()
    probe.write_text('globalThis.IGI_DIRECTX_FAILURE_MODE = ' + json.dumps(args.scenario) + ';\n' +
                     probe_source.read_text())
    required = [ROOT / 'scripts/GameNatives/display/00498480-Native_00498480.cpp',
                ROOT / 'build/native-analysis/directx-candidate-sol/main.cpp']
    validate_receipt(args.receipt, ROOT, args.candidate, required_sources=required)
    paths = [retail, args.candidate, probe, probe_source, args.receipt, Path(__file__),
             ROOT / 'scripts/tester/frida_probe_driver.py']
    identities = {str(p): digest(p) for p in paths}
    if identities[str(retail)] != 'dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':
        raise ValueError('wrong retail image')
    symbols = subprocess.check_output(['i686-w64-mingw32-nm', '-n', str(args.candidate)], text=True)
    texts = [(int(parts[0], 16), parts[2]) for line in symbols.splitlines()
             if len(parts := line.split()) == 3 and parts[1] in ('t', 'T')]
    entries = [a for a, name in texts if name == '_Native_00498480']
    if len(entries) != 1:
        raise ValueError('missing or ambiguous candidate native symbol')
    address = entries[0]
    extent = min(a for a, name in texts if a > address) - address
    device = frida.get_device_manager().add_remote_device('127.0.0.1:27042')
    names = lambda dev: [p.name for p in dev.enumerate_processes()
                         if p.name.lower() in ('igi.exe', 'igi1.exe', 'open-igi.exe')]
    reports = {}
    with capture_lease(device, names, timeout=2):
        for side, image, va, size in [('retail', retail, 0x498480, 0x44a),
                                      ('candidate', args.candidate, address, extent)]:
            output = args.out / (side + '.json')
            command = [sys.executable, str(ROOT / 'scripts/tester/frida_probe_driver.py'),
                       '--exe', str(image), '--probe', str(probe), '--target', hex(va),
                       '--native-size', hex(size), '--arg-words', '2', '--name', 'Native_00498480',
                       '--observe', '15', '--level', '1', '--out', str(output),
                       '--expect-exe-sha256', identities[str(image)]]
            result = subprocess.run(command, capture_output=True, text=True, timeout=55)
            (args.out / (side + '-driver.log')).write_text(result.stdout + result.stderr)
            reports[side] = json.loads(output.read_text())
            if result.returncode or names(device):
                raise RuntimeError(side + ' capture or cleanup failed')
        for path, expected in identities.items():
            if digest(path) != expected:
                raise ValueError('capture input changed: ' + path)
        validate_receipt(args.receipt, ROOT, args.candidate, required_sources=required)
    comparison = compare(reports, args.scenario)
    comparison.update(identities=identities, candidate_va=hex(address), candidate_extent=hex(extent))
    (args.out / 'comparison.json').write_text(json.dumps(comparison, indent=2) + '\n')
    print(json.dumps(comparison, indent=2))
    return 0 if comparison['observations_match'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
