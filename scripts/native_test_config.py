"""One configuration for the autonomous verifier; legacy build outputs stay separate."""
import json
import os
from pathlib import Path


def load_config(root):
    cfg=json.loads((Path(root)/'scripts/igi-workflow.json').read_text())
    cfg['build_directory']=cfg.get('verification_build_directory','build-native-auto-win32')
    cfg['candidate']=cfg.get('verification_candidate',cfg['build_directory']+'/igi1.exe')
    cfg['frida_host']=os.environ.get('FRIDA_HOST',cfg.get('frida_host','127.0.0.1:27042'))
    return cfg
