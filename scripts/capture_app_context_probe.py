#!/usr/bin/env python3
"""Sequential same-input diagnostic calls to seven retail AppContext accessors."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
from threading import Timer
import time
import frida
from capture_world_checkpoint import ROOT, GAME, WIN_GAME, symbols
from native_diff_runner import NATIVES, GETTERS
from verification_scope import stage_candidate, address

FIELDS = ('debugged', 'debug_text_state', 'lightmaps_used', 'terrain_lightmaps_used',
          'debug_keys_enabled', 'fixme_small')
GLOBALS = dict(zip(FIELDS, ('0x005C8BF8', '0x005C8BF4', '0x005C8BE4', '0x005C8BE8',
                           '0x0057B194', '0x005C8E00')))
VALUES = (0, 1, -1, 127, 128, -128, 0x12345678, -0x12345678,
          0x7fffffff, -0x80000000, 0x55555555, -0x55555556)


def digest(path):
    hasher = hashlib.sha256()
    with path.open('rb') as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b''):
            hasher.update(chunk)
    return hasher.hexdigest()


def bounded(call, *arguments, seconds=45, **keywords):
    cancellation = frida.Cancellable()
    timer = Timer(seconds, cancellation.cancel)
    timer.start()
    try:
        with cancellation:
            return call(*arguments, **keywords)
    finally:
        timer.cancel()


def fixtures():
    result = []
    for setter, records in ((True, NATIVES), (False, GETTERS)):
        for address, (name, field, _) in records.items():
            cases = []
            for index, value in enumerate(range(256) if setter else VALUES):
                state = {key: VALUES[(index + offset) % len(VALUES)] for offset, key in enumerate(FIELDS)}
                state[field] = 0 if setter else value
                item = dict(initial_state=state)
                if setter:
                    item.update(raw_byte=value, signed_arg=value if value < 128 else value - 256)
                cases.append(item)
            result.append(dict(address=address, name=name, field=field, setter=setter, fixtures=cases))
    return result


def resources(level):
    paths = [GAME / 'config.qvm', GAME / 'humanplayer/humanplayer.qvm', GAME / 'YMBE.AFP']
    level_root = GAME / 'MISSIONS' / 'location0' / f'LEVEL{level}'
    # AFP identity binds packed level assets; retain loose script hashes when present.
    if not level_root.is_dir():
        raise ValueError('Requested level resource directory is missing: ' + str(level_root))
    paths += sorted(level_root.rglob('*'))
    return {str(path.relative_to(GAME)): digest(path) for path in paths if path.is_file()}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--candidate', type=Path, required=True)
    parser.add_argument('--host', default=os.environ.get('FRIDA_HOST', '127.0.0.1:27042'))
    parser.add_argument('--level', type=int, required=True)
    parser.add_argument('--seconds', type=int, default=3)
    parser.add_argument('--address', action='append', type=address,
                        help='Exact AppContext native filter; repeatable; unknown addresses rejected')
    args = parser.parse_args()
    if not 1 <= args.level <= 14 or not 1 <= args.seconds <= 30:
        parser.error('level must be 1..14 and seconds 1..30')
    candidate = args.candidate.resolve()
    names = symbols(candidate)
    calls = fixtures()
    if args.address:
        requested = set(args.address)
        unknown = requested - {item['address'] for item in calls}
        if unknown:
            parser.error('Unknown AppContext addresses: ' + ', '.join(sorted(unknown)))
        calls = [item for item in calls if item['address'] in requested]
    candidate_target = dict(fields=FIELDS, state_accessor=names['igi::retail::core::AppContext_Get()'],
                            natives=[dict(item, entry=names['igi::retail::core::' + item['name'] +
                                      ('(signed char)' if item['setter'] else '()')]) for item in calls])
    retail_target = dict(fields=FIELDS, globals=GLOBALS,
                         natives=[dict(item, entry=item['address']) for item in calls])
    source_path = ROOT / 'scripts/frida_app_context_probe.js'
    source_bytes = source_path.read_bytes()
    source = source_bytes.decode('utf-8')
    conditions = resources(args.level)
    if not {'config.qvm', 'humanplayer/humanplayer.qvm', 'YMBE.AFP'} <= conditions.keys():
        raise ValueError('Required resource conditions are missing')
    manifest_path = ROOT / 'docs/fidelity/native-verification.json'
    manifest = json.loads(manifest_path.read_text())
    required = {item['address'] for item in calls}
    records = [item for item in manifest['natives'] if item.get('address') in required]
    if len(records) != len(required) or {item['address'] for item in records} != required:
        raise ValueError('Manifest must uniquely identify each selected AppContext native')
    expected_images = {item.get('binary_sha256') for item in records}
    expected_images.add(manifest.get('binary_sha256'))
    hashes = {'retail': digest(GAME / 'igi.exe'), 'candidate': digest(candidate)}
    if expected_images != {hashes['retail']}:
        raise ValueError('Retail image does not match selected address-bound manifest records')
    sources = {relative: digest(ROOT / relative) for relative in (
        'engine/src/retail/core/app_context.cpp',
        'engine/include/igi/retail/core/app_context.hpp')}
    manifest_sha256 = digest(manifest_path)
    def unchanged():
        return (resources(args.level) == conditions and digest(manifest_path) == manifest_sha256 and
                all(digest(ROOT / path) == value for path, value in sources.items()))
    cancellation = frida.Cancellable()
    timer = Timer(10, cancellation.cancel)
    timer.start()
    try:
        with cancellation:
            device = frida.get_device_manager().add_remote_device(args.host)
            existing = [(p.pid, p.name) for p in bounded(device.enumerate_processes, seconds=10)
                        if p.name.lower() == 'igi.exe' or p.name.lower().startswith('open-igi')]
    finally:
        timer.cancel()
    if existing:
        raise RuntimeError('Pre-existing games are not owned by this capture: ' + repr(existing))
    folder = ROOT / 'build/runtime-traces' / ('app-context-level' + str(args.level) + '-' +
        datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
    folder.mkdir(parents=True)
    (folder / 'probe.js').write_bytes(source_bytes)
    report = dict(schema_version=1, result='inconclusive', level=args.level,
                  scope='Selected AppContext functions; explicit agent-thread fixtures, not feature acceptance',
                  selected_addresses=[item['address'] for item in calls],
                  executable_hashes=hashes, probe_sha256=hashlib.sha256(source_bytes).hexdigest(),
                  driver_sha256=digest(Path(__file__)), runner_sha256=digest(ROOT / 'scripts/native_diff_runner.py'),
                  resources=conditions, source_hashes=sources, manifest_sha256=manifest_sha256, sides={})

    def persist():
        temporary = folder / 'comparison.json.tmp'
        temporary.write_text(json.dumps(report, indent=2) + '\n')
        temporary.replace(folder / 'comparison.json')
    persist()

    def capture(side, executable, target):
        pid = session = None
        spawn_attempted = False
        errors = []
        result = dict(binary_sha256=digest(executable), command=['window', f'level{args.level}'],
                      cwd=WIN_GAME, target=target, result='failed', stage='preflight')
        report['sides'][side] = result
        def progress(stage):
            result['stage'] = stage
            result['stage_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
            (folder / (side + '.json')).write_text(json.dumps(result, indent=2) + '\n')
            persist()
        progress('preflight')
        try:
            if not unchanged() or digest(executable) != hashes[side]:
                raise ValueError('Resource or executable identity changed before capture')
            existing = [(p.pid, p.name) for p in bounded(device.enumerate_processes, seconds=10)
                        if p.name.lower() == 'igi.exe' or p.name.lower().startswith('open-igi')]
            if existing:
                raise RuntimeError('Game appeared before capture; no ownership: ' + repr(existing))
            spawn_attempted = True
            progress('spawn-requested-ownership-pending')
            pid = bounded(device.spawn, ['Z:' + str(executable).replace('/', '\\'), 'window', f'level{args.level}'], cwd=WIN_GAME)
            result['pid'] = pid
            result['executable'] = str(executable)
            progress('spawned-owned-pid')
            session = bounded(device.attach, pid)
            progress('attached')
            script = session.create_script(source)
            def message(value, data):
                with (folder / (side + '-messages.jsonl')).open('a') as stream:
                    stream.write(json.dumps(dict(utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                                                message=value)) + '\n')
                    stream.flush()
                if value.get('type') == 'error':
                    errors.append(value)
            script.on('message', message)
            bounded(script.load)
            progress('script-loaded')
            result['observed_cwd'] = bounded(script.exports_sync.cwd, seconds=10)
            if result['observed_cwd'].lower().rstrip('\\') != WIN_GAME.lower():
                raise ValueError('Observed game working directory differs')
            progress('fixtures-running')
            result['fixtures'] = dict(cases=[], globals_restored=True,
                                      scope='Explicit agent-thread fixtures; six-field memory effects and EAX return only')
            result['completed_native_addresses'] = []
            for native in target['natives']:
                result['active_native_address'] = native['address']
                result['fixtures']['globals_restored'] = False
                progress('native-fixtures-running')
                batch = bounded(script.exports_sync.run,
                                dict(target, natives=[native], module=executable.name), seconds=45)
                if batch.get('globals_restored') is not True or len(batch.get('cases', [])) != len(native['fixtures']):
                    raise ValueError('Incomplete fixture batch or globals not restored: ' + native['address'])
                if batch.get('scope') != result['fixtures']['scope']:
                    raise ValueError('Fixture scope changed across batches')
                result['fixtures']['globals_restored'] = True
                result['fixtures']['cases'].extend(batch['cases'])
                result['completed_native_addresses'].append(native['address'])
                progress('native-fixtures-completed-globals-restored')
            result.pop('active_native_address', None)
            progress('fixtures-completed-globals-restored')
            bounded(device.resume, pid)
            progress('natural-observation')
            time.sleep(args.seconds)
            result['observation'] = bounded(script.exports_sync.snapshot, seconds=10)
            progress('observation-captured')
            if not unchanged() or digest(executable) != hashes[side]:
                raise ValueError('Resource or executable identity changed during capture')
            result['result'] = 'failed' if errors else 'captured'
        except Exception as error:
            result['error'] = str(error)
            result['failure_stage'] = result['stage']
            progress('failed-before-cleanup')
        finally:
            if pid is not None:
                try:
                    bounded(device.kill, pid, seconds=10)
                except frida.ProcessNotFoundError:
                    pass
                except Exception as error:
                    result.update(result='failed', cleanup_error=str(error))
            if session is not None:
                try:
                    bounded(session.detach, seconds=10)
                except frida.InvalidOperationError:
                    pass
                except Exception as error:
                    result.update(result='failed', detach_error=str(error))
            result['script_errors'] = errors
            report['sides'][side] = result
            unresolved = spawn_attempted and pid is None
            if unresolved:
                result.update(result='failed', cleanup_ownership='unresolved: spawn returned no PID; no process was killed')
            elif pid is not None:
                try:
                    if any(p.pid == pid for p in bounded(device.enumerate_processes, seconds=10)):
                        result.update(result='failed', cleanup_error='Owned game process remains alive')
                        unresolved = True
                except Exception as error:
                    result.update(result='failed', cleanup_error='Cannot confirm owned process exit: ' + str(error))
                    unresolved = True
            progress('cleanup-ownership-unresolved' if unresolved or result.get('cleanup_error') else 'cleanup-finished')

    try:
        capture('retail', GAME / 'igi.exe', retail_target)
        if report['sides']['retail']['result'] == 'captured':
            with stage_candidate(candidate, GAME) as staged:
                capture('candidate', staged, candidate_target)
        sides = report['sides']
        expected = [(item['address'], fixture) for item in calls for fixture in item['fixtures']]
        same = (len(sides) == 2 and all(side['result'] == 'captured' for side in sides.values()) and
                sides['retail']['fixtures'] == sides['candidate']['fixtures'] and
                sides['retail']['fixtures']['globals_restored'] is True and
                [(case['address'], case['input']) for case in sides['retail']['fixtures']['cases']] == expected and
                all(case['observed_initial_state'] == case['input']['initial_state']
                    for case in sides['retail']['fixtures']['cases']))
        report['result'] = 'passed' if same else 'failed'
        report['cases'] = len(expected)
    finally:
        report['limitations'] = ['Explicit calls precede startup; natural call counts are separate observations.',
                                'Only six named state fields and EAX are compared; unrelated memory is outside this probe.',
                                'Fixture parity does not establish natural integration or whole-game equivalence.']
        persist()
    print(folder, report['result'], flush=True)
    return int(report['result'] != 'passed')


if __name__ == '__main__':
    raise SystemExit(main())
