#!/usr/bin/env python3
"""Same reviewed modifier fixtures in sequential retail/candidate processes."""
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
from verification_scope import stage_candidate


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--candidate',type=Path,required=True)
    p.add_argument('--host',default=os.environ.get('FRIDA_HOST','127.0.0.1:27042'))
    p.add_argument('--level',type=int,required=True)
    p.add_argument('--fixtures',type=Path,required=True)
    p.add_argument('--seconds',type=int,default=3)
    a=p.parse_args()
    if not 1<=a.level<=14 or not 1<=a.seconds<=30:raise ValueError('Invalid capture limits')
    replay=json.loads(a.fixtures.read_text())
    if replay['address']!='0x00505E40' or replay['result']!='passed':raise ValueError('A passing reviewed replay is required')
    if digest(GAME/'igi.exe')!=replay['binary_sha256']:raise ValueError('Retail binary differs from replay')
    if digest(ROOT/'engine/src/retail/terrain/terrain_loaders.cpp')!=replay['implementation_sha256']:raise ValueError('Source changed since fixture replay')
    fixtures=[case['input'] for case in replay['cases'] if case['reference_output']['return_value']['kind']!='fatal-loop']
    if not fixtures:raise ValueError('No valid nonfatal fixtures')
    cancellation=frida.Cancellable();timer=Timer(10,cancellation.cancel);timer.start()
    try:
        with cancellation:
            device=frida.get_device_manager().add_remote_device(a.host)
            existing=[(x.pid,x.name) for x in device.enumerate_processes() if x.name.lower() in ('igi.exe','open-igi.exe')]
    finally:timer.cancel()
    if existing:raise RuntimeError('Existing games not owned by capture: '+repr(existing))
    folder=ROOT/'build/runtime-traces'/('modifier-level'+str(a.level)+'-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
    folder.mkdir(parents=True)
    source=(ROOT/'scripts/frida_texture_modifier_probe.js').read_text();(folder/'probe.js').write_text(source)
    report=dict(scope='Explicit same-input modifier native calls; natural reachability reported separately',level=a.level,
                probe_sha256=hashlib.sha256(source.encode()).hexdigest(),driver_sha256=digest(Path(__file__)),
                fixtures_sha256=digest(a.fixtures),sides={},result='inconclusive')
    for side in ('retail','candidate'):
        binary=GAME/'igi.exe' if side=='retail' else a.candidate.resolve()
        if side=='retail':target=dict(entry='0x00505E40',count='0x00B97DF8',stack='0x00B97DA0',mask='0x00BA5060')
        else:
            names=symbols(binary)
            target={key:names['igi::retail::terrain::'+name] for key,name in
                    [('entry','TextureModifier_Push(void*)'),('count','g_b97df8'),('stack','g_b97da0'),('mask','g_ba5060')]}
        def capture(executable):
            pid=None;session=None;errors=[]
            result=dict(binary_sha256=digest(executable),command=['window',f'level{a.level}'],cwd=WIN_GAME,target=target)
            try:
                pid=device.spawn(['Z:'+str(executable).replace('/','\\'),'window',f'level{a.level}'],cwd=WIN_GAME)
                session=device.attach(pid);script=session.create_script(source)
                def on_message(message,data):
                    if message.get('type')=='error':errors.append(message)
                script.on('message',on_message);script.load()
                result['fixtures']=script.exports_sync.run(dict(target,module=executable.name,fixtures=fixtures))
                device.resume(pid);time.sleep(a.seconds)
                result['observation']=script.exports_sync.snapshot();result['result']='failed' if errors else 'captured'
            except Exception as error:result.update(result='failed',error=str(error))
            finally:
                if pid is not None:
                    try:device.kill(pid)
                    except frida.ProcessNotFoundError:pass
                if session:
                    try:session.detach()
                    except frida.InvalidOperationError:pass
                result['script_errors']=errors;report['sides'][side]=result
                (folder/(side+'.json')).write_text(json.dumps(result,indent=2)+'\n')
                if pid is not None and any(x.pid==pid for x in device.enumerate_processes()):raise RuntimeError('Owned process is still running')
        if side=='retail':capture(binary)
        else:
            with stage_candidate(binary,GAME) as staged:capture(staged)
        if report['sides'][side]['result']=='failed':break
    sides=report['sides'];same=(len(sides)==2 and all(x['result']=='captured' for x in sides.values()) and
      sides['retail']['fixtures']==sides['candidate']['fixtures'] and
      [c['input'] for c in sides['retail']['fixtures']['cases']]==fixtures)
    report.update(result='passed' if same else 'failed',cases=len(fixtures),
                  limitations=['Agent-thread fixture parity does not establish natural integration, rendering, or whole-game equivalence.',
                               'Fatal paths and arbitrary aliases are excluded from live invocation.'])
    (folder/'comparison.json').write_text(json.dumps(report,indent=2)+'\n')
    print(folder,report['result'],flush=True)
    return int(not same)

if __name__=='__main__':raise SystemExit(main())
