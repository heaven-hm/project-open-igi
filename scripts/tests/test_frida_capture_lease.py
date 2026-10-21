from pathlib import Path
import sys
import tempfile
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from frida_capture_lease import capture_lease


class CaptureLeaseTests(unittest.TestCase):
    def test_same_host_serializes_and_releases_after_failure(self):
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(ValueError, 'fixture failure'):
                with capture_lease(None, lambda _: [], directory=folder, timeout=0):
                    with self.assertRaisesRegex(TimeoutError, 'owns the runtime lease'):
                        with capture_lease(None, lambda _: [], directory=folder, timeout=0):
                            self.fail('overlapping capture acquired lease')
                    raise ValueError('fixture failure')
            with capture_lease(None, lambda _: [], directory=folder, timeout=0):
                pass

    def test_existing_game_is_never_terminated(self):
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(TimeoutError, 'no process terminated'):
                with capture_lease(object(), lambda _: [('igi.exe', 42)], directory=folder, timeout=0):
                    self.fail('existing game was ignored')
            with capture_lease(None, lambda _: [], directory=folder, timeout=0):
                pass

    def test_process_query_failure_releases_lease(self):
        def unavailable(_):
            raise RuntimeError('server unavailable')
        with tempfile.TemporaryDirectory() as folder:
            with self.assertRaisesRegex(RuntimeError, 'server unavailable'):
                with capture_lease(None, unavailable, directory=folder, timeout=0):
                    pass
            with capture_lease(None, lambda _: [], directory=folder, timeout=0):
                pass


if __name__ == '__main__':
    unittest.main()
