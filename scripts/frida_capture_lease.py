"""Coordinate owned sequential runtime captures; never terminate existing games."""
from contextlib import contextmanager
import fcntl
import hashlib
import os
from pathlib import Path
import tempfile
import time


@contextmanager
def capture_lease(device, process_names, *, timeout=60, directory=None):
    """Hold a host-specific local lease across both sides of a paired capture.

    Existing callers without this lease remain protected by process preflight.
    This coordinates cooperating local drivers, not other machines/Frida clients.
    """
    host = os.environ.get('FRIDA_HOST', '127.0.0.1:27042')
    key = hashlib.sha256(host.rsplit(':', 1)[0].encode()).hexdigest()[:20]
    folder = Path(directory or tempfile.gettempdir())
    deadline = time.monotonic() + timeout
    with (folder / ('igi-frida-capture-' + key + '.lock')).open('a') as stream:
        while True:
            try:
                fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
                break
            except BlockingIOError:
                if time.monotonic() >= deadline:
                    raise TimeoutError('another paired capture owns the runtime lease')
                time.sleep(0.2)
        try:
            while process_names(device):
                if time.monotonic() >= deadline:
                    raise TimeoutError('pre-existing game remains active; no process terminated')
                time.sleep(0.5)
            yield
        finally:
            fcntl.flock(stream, fcntl.LOCK_UN)
