#!/usr/bin/env python3
"""Headless Frida plumbing check of the owned diagnostic PE32, before resume.

No game main method executes; this is not native behavior or scene evidence.
"""
import hashlib
import json
import os
from pathlib import Path
import sys
from candidate_build_receipt import validate_receipt

ROOT=Path(__file__).resolve().parents[1]


def main():
    config=json.loads((ROOT/'scripts/igi-workflow.json').read_text())
    try: import frida
    except ImportError:
        python=config['frida_python']
        if Path(python).resolve()==Path(sys.executable).resolve(): raise
        os.execv(python,[python,str(Path(__file__).resolve())])
    candidate=ROOT/config['candidate']; receipt=ROOT/config['build_directory']/'candidate-receipt.json'
    output=ROOT/'build/frida-shell-smoke.json'
    report=dict(kind='igi-diagnostic-frida-smoke',schema_version=1,result='blocked',native_verified=False,
                scope='owned diagnostic process, spawn/attach/script before resume; no game code executed')
    pid=None; session=None; script=None; device=None
    try:
        validate_receipt(receipt,ROOT,candidate)
        report['candidate_sha256']=hashlib.sha256(candidate.read_bytes()).hexdigest()
        device=frida.get_device_manager().add_remote_device(os.environ.get('FRIDA_HOST',config['frida_host']))
        to_windows=lambda path:'Z:\\'+str(path.resolve())[1:].replace('/','\\')
        pid=device.spawn([to_windows(candidate)],cwd=to_windows(Path(config['retail_exe']).parent))
        report['owned_pid']=pid
        session=device.attach(pid)
        probe="rpc.exports = { inspect(name) { const m = Process.findModuleByName(name); return { arch: Process.arch, pointerSize: Process.pointerSize, module: m === null ? null : m.name }; } };"
        report['probe_sha256']=hashlib.sha256(probe.encode()).hexdigest()
        script=session.create_script(probe);script.load()
        observed=script.exports_sync.inspect(candidate.name);report['observed']=observed
        if observed!=dict(arch='ia32',pointerSize=4,module=candidate.name): raise ValueError('unexpected candidate architecture/module')
        validate_receipt(receipt,ROOT,candidate)
        report['result']='passed'
    except Exception as error:
        report['error']=repr(error)
    finally:
        if pid is not None:
            try:
                device.kill(pid)
                report['owned_process_terminated']=pid not in {p.pid for p in device.enumerate_processes()}
                if not report['owned_process_terminated']: report['result']='blocked'
            except Exception as error:
                report.update(result='blocked',cleanup_error=repr(error))
        if session:
            try: session.detach()
            except Exception: pass
        output.parent.mkdir(parents=True,exist_ok=True);output.write_text(json.dumps(report,indent=2)+'\n')
        print(f"{report['result']}: {output}; native_verified=False")
    return 0 if report['result']=='passed' else 1


if __name__=='__main__':raise SystemExit(main())
