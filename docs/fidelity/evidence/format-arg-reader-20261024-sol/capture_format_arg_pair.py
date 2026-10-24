#!/usr/bin/env python3
"""Compare naturally captured retail argument-reader inputs with compiled replay.

Diagnostic only: the candidate native runs on Frida's RPC thread before startup.
This cannot authorize natural game-call-flow or production native acceptance.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import signal
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'scripts'))
from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
import frida


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def calls(report):
    entries, returns = report.get('entries', []), report.get('returns', [])
    expected = list(range(1, len(entries) + 1))
    if not entries or [e.get('callIndex') for e in entries] != expected or \
            sorted(e.get('callIndex') for e in returns) != expected:
        raise ValueError('empty, duplicated or incomplete native call sequence')
    by_index = {e['callIndex']: e for e in returns}
    return [dict(inputWord=e['inputWord'], cursorCellOverlapsValue=e['cursorCellOverlapsValue'],
                 resultWord=by_index[e['callIndex']]['resultWord'],
                 cursorAdvance=by_index[e['callIndex']]['cursorAdvance'],
                 originalWordAfter=by_index[e['callIndex']]['originalWordAfter']) for e in entries]


def require_complete(report, mode, count=None):
    terminal = [e for e in report.get('events', []) if e.get('type') == 'arg_reader_complete']
    if (report.get('result') != 'observed' or report.get('failures') or
            report.get('cleanup', {}).get('owned_process_exited') is not True or
            len(terminal) != 1 or terminal[0].get('mode') != mode or
            terminal[0].get('pending') != 0 or terminal[0].get('failures') or
            terminal[0].get('count') != len(report.get('entries', [])) or
            terminal[0].get('returned') != len(report.get('returns', [])) or
            (count is not None and (terminal[0].get('count') != count or
                                   terminal[0].get('replayCount') != count))):
        raise ValueError('capture lacks a complete typed native terminal marker')
    return calls(report)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--dll-replay', action='store_true',
                        help='Run the compiled import-free DLL in a second retail host; never game-flow acceptance')
    args = parser.parse_args()
    args.out = args.out.resolve()
    args.out.mkdir(parents=True, exist_ok=False)
    retail = Path('/Users/haseeb-mir/Downloads/IGI/igi.exe')
    probe = ROOT / 'scripts/tester/frida_format_arg_reader.js'
    source = ROOT / 'build/native-analysis/format-arg-reader-sol/004A7EFF-Native_004A7EFF.cpp'
    header = source.with_suffix('.hpp')
    driver = ROOT / 'scripts/tester/frida_probe_driver.py'
    build = ROOT / 'build-format-arg-candidate-sol'
    candidate = build / 'igi1.exe'
    receipt = build / 'diagnostic-receipt.json'
    identities = {str(p): digest(p) for p in [retail, probe, source, header, driver, Path(__file__)]}
    if identities[str(retail)] != 'dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':
        raise ValueError('wrong retail image')
    device = frida.get_device_manager().add_remote_device('127.0.0.1:27042')
    names = lambda dev: [p.name for p in dev.enumerate_processes()
                         if p.name.lower() in ('igi.exe', 'igi1.exe', 'open-igi.exe') or
                         p.name.lower().startswith('open-igi-probe-')]

    def capture(side, image, va, extra=()):
        command = [sys.executable, str(driver), '--exe', str(image), '--probe', str(probe),
                   '--target', hex(va), '--native-size', '13', '--arg-words', '1',
                   '--name', 'Native_004A7EFF', '--observe', '15' if side == 'retail' else '2',
                   '--max-calls', '65536', '--level', '1', '--expect-exe-sha256', digest(image),
                   '--out', str(args.out / (side + '.json')), *extra]
        process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        timed_out = False
        try:
            stdout, stderr = process.communicate(timeout=90)
        except subprocess.TimeoutExpired:
            # SIGINT gives the driver's finally block a chance to terminate its
            # own suspended process and persist the ownership/cleanup report.
            timed_out = True
            journal = args.out / (side + '.owned.json')
            if journal.is_file():
                owned = json.loads(journal.read_text())
                if (owned.get('kind') != 'igi-owned-capture-start' or
                        owned.get('executable', {}).get('sha256') != digest(image) or
                        Path(owned['executable']['path']).resolve() != image.resolve()):
                    raise RuntimeError('timeout ownership journal does not bind this image')
                matching = [p for p in device.enumerate_processes() if p.pid == owned['pid']]
                if matching:
                    if len(matching) != 1 or matching[0].name.lower() != image.name.lower():
                        raise RuntimeError('timeout PID ownership is ambiguous; no process killed')
                    device.kill(owned['pid'])
            process.send_signal(signal.SIGINT)
            try:
                stdout, stderr = process.communicate(timeout=25)
            except subprocess.TimeoutExpired:
                process.kill()
                stdout, stderr = process.communicate()
        (args.out / (side + '-driver.log')).write_text(stdout + stderr)
        if timed_out or process.returncode or names(device):
            raise RuntimeError(side + ' capture/cleanup failed')
        return json.loads((args.out / (side + '.json')).read_text())

    with capture_lease(device, names, timeout=2):
        retail_report = capture('retail', retail, 0x4a7eff)
        observed = require_complete(retail_report, 'natural-retail')
        if any(row['cursorCellOverlapsValue'] is not False for row in observed):
            raise ValueError('observed overlapping inputs cannot be silently excluded from replay')
        input_path = args.out / 'retail-inputs.json'
        input_path.write_text(json.dumps(dict(kind='igi-retail-arg-reader-inputs',
            retailVa='0x004A7EFF', retailReportSha256=digest(args.out/'retail.json'),
            cases=[{k: row[k] for k in ('inputWord', 'cursorCellOverlapsValue')} for row in observed]),
            indent=2) + '\n')
        # Build AFTER retail; other agents' earlier source changes cannot make
        # this an old installed candidate. Strict freshness remains enabled.
        if args.dll_replay:
            candidate = args.out/'candidate.dll'
            receipt = args.out/'dll-build.json'
            command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared', '-nostdlib',
                       '-Wl,--entry,0,--export-all-symbols', str(source), '-o', str(candidate)]
        else:
            command = [sys.executable, str(ROOT/'scripts/candidate_build_receipt.py'), '--root', str(ROOT),
                       '--build-dir', str(build), '--receipt', str(receipt)]
        with (args.out/'build.log').open('w') as log:
            result = subprocess.run(command, stdout=log, stderr=subprocess.STDOUT, timeout=180)
        if result.returncode:
            raise RuntimeError('clean diagnostic candidate build failed')
        if args.dll_replay:
            imports = subprocess.check_output(['i686-w64-mingw32-objdump', '-p', str(candidate)], text=True)
            if 'DLL Name:' in imports or 'AddressOfEntryPoint\t00000000' not in imports:
                raise ValueError('test DLL must have no imports or entry callback')
            receipt.write_text(json.dumps(dict(kind='diagnostic-native-dll-build', verified=False,
                command=command, source_sha256=digest(source), header_sha256=digest(header),
                candidate_sha256=digest(candidate), compiler_version=subprocess.check_output(
                    ['i686-w64-mingw32-g++','--version'], text=True),
                imports=imports, limits=['No game executable or production intake is certified.']), indent=2)+'\n')
            inputs = json.loads(input_path.read_text())
            inputs['compiledDllPath'] = 'Z:\\' + str(candidate)[1:].replace('/', '\\')
            code = subprocess.check_output(['r2','-q','-2','-c','p8 13 @ 0x4a7eff; q',str(retail)], text=True).strip()
            if len(code) != 26:
                raise ValueError('retail byte capture is incomplete')
            inputs['expectedCodeHex'] = code
            input_path.write_text(json.dumps(inputs,indent=2)+'\n')
        else:
            validate_receipt(receipt, ROOT, candidate, required_sources=[source])
        identities.update({str(candidate): digest(candidate), str(receipt): digest(receipt),
                           str(input_path): digest(input_path)})
        symbols = subprocess.check_output(['i686-w64-mingw32-nm', str(candidate)], text=True)
        entries = [int(p[0], 16) for line in symbols.splitlines()
                   if len(p := line.split()) == 3 and p[2] == '_Native_004A7EFF' and p[1] == 'T']
        if len(entries) != 1:
            raise ValueError('candidate native symbol absent or ambiguous')
        candidate_report = capture('candidate', retail if args.dll_replay else candidate,
                                   0x4a7eff if args.dll_replay else entries[0],
                                   ['--probe-input-json', str(input_path), '--stream-probe-cases'] +
                                   (['--diagnostic-resume-before-hook'] if args.dll_replay else []))
        replayed = require_complete(candidate_report, 'compiled-dll-replay-in-retail-host' if args.dll_replay
                                    else 'retail-input-replay-before-startup', len(observed))
        if not args.dll_replay:
            validate_receipt(receipt, ROOT, candidate, required_sources=[source])
        for path, value in identities.items():
            if digest(path) != value:
                raise ValueError('capture/build input changed: ' + path)
    mismatch = [i + 1 for i, (a, b) in enumerate(zip(observed, replayed)) if a != b]
    semantic_checks = all(row['cursorAdvance'] == 4 and row['inputWord'] == row['resultWord'] ==
                          row['originalWordAfter'] for row in observed + replayed)
    report = dict(kind='igi-format-arg-reader-recorded-input-replay', verified=False,
                  natural_candidate_flow=False, retail_va='0x004A7EFF',
                  candidate_preferred_va=hex(entries[0]),
                  candidate_va=candidate_report['hooks'][0]['target'],
                  identities=identities, calls=len(observed), mismatched_calls=mismatch,
                  observations_match=not mismatch and semantic_checks,
                  semantic_checks=semantic_checks,
                  scope=('All naturally observed retail inputs replayed on the compiled x86 DLL in a second retail host'
                         if args.dll_replay else
                         'All naturally observed retail calls replayed on the compiled x86 native before candidate startup'),
                  replay_host='retail' if args.dll_replay else 'candidate-executable',
                  limits=['Replay uses owned memory and the RPC thread, not the candidate formatter call graph.',
                          'No aliasing, invalid-pointer or concurrency claim follows from these disjoint recorded inputs.',
                          'No production intake, reviewed runtime contract or full-game acceptance.'])
    (args.out/'comparison.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return 0 if report['observations_match'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
