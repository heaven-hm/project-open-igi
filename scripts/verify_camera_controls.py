#!/usr/bin/env python3
"""Exercise the owned diagnostic window; no whole-game acceptance attestation."""
import datetime
import hashlib
import json
from pathlib import Path
import struct
import subprocess
import time
import frida
from threading import Timer

ROOT=Path(__file__).resolve().parents[1]
GAME=Path('/Users/haseeb-mir/Downloads/IGI')

def main():
    folder=ROOT/'build/runtime-traces'/('camera-controls-'+datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%SZ'))
    folder.mkdir(parents=True)
    executable=GAME/'open-igi.exe'
    report=dict(scope='Owned diagnostic window, relative mouse and developer controls; gameplay parity unverified',
                executable_sha256=hashlib.sha256(executable.read_bytes()).hexdigest(),checkpoints={})
    cancel=frida.Cancellable();timer=Timer(10,cancel.cancel);timer.start()
    try:
        with cancel:
            device=frida.get_device_manager().add_remote_device('127.0.0.1:27043')
            games=[(p.pid,p.name) for p in device.enumerate_processes() if p.name.lower() in ('igi.exe','open-igi.exe')]
            if games:
                raise RuntimeError('An existing game session is not owned by this test: '+repr(games))
    finally:timer.cancel()
    symbols=subprocess.check_output(['i686-w64-mingw32-nm','-C','--defined-only',str(executable)],text=True)
    entry=next('0x'+line.split()[0] for line in symbols.splitlines() if ' T igi::retail::render::Transcontext_BuildViewBlock(' in line)
    log=GAME/'open-igi-model-scene.log';offset=log.stat().st_size if log.exists() else 0
    pid=None;session=None;script=None;errors=[]
    try:
        pid=device.spawn(['Z:'+str(executable).replace('/','\\'),'window','level1','devmode'],cwd='Z:'+str(GAME).replace('/','\\'))
        session=device.attach(pid)
        source=(ROOT/'scripts/frida_camera_controls.js').read_text();(folder/'probe.js').write_text(source)
        report['probe_sha256']=hashlib.sha256(source.encode()).hexdigest()
        script=session.create_script(source)
        def message(msg,data):
            if msg.get('type')=='error':errors.append(msg)
            if msg.get('payload',{}).get('kind')=='frame':
                frame=msg['payload'];size=54+len(data)
                header=struct.pack('<2sIHHI',b'BM',size,0,0,54)+struct.pack('<IiiHHIIiiII',40,frame['width'],-frame['height'],1,32,0,len(data),0,0,0,0)
                (folder/'frame.bmp').write_bytes(header+data)
        script.on('message',message);script.load();script.exports_sync.start(entry);device.resume(pid)
        def wait_frame(previous=0):
            for _ in range(100):
                snapshot=script.exports_sync.snapshot()
                if snapshot['frames']>previous:return snapshot
                time.sleep(.2)
            raise RuntimeError('No new rendered camera frame')
        report['checkpoints']['player_initial']=wait_frame()
        for name,dx in [('look_right',1600),('look_farther_right',1600),('look_left',-1600)]:
            before=script.exports_sync.snapshot();script.exports_sync.mouse(dx,0);time.sleep(.6)
            after=wait_frame(before['frames']);report['checkpoints'][name]=after
            if after['latest']['matrix']==before['latest']['matrix']:raise RuntimeError(name+' did not change the view')
        script.exports_sync.key(0x78,False);script.exports_sync.key(0x78,True);script.exports_sync.key(0x78,False)
        saved=script.exports_sync.snapshot();script.exports_sync.key(0x7b,False);time.sleep(.4)
        script.exports_sync.mouse(200,80);time.sleep(.6)
        report['checkpoints']['free_camera']=script.exports_sync.snapshot()
        script.exports_sync.key(0x7b,False);time.sleep(.6)
        restored=script.exports_sync.snapshot();report['checkpoints']['player_restored']=restored
        if restored['latest']['matrix']!=saved['latest']['matrix']:raise RuntimeError('F12 did not restore the player view')
        script.exports_sync.capture();time.sleep(.3)
        script.exports_sync.key(0x79,False)
        for _ in range(100):
            with log.open('rb') as stream:stream.seek(offset);text=stream.read().decode(errors='replace')
            if 'Developer loaded next level: level2' in text:break
            time.sleep(.2)
        else:raise RuntimeError('F10 did not load level2')
        report['checkpoints']['level2']=wait_frame(restored['frames'])
        report['result']='passed' if not errors else 'failed'
    except Exception as error:
        report.update(result='failed',error=str(error))
    finally:
        if pid is not None:
            try:device.kill(pid)
            except frida.ProcessNotFoundError:pass
        if session:
            try:session.detach()
            except frida.InvalidOperationError:pass
        if log.exists():
            with log.open('rb') as stream:stream.seek(offset);(folder/'scene.log').write_bytes(stream.read())
        report['script_errors']=errors;(folder/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(folder,report['result'],report.get('error',''),flush=True)
    return int(report['result']!='passed')

if __name__=='__main__':raise SystemExit(main())
