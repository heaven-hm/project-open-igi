"""Start/stop a private, hash-checked Frida server for one sequential test."""
from contextlib import contextmanager
import hashlib
import json
import os
from pathlib import Path
import re
import signal
import socket
import subprocess
import time

SERVER_SHA256='d4b0b8af908b13c673d9e40b805b446182b051a61b4f62f519793ddaaa651d3b'


def server_processes(windows,host):
    """Identify the exact private Wine service, excluding its launcher."""
    listing=subprocess.run(['ps','-axo','pid=,command='],capture_output=True,text=True,
                           check=True,timeout=10).stdout
    expression=re.compile(re.escape(windows)+r'\s+-l\s+'+re.escape(host)+r'\s*',re.I)
    found={}
    for line in listing.splitlines():
        parts=line.strip().split(None,1)
        if len(parts)==2 and parts[0].isdigit() and expression.fullmatch(parts[1]):
            found[int(parts[0])]=parts[1]
    return found


def stop_owned_server(windows,host,owned):
    # Wine detaches the Windows process into a separate process group. Killing
    # the Popen launcher alone does not terminate this server. Recheck identity
    # before each signal and never kill pre-existing or differently named PIDs.
    for sig in (signal.SIGTERM,signal.SIGKILL):
        current=server_processes(windows,host)
        for pid,command in owned.items():
            if current.get(pid)==command:
                try:os.kill(pid,sig)
                except ProcessLookupError:pass
        deadline=time.monotonic()+3
        while time.monotonic()<deadline:
            current=server_processes(windows,host)
            if not any(current.get(pid)==command for pid,command in owned.items()):return
            time.sleep(.1)
    raise RuntimeError('owned private Frida server did not terminate')


@contextmanager
def runtime_endpoint(cfg,folder):
    wine=Path(cfg.get('verification_wine','/Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine'))
    server=Path(cfg.get('verification_frida_server','/private/tmp/igi-frida-server-17.24.0-windows-x86.exe'))
    if not wine.is_file() or not server.is_file():
        raise ValueError('configured verifier Wine/Frida server missing')
    if hashlib.sha256(server.read_bytes()).hexdigest()!=SERVER_SHA256:
        raise ValueError('Frida server image hash mismatch')
    with socket.socket() as listener:
        listener.bind(('127.0.0.1',0))
        port=listener.getsockname()[1]
    host=f'127.0.0.1:{port}'
    folder.mkdir(parents=True,exist_ok=True)
    windows='Z:\\'+str(server.resolve())[1:].replace('/','\\')
    previous=server_processes(windows,host)
    owned={}
    with (folder/'frida-server.log').open('w') as log:
        process=subprocess.Popen([str(wine),'--bottle',cfg.get('verification_bottle','open-igi'),
                                  windows,'-l',host],stdout=log,stderr=subprocess.STDOUT)
        identity=dict(kind='igi-private-frida-service',host=host,owned_pid=process.pid,
                      server_path=str(server),server_sha256=SERVER_SHA256,bottle=cfg.get('verification_bottle','open-igi'),
                      rpc_health_passed=False,terminated=False)
        try:
            # A successful TCP bind is only readiness for the actual Frida RPC
            # health check below; file/listener presence is never tool health.
            deadline=time.monotonic()+15
            while True:
                if process.poll() is not None:raise ValueError('private Frida server exited; see frida-server.log')
                try:
                    with socket.create_connection(('127.0.0.1',port),timeout=.5):break
                except OSError:
                    if time.monotonic()>=deadline:raise TimeoutError('private Frida server did not listen')
                    time.sleep(.2)
            code='import frida,json;d=frida.get_device_manager().add_remote_device('+repr(host)+');print(json.dumps([p.name for p in d.enumerate_processes()]))'
            health=subprocess.run([cfg['frida_python'],'-c',code],capture_output=True,text=True,timeout=15)
            (folder/'frida-health.log').write_text(health.stdout+health.stderr)
            if health.returncode:raise ValueError('private Frida RPC health failed')
            owned={pid:command for pid,command in server_processes(windows,host).items() if pid not in previous}
            if len(owned)!=1:raise ValueError('private Frida server process ownership is ambiguous')
            identity['owned_server_pids']=sorted(owned)
            identity['rpc_health_passed']=True
            yield host
        finally:
            try:
                # Also discover an owned child if readiness failed before RPC.
                if not owned:
                    owned={pid:command for pid,command in server_processes(windows,host).items() if pid not in previous}
                try:
                    stop_owned_server(windows,host,owned)
                finally:
                    process.terminate()
                    try:process.wait(timeout=5)
                    except subprocess.TimeoutExpired:process.kill();process.wait()
                identity['owned_server_pids']=sorted(owned)
                with socket.socket() as check:
                    check.settimeout(.5)
                    identity['listener_closed']=check.connect_ex(('127.0.0.1',port))!=0
                identity['terminated']=process.poll() is not None and identity['listener_closed']
                if not identity['terminated']:raise RuntimeError('private Frida listener remains after cleanup')
            except Exception as error:
                identity['cleanup_error']=str(error)
                raise
            finally:
                (folder/'frida-service.json').write_text(json.dumps(identity,indent=2)+'\n')
