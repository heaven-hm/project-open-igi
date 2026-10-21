#!/usr/bin/env python3
"""Compare retail 0x004C67E0 with the current diagnostic OpenIGI x86 candidate.

This direct-call probe compares return values and the first caller-stack word,
which retail uses as scratch. Retail runs first; both Level 1 processes are
terminated before the next starts. It does not prove natural gameplay reaches
the candidate native.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
import re
import sys
import time

try:
    import frida
except ModuleNotFoundError:
    frida = None


ROOT = pathlib.Path(__file__).resolve().parent.parent
GAME_DIR = pathlib.Path('/Users/haseeb-mir/Downloads/IGI')
RETAIL_EXE = GAME_DIR / 'igi.exe'
RETAIL_VA = '0x004C67E0'
SYMBOL = '__ZN3igi6retail6dynobj24DynObj_LinkDynamicCube_7Ejjjjjj'
PROBE = ROOT / 'scripts/frida_dynobj_link_dynamic_cube_probe.js'


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def win_path(path: pathlib.Path) -> str:
    prefix = '/Users/'
    value = str(path.resolve())
    if not value.startswith(prefix):
        raise ValueError(f'path is outside the CrossOver Z: mapping: {value}')
    return 'Z:\\' + value[1:].replace('/', '\\')


def process_names(device) -> list[str]:
    return [process.name.lower() for process in device.enumerate_processes()
            if process.name.lower() in {'igi.exe', 'open-igi.exe'}
            or (process.name.lower().startswith('open-igi-probe-') and process.name.lower().endswith('.exe'))]


def cases() -> list[list[int]]:
    boundary = [0, 1, 0x1FFF, 0x2000, 0x2001, 0x3FFF, 0x4000, 0x4001,
                0x7FFFFFFF, 0x80000000, 0xFFFFFFFF]
    vectors = [[0, 0, 0, value, 0, 0] for value in boundary]
    vectors.extend([0, 0, 0, 1 << bit, 0, 0] for bit in range(32))
    vectors.extend([0, 0, 0, 0, 1 << bit, 0] for bit in range(32))
    vectors.extend([0, 0, 0, 0, 0, 1 << bit] for bit in range(32))
    vectors.append([0x11111111, 0x22222222, 0x33333333,
                    0x91111111, 0xA2222222, 0xB3333333])
    unique = []
    seen = set()
    for vector in vectors:
        key = tuple(vector)
        if key not in seen:
            seen.add(key)
            unique.append(vector)
    return unique


class CaptureError(RuntimeError):
    def __init__(self, record, error):
        self.record = record
        super().__init__(str(error))


def run_one(device, exe: pathlib.Path, name: str, process_module: str, function_va: str,
            probe_source: str, input_cases: list[list[int]], observe_seconds: float,
            probe_before_resume: bool = False) -> dict:
    pid = None
    session = None
    script = None
    record = {'name': name, 'executable': str(exe), 'executable_sha256': sha256(exe),
              'arguments': ['window', 'level1'], 'cwd': str(GAME_DIR),
              'result': 'incomplete'}
    try:
        pid = device.spawn([win_path(exe), 'window', 'level1'], cwd=win_path(GAME_DIR))
        session = device.attach(pid)
        script = session.create_script(probe_source)
        script.load()
        if probe_before_resume:
            record['probe_phase'] = 'before-primary-thread-resume'
            record['probe'] = script.exports_sync.probe(process_module, function_va, input_cases)
        device.resume(pid)
        time.sleep(observe_seconds)
        if pid not in {p.pid for p in device.enumerate_processes()}:
            raise RuntimeError('owned game exited before the startup observation completed')
        if name == 'retail':
            record['escape'] = script.exports_sync.press_escape()
            if not record['escape'].get('sent'):
                raise RuntimeError(f"Escape was not delivered to the IGI window: {record['escape']}")
            time.sleep(1)
        if not probe_before_resume:
            record['probe'] = script.exports_sync.probe(process_module, function_va, input_cases)
        record['result'] = 'captured'
    except Exception as error:
        record['result'] = 'failed'
        record['error'] = repr(error)
        raise CaptureError(record, error) from error
    finally:
        if pid is not None:
            try:
                device.kill(pid)
                record['owned_process_terminated'] = True
            except Exception as error:
                record['cleanup_error'] = repr(error)
        if session is not None:
            try:
                session.detach()
            except Exception:
                pass
    if process_names(device):
        raise RuntimeError(f'game process remained after {name} capture: {process_names(device)}')
    return record


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument('--address', default=RETAIL_VA)
    parser.add_argument('--binary', type=pathlib.Path, default=RETAIL_EXE)
    parser.add_argument('--binary-sha256')
    parser.add_argument('--implementation', type=pathlib.Path,
                        default=ROOT / 'engine/src/retail/dynobj/dynamic_cube.cpp')
    parser.add_argument('--implementation-sha256')
    parser.add_argument('--root', type=pathlib.Path, default=ROOT)
    parser.add_argument('--candidate-exe', type=pathlib.Path,
                        default=pathlib.Path(os.environ.get(
                            'IGI_CANDIDATE_EXE', str(ROOT / 'build-win32-no-verify/engine/platform/windows/open-igi.exe'))))
    parser.add_argument('--candidate-va', help='Override only when resolving the exact current candidate symbol.')
    parser.add_argument('--observe-seconds', type=float, default=8.0)
    parser.add_argument('--out', type=pathlib.Path)
    args = parser.parse_args()

    if frida is None:
        frida_python = pathlib.Path.home() / 'Library/Application Support/pipx/venvs/frida-tools/bin/python'
        if not frida_python.is_file():
            raise RuntimeError('Frida Python runtime is unavailable')
        os.execv(str(frida_python), [str(frida_python), str(pathlib.Path(__file__).resolve()), *sys.argv[1:]])

    candidate = args.candidate_exe.resolve()
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    address = '0x' + args.address[-8:].upper()
    if address != RETAIL_VA:
        raise ValueError(f'this runner supports only {RETAIL_VA}, got {args.address}')
    if not binary.is_file() or not implementation.is_file() or not candidate.is_file():
        raise FileNotFoundError('retail or candidate executable is missing')
    binary_sha256 = sha256(binary)
    implementation_sha256 = sha256(implementation)
    if args.binary_sha256 and args.binary_sha256.lower() != binary_sha256:
        raise RuntimeError('retail image hash differs from the native gate input')
    if args.implementation_sha256 and args.implementation_sha256.lower() != implementation_sha256:
        raise RuntimeError('implementation hash differs from the native gate input')
    if candidate.stat().st_mtime_ns < implementation.stat().st_mtime_ns:
        raise RuntimeError('candidate executable predates the implementation source; rebuild the x86 candidate')
    import subprocess
    symbol_table = subprocess.run(['i686-w64-mingw32-nm', '--defined-only', str(candidate)],
                                  text=True, capture_output=True, check=True, timeout=60).stdout
    found = re.search(rf'^([0-9a-fA-F]+) T {re.escape(SYMBOL)}$', symbol_table, re.M)
    if not found and not args.candidate_va:
        raise RuntimeError('current candidate executable does not contain DynObj_LinkDynamicCube_7')
    candidate_va = args.candidate_va or f'0x{int(found.group(1), 16):08X}'

    host = os.environ.get('FRIDA_HOST', '127.0.0.1:27042')
    device = frida.get_device_manager().add_remote_device(host)
    active = process_names(device)
    if active:
        raise RuntimeError(f'refusing to start a paired run while a game is already active: {active}')

    probe_bytes = PROBE.read_bytes()
    probe_source = probe_bytes.decode('utf-8')
    input_cases = cases()
    report = {
        'schema_version': 1,
        'kind': 'behavioral-test-report',
        'address': address,
        'native': 'DynObj_LinkDynamicCube_7',
        'scope': 'direct scalar-call comparison of return and first argument stack slot; does not establish natural gameplay reachability',
        'binary_sha256': binary_sha256,
        'implementation_sha256': implementation_sha256,
        'retail_sha256': binary_sha256,
        'candidate_sha256': sha256(candidate),
        'candidate_function_va': candidate_va,
        'probe_sha256': hashlib.sha256(probe_bytes).hexdigest(),
        'frida_client_version': frida.__version__,
        'frida_host': host,
        'input_cases': input_cases,
    }
    report['retail'] = run_one(device, binary, 'retail', 'igi.exe', address,
                               probe_source, input_cases, args.observe_seconds)
    report['candidate'] = run_one(device, candidate, 'candidate', 'open-igi.exe', candidate_va,
                                  probe_source, input_cases, args.observe_seconds)
    retail_calls = report['retail']['probe']['calls']
    candidate_calls = report['candidate']['probe']['calls']
    mismatches = [{'index': i, 'retail': retail_calls[i], 'candidate': candidate_calls[i]}
                  for i in range(len(input_cases)) if retail_calls[i] != candidate_calls[i]]
    behavioral_cases = []
    for index, (retail_call, candidate_call) in enumerate(zip(retail_calls, candidate_calls)):
        behavioral_cases.append({
            'id': f'direct-call-{index:03d}',
            'input': {'args': retail_call['args'],
                      'initial_state': {'first_argument_stack_word': retail_call['stackEffect']['before']}},
            'categories': ['boundary' if index < 11 else 'nominal'],
            'reference_output': {
                'return_value': int(retail_call['result']),
                'state_delta': {'first_argument_stack_word': retail_call['stackEffect']['after']},
                'side_effects': [],
            },
            'candidate_output': {
                'return_value': int(candidate_call['result']),
                'state_delta': {'first_argument_stack_word': candidate_call['stackEffect']['after']},
                'side_effects': [],
            },
        })
    failures = sum(case['reference_output'] != case['candidate_output'] for case in behavioral_cases)
    report.update({
        'result': 'passed' if failures == 0 else 'failed',
        'executed': True,
        'oracle': 'retail-binary-execution',
        'method': 'Frida direct calls in sequential retail and current Windows x86 candidate Level 1 processes; identical six-scalar inputs; captured EAX and the first caller-stack word after return.',
        'observable_fields': ['return_value', 'state_delta', 'side_effects'],
        'float_tolerance': {'absolute': 0, 'relative': 0},
        'cases': behavioral_cases,
        'tests_total': len(behavioral_cases),
        'tests_passed': len(behavioral_cases) - failures,
        'tests_failed': failures,
        'runtime': {
            'retail': {key: report['retail'].get(key) for key in ('executable_sha256', 'arguments', 'cwd', 'escape', 'probe')},
            'candidate': {key: report['candidate'].get(key) for key in ('executable_sha256', 'arguments', 'cwd', 'probe')},
            'candidate_function_va': candidate_va,
            'frida_client_version': frida.__version__,
            'probe_sha256': hashlib.sha256(probe_bytes).hexdigest(),
        },
        'comparison': {'calls_total': len(input_cases), 'calls_matched': len(input_cases) - len(mismatches),
                       'mismatches': mismatches},
        'runner': {
            'path': pathlib.Path(__file__).resolve().relative_to(ROOT).as_posix(),
            'sha256': sha256(pathlib.Path(__file__).resolve()),
            'command': [str(pathlib.Path(__file__).resolve()), *sys.argv[1:]],
        },
    })
    serialized = json.dumps(report, indent=2, sort_keys=True) + '\n'
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(serialized, encoding='utf-8')
    print(serialized, end='')
    return 0 if failures == 0 else 1


if __name__ == '__main__':
    raise SystemExit(main())
