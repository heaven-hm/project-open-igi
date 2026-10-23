#!/usr/bin/env python3
"""Discovery and honest entry points for the clean retail-flow worktree."""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / 'docs/fidelity/native-verification.json'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='command', required=True)
    doc = sub.add_parser('doctor', help='Report paths; --check runs actual tool checks and saves raw results.')
    doc.add_argument('--check', action='store_true')
    doc.add_argument('--report', type=Path, default=ROOT / 'build/tool-health.json')
    analysis = sub.add_parser('analyze', help='Capture Binary Ninja, radare2 CFG and angr for one retail address; not acceptance.')
    analysis.add_argument('address')
    analysis.add_argument('--report', type=Path)
    sub.add_parser('smoke', help='Headless Frida plumbing check of the diagnostic candidate before resume; no game/native verification.')
    sub.add_parser('self-check', help='Exercise fail-closed report validation; infrastructure only.')
    intake = sub.add_parser('intake', help='Strict new-native intake: static gate + paired runtime + typed comparison; accept only on full pass, reject otherwise.')
    intake.add_argument('address')
    intake.add_argument('--probe', type=Path, required=True, help='Reviewed typed probe covering the complete changed-native scope')
    intake.add_argument('--level', type=int, default=1, choices=range(1,15))
    intake.add_argument('--report', type=Path, default=ROOT / 'build/native-intake' / 'report.json')
    native = sub.add_parser('native', help='Show historical evidence for one retail address.')
    native.add_argument('address', help='Retail virtual address, e.g. 0x00405850')
    gate = sub.add_parser('static', help='Run the strict native gate for one address.')
    gate.add_argument('address')
    full = sub.add_parser('verify', help='Run the existing strict per-native workflow (may require adaptation).')
    full.add_argument('address')
    full.add_argument('--probe', type=Path, help='Reviewed typed probe for paired capture')
    full.add_argument('--level', type=int, default=1, choices=range(1, 15))
    args = parser.parse_args()
    if args.command == 'smoke':
        return subprocess.run([sys.executable, str(ROOT / 'scripts/frida_shell_smoke.py')], cwd=ROOT, check=False).returncode
    if args.command == 'self-check':
        return subprocess.run([sys.executable, str(ROOT / 'scripts/workflow_self_check.py')], cwd=ROOT, check=False).returncode
    if args.command == 'intake':
        try:
            intake_address = int(args.address, 0)
            if not 0 <= intake_address <= 0xffffffff:
                raise ValueError('address out of x86 range')
        except ValueError:
            parser.error(f'invalid address: {args.address}')
        intake_addr = f'0x{intake_address:08X}'
        from workflow_tools import config
        settings = config()
        environment = dict(os.environ, LC_ALL='C', LANG='C')
        environment.setdefault('IGI_ANGR_PYTHON', settings['angr_python'])
        environment.setdefault('FRIDA_HOST', settings['frida_host'])
        run_command = [sys.executable, str(ROOT / 'scripts/smart_end_to_end_verification.py'),
                       '--address', intake_addr, '--level', str(args.level), '--probe', str(args.probe),
                       '--report', str(args.report)]
        completed = subprocess.run(run_command, cwd=ROOT, env=environment, capture_output=True, text=True)
        accepted = completed.returncode == 0
        document = {}
        try:
            if args.report.is_file():
                document = json.loads(args.report.read_text())
        except (OSError, ValueError):
            document = {}
        accepted = bool(accepted and isinstance(document, dict) and document.get('result') == 'passed' and document.get('verified') is True)
        args.report.parent.mkdir(parents=True, exist_ok=True)
        args.report.write_text(json.dumps(document, indent=2) + '\n')
        print('True' if accepted else 'False')
        print(f"{'ACCEPTED' if accepted else 'REJECTED'}: native {intake_addr} static+runtime intake")
        print(f"report: {args.report}")
        return 0 if accepted else 1
    if args.command == 'doctor' and args.check:
        from workflow_tools import doctor
        return doctor(args.report)
    if args.command == 'doctor':
        paths = {'retail': Path('/Users/haseeb-mir/Downloads/IGI/igi.exe'),
                 'GameNatives': ROOT / 'scripts/GameNatives',
                 'historical manifest': MANIFEST,
                 'historical v4 catalog': ROOT / 'assets/IGINatives_v4.json',
                 'angr interpreter': Path(os.environ.get('IGI_ANGR_PYTHON', str(ROOT / '.venv-angr/bin/python'))),
                 'RE-Agent': Path('/Users/haseeb-mir/Documents/Code/re-agent')}
        for name, path in paths.items():
            print(f'{name}: {path} ({"present" if path.exists() else "missing"})')
        for name, command in [('radare2', 'r2'), ('Frida', 'frida'), ('Windows x86 compiler', 'i686-w64-mingw32-g++')]:
            print(f'{name}: {shutil.which(command) or "not on PATH"}')
        print('MCP providers: availability is session-specific; check their actual connection separately.')
        return 0
    try:
        address = int(args.address, 0)
        if not 0 <= address <= 0xffffffff:
            raise ValueError('address out of x86 range')
    except ValueError as exc:
        parser.error(f'invalid address: {exc}')
    addr = f'0x{address:08X}'
    if args.command == 'analyze':
        from workflow_tools import analyze
        return analyze(addr, args.report or ROOT / 'build/native-analysis' / addr / 'capture.json')
    if args.command == 'native':
        records = json.loads(MANIFEST.read_text()).get('natives', [])
        matches = [r for r in records if int(str(r.get('address', '0')), 16) == address]
        print('Historical records only: verify source, hashes, scope and runtime evidence before reuse.')
        print(json.dumps(matches, indent=2))
        print('GameNatives candidates:')
        for path in sorted((ROOT / 'scripts/GameNatives').rglob(f'{address:08X}*')):
            print(path.relative_to(ROOT))
        return 0 if matches else 1
    records = json.loads(MANIFEST.read_text()).get('natives', [])
    matches = [r for r in records if int(str(r.get('address', '0')), 16) == address]
    implementation = matches[0].get('implementation', {}).get('path', '') if len(matches) == 1 else ''
    candidate = (ROOT / implementation).resolve()
    if not implementation.startswith('engine/') or not candidate.is_file() or not candidate.is_relative_to(ROOT / 'engine'):
        print(f'BLOCKED: {addr} has no mapped implementation in this clean branch; historical records cannot verify it.', file=sys.stderr)
        return 1
    if args.command == 'static':
        cmd = [sys.executable, str(ROOT / 'scripts/native_gate.py'), '--root', str(ROOT), '--address', addr, '--file', str(implementation),
               '--threshold', 'VERIFIED', '--cfg-threshold', '0.85', '--require-semantic-evidence', '--require-behavioral-tests']
    else:
        cmd = [sys.executable, str(ROOT / 'scripts/smart_end_to_end_verification.py'), '--address', addr, '--level', str(args.level)]
    if args.command == 'verify' and args.probe:
        cmd += ['--probe', str(args.probe)]
    from workflow_tools import config
    settings = config()
    environment = dict(os.environ)
    environment.setdefault('IGI_ANGR_PYTHON', settings['angr_python'])
    environment.setdefault('FRIDA_HOST', settings['frida_host'])
    print('Running existing verifier; missing evidence or incompatible legacy paths must block acceptance.', flush=True)
    return subprocess.run(cmd, cwd=ROOT, env=environment, check=False).returncode


if __name__ == '__main__':
    raise SystemExit(main())
