"""Start/stop a private, hash-checked Frida server for one sequential test."""
from contextlib import contextmanager
import hashlib
import json
from pathlib import Path
import socket
import subprocess
import time

SERVER_SHA256='d4b0b8af908b13c673d9e40b805b446182b051a61b4f62f519793ddaaa651d3b'


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
            identity['rpc_health_passed']=True
            yield host
        finally:
            process.terminate()
            try:process.wait(timeout=5)
            except subprocess.TimeoutExpired:process.kill();process.wait()
            identity['terminated']=process.poll() is not None
            (folder/'frida-service.json').write_text(json.dumps(identity,indent=2)+'\n')
