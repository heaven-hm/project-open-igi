#!/usr/bin/env python3
"""Capture task-4 evidence sequentially before startup; never assert parity."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time
import datetime
from threading import Timer
import frida
from verification_scope import stage_candidate

ROOT = Path(__file__).resolve().parents[1]
GAME = Path('/Users/haseeb-mir/Downloads/IGI')
WIN_GAME = 'Z:' + str(GAME).replace('/', '\\')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def symbols(path):
    result = subprocess.run(['i686-w64-mingw32-nm', '-C', '--defined-only', str(path)],
                            capture_output=True, text=True, check=True)
    names = {}
    for line in result.stdout.splitlines():
        parts = line.split(None, 2)
        if len(parts) == 3:
            names[parts[2]] = '0x' + parts[0]
    return names


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--level', type=int, required=True)
    parser.add_argument('--seconds', type=int, default=12)
    parser.add_argument('--host', default=os.environ.get('FRIDA_HOST', '127.0.0.1:27042'))
    parser.add_argument('--pure-fpu-cases', action='store_true',
                        help='Also explicitly invoke the two reviewed pure scalar conversion natives.')
    parser.add_argument('--lightmap-cases', action='store_true',
                        help='Run reviewed lightmap fixtures before resume, restoring native globals afterwards.')
    parser.add_argument('--candidate', type=Path, default=ROOT/'build-win32/engine/platform/windows/open-igi.exe')
    args = parser.parse_args()
    if not 1 <= args.level <= 14 or not 1 <= args.seconds <= 60:
        raise ValueError('level must be 1..14 and duration 1..60 seconds')
    cancellation = frida.Cancellable()
    timer = Timer(10, cancellation.cancel)
    timer.start()
    try:
        with cancellation:
            device = frida.get_device_manager().add_remote_device(args.host)
            existing = [(p.pid, p.name) for p in device.enumerate_processes()
                        if p.name.lower() == 'igi.exe' or p.name.lower().startswith('open-igi')]
    finally:
        timer.cancel()
    if existing:
        raise RuntimeError('Existing game sessions are not owned by this capture: ' + repr(existing))
    probe_bytes = (ROOT/'scripts/frida_world_checkpoint_probe.js').read_bytes()
    source = probe_bytes.decode('utf-8') + '''
rpc.exports.cwd = function() {
  const buffer=Memory.alloc(4096);
  const fn=new NativeFunction(Process.getModuleByName('kernel32.dll').getExportByName('GetCurrentDirectoryW'),
                              'uint32',['uint32','pointer'],'stdcall');
  const length=fn(2048,buffer);
  if(length===0 || length>=2048)throw new Error('GetCurrentDirectoryW failed');
  return buffer.readUtf16String(length);
};
'''
    folder = ROOT/'build/runtime-traces'/('world-level'+str(args.level)+'-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
    folder.mkdir(parents=True)
    (folder/'probe.js').write_text(source)
    summary = dict(level=args.level, frida_host=args.host, probe_sha256=hashlib.sha256(source.encode()).hexdigest(),
                   result='inconclusive', limitations=['Diagnostic world/native capture, not a release acceptance report.',
                   'Object identities are opaque; model/material/texture reconciliation remains required.',
                   'Instrumentation overhead is excluded from frame-rate claims.'], sides={})

    def capture(side, binary, config):
        pid = None
        session = None
        script = None
        report = dict(binary=str(binary), sha256=digest(binary), config=config,
                      command=['window',f'level{args.level}'], snapshots=[], script_errors=[])
        scene_log = GAME/'open-igi-model-scene.log'
        scene_offset = scene_log.stat().st_size if scene_log.exists() else 0
        try:
            pid = device.spawn(['Z:'+str(binary).replace('/','\\'), 'window', f'level{args.level}'], cwd=WIN_GAME)
            report['pid'] = pid
            session = device.attach(pid)
            script = session.create_script(source)
            def record_message(message, data):
                if message.get('type') == 'error':
                    report['script_errors'].append(message)
                with (folder/(side+'-messages.jsonl')).open('a') as stream:
                    stream.write(json.dumps(message)+'\n')
            script.on('message', record_message)
            script.load()
            report['observed_cwd'] = script.exports_sync.cwd()
            if report['observed_cwd'].lower().rstrip('\\') != WIN_GAME.lower():
                raise RuntimeError('Observed game working directory differs')
            report['hooks_before_resume'] = script.exports_sync.start(dict(config,module=binary.name))
            if config.get('pureScalarCases'):
                report['pure_scalar_cases'] = script.exports_sync.run_pure_scalar_cases(config['pureScalarCases'])
            if config.get('lightmapCases'):
                report['lightmap_cases'] = script.exports_sync.run_lightmap_cases(config['lightmapCases'])
            device.resume(pid)
            escape_delivered = False
            for _ in range(args.seconds):
                time.sleep(1)
                snapshot = script.exports_sync.snapshot()
                keyboard_ready = (not config.get('escapeObservation') or
                                  (snapshot['counts'].get('Keyboard_Update', 0) > 0 and
                                   (snapshot['counts'].get('LevelFlow_GetBreakCutSceneKey', 0) > 0 or
                                    snapshot['counts'].get('Terrain_Draw', 0) > 0)))
                if not escape_delivered and keyboard_ready:
                    attempt = script.exports_sync.skip_cutscene()
                    report.setdefault('escape_attempts', []).append(attempt)
                    escape_delivered = attempt['delivered']
                    snapshot = script.exports_sync.snapshot()
                report['snapshots'].append(snapshot)
                (folder/(side+'.json')).write_text(json.dumps(report,indent=2))
            latest = report['snapshots'][-1]
            report['escape_observation'] = dict(
                native_keyboard_frames=latest['escapeObservedFrames'],
                native_cutscene_pressed_returns=latest['breakCutscenePressedReturns'],
                supported=bool(config.get('escapeObservation')),
                cutscene_transition='unverified')
            report['result'] = ('failed' if latest['failures'] or latest['overflow'] or report['script_errors'] else
                                'observed' if latest['counts'] else 'unobserved')
        except Exception as error:
            report['result'] = 'failed'
            report['error'] = str(error)
        finally:
            if script is not None:
                try: report['escape_cleanup'] = script.exports_sync.release_escape()
                except (frida.InvalidOperationError, frida.ProcessNotFoundError): pass
                except Exception as error:
                    report['result'] = 'failed'
                    report['escape_cleanup_error'] = str(error)
            if pid is not None:
                try: device.kill(pid)
                except frida.ProcessNotFoundError: pass
            if session is not None:
                try: session.detach()
                except frida.InvalidOperationError: pass
            if side == 'candidate' and scene_log.exists():
                with scene_log.open('rb') as stream:
                    # A truncated/replaced log must not include stale prior-run bytes.
                    if scene_log.stat().st_size >= scene_offset:
                        stream.seek(scene_offset)
                    captured_log = stream.read()
                log_path = folder/'candidate-scene.log'
                log_path.write_bytes(captured_log)
                report['scene_log'] = dict(path=log_path.name, sha256=digest(log_path),
                                           initial_offset=scene_offset)
            (folder/(side+'.json')).write_text(json.dumps(report,indent=2))
            if pid is not None and any(p.pid == pid for p in device.enumerate_processes()):
                raise RuntimeError('Owned game is still running; refusing next capture')
        summary['sides'][side] = dict(result=report['result'],sha256=report['sha256'],error=report.get('error'))
        latest = report['snapshots'][-1] if report['snapshots'] else {}
        print(side,report['result'],'counts=',latest.get('counts'), 'objects=',len(latest.get('objectPositions',[])),flush=True)

    retail_config = dict(taskRegistration='0x004F1090',
                         fpuInitialization=dict(entry='0x00498460',controlWord='0x006E5BC4'),
                         rigidModelIdentity=dict(constructor='0x004F2930',virtualModels='0x00A443A0',virtualModelCount='0x00A4EDEC'),
                         escapeObservation=dict(keyState='0x00C28C63',keyboardUpdate='0x00490230',breakCutscene='0x00488E00'),
                         heightmap=dict(entry='0x00506F00',count='0x00B97DF4'),worldObjectCommit='0x004C6940',
                         entries=[dict(name=name,address=address) for name,address in (
                             ('Terrain_Load','0x004C6240'),('Terrain_LoadTexturesAndLightmaps','0x004C5F00'),
                             ('Terrain_TraverseContourQuadtree','0x004D14E0'),
                             ('Terrain_DecodeChunkCommands','0x00510010'),('Terrain_Draw','0x00512C30'))])
    pure_targets = []
    if args.pure_fpu_cases:
        for address, symbol in [('0x004AC66B', 'Camera_ApplyBandFilter_24(unsigned short)'),
                                ('0x004AC6FD', 'Camera_ApplyWideScreen_24(unsigned int)')]:
            evidence = json.loads((ROOT/'docs/fidelity/evidence/fpu-state-20261020'/
                                   (address+'-replay.json')).read_text())
            pure_targets.append(dict(retailAddress=address, entry=address,
                                     inputs=[case['input']['arguments'][0] for case in evidence['cases']],
                                     symbol='igi::retail::core::'+symbol))
        retail_config['pureScalarCases'] = pure_targets
    if args.lightmap_cases:
        replay = json.loads((ROOT/'docs/fidelity/evidence/lightmap-enter-20261020/after.json').read_text())
        retail_config['lightmapCases'] = dict(
            entry='0x004F82B0', modeEntry='0x004F1160', count='0x00B97D24',
            stack='0x00B97B20', bd='0x00A758BD', be='0x00A758BE',
            fixtures=[case['input'] for case in replay['cases']])
    if digest(GAME/'igi.exe') != 'dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':
        raise ValueError('Retail image differs from the statically analyzed image')
    capture('retail',GAME/'igi.exe',retail_config)
    names = symbols(args.candidate)
    candidate_config = dict(entries=[],heightmap=dict(
        entry=names['igi::retail::terrain::HeightMap_Enter(void*)'],
        count=names['igi::retail::terrain::(anonymous namespace)::g_b97df4']))
    if pure_targets:
        candidate_config['pureScalarCases'] = [dict(target, entry=names[target['symbol']])
                                               for target in pure_targets]
    if args.lightmap_cases:
        candidate_config['lightmapCases'] = dict(
            entry=names['igi::retail::terrain::TerrainLightMap_Init_2(void*)'],
            modeEntry=names['igi::retail::script::Script_ParseEnumReal32_5()'],
            count=names['igi::retail::terrain::g_b97d24'],
            stack=names['igi::retail::terrain::g_data_b97b20'],
            bd=names['igi::retail::script::g_a758bd'], be=names['igi::retail::script::g_a758be'],
            fixtures=retail_config['lightmapCases']['fixtures'])
    if 'igi::retail::core::Display_ClearSurfaces_16()' in names:
        candidate_config['fpuInitialization'] = dict(
            entry=names['igi::retail::core::Display_ClearSurfaces_16()'],
            controlWord=names['igi::retail::core::g_6e5bc4'])
    summary['candidate_world_commit_symbols'] = [name for name in names if 'WorldObject_CommitTransform(' in name]
    summary['candidate_missing_natives'] = ([] if summary['candidate_world_commit_symbols'] else
                                           ['WorldObject_CommitTransform (no engine implementation symbol)'])
    with stage_candidate(args.candidate.resolve(),GAME) as staged:
        capture('candidate',staged,candidate_config)
    scalar_mismatch = False
    lightmap_mismatch = False
    if args.lightmap_cases:
        sides = [json.loads((folder/(side+'.json')).read_text()) for side in ('retail', 'candidate')]
        sequences = [side.get('lightmap_cases', {}) for side in sides]
        expected_inputs = retail_config['lightmapCases']['fixtures']
        lightmap_mismatch = (not expected_inputs or sequences[0] != sequences[1] or
                             any(not sequence.get('globals_restored') or
                                 [case.get('input') for case in sequence.get('cases', [])] != expected_inputs
                                 for sequence in sequences))
        summary['lightmap_comparison'] = dict(
            result='failed' if lightmap_mismatch else 'passed', cases=len(expected_inputs),
            scope='Explicit valid-context fixtures; globals restored before game resume; natural terrain integration unverified')
    if pure_targets:
        sides = [json.loads((folder/(side+'.json')).read_text()) for side in ('retail', 'candidate')]
        sequences = [side.get('pure_scalar_cases', []) for side in sides]
        reference = {item['retailAddress']: item['cases'] for item in sequences[0]}
        candidate = {item['retailAddress']: item['cases'] for item in sequences[1]}
        expected = {target['retailAddress'] for target in pure_targets}
        scalar_mismatch = set(reference) != expected or set(candidate) != expected or reference != candidate
        for sequence in sequences:
            if len(sequence) != len(expected):
                scalar_mismatch = True
            for target in pure_targets:
                matches = [item for item in sequence if item['retailAddress'] == target['retailAddress']]
                if (not target['inputs'] or len(matches) != 1 or
                        [case.get('input') for case in matches[0]['cases']] != target['inputs']):
                    scalar_mismatch = True
        summary['pure_scalar_comparison'] = dict(
            result='failed' if scalar_mismatch else 'passed',
            cases=sum(len(cases) for cases in reference.values()),
            scope='Two explicit pure scalar native calls on Frida agent threads; no whole-game parity claim')
    (folder/'comparison.json').write_text(json.dumps(summary,indent=2))
    print('Saved',folder,flush=True)
    return int(scalar_mismatch or lightmap_mismatch or any(side['result'] == 'failed' for side in summary['sides'].values()))


if __name__ == '__main__':
    raise SystemExit(main())
