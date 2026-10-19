import contextlib
import io
import os
import pathlib
import subprocess
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import cmake_native_gate as bridge


class CMakeGateTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        (self.root / 'scripts').mkdir()
        self.binary = self.root / 'retail.exe'
        self.binary.write_bytes(b'fixture')
        for name in ('build_attestation.py', 'generate_frida_level_probe.py', 'discover_referenced_natives.py'):
            (self.root / 'scripts' / name).touch()

    def invoke(self, args, responses=()):
        argv = ['cmake_native_gate.py', '--root', str(self.root), '--binary', str(self.binary), *args]
        with mock.patch.object(sys, 'argv', argv), mock.patch.dict(os.environ, {}, clear=True), \
             mock.patch.object(bridge.subprocess, 'run', side_effect=responses) as run, \
             mock.patch.object(bridge.shutil, 'which', return_value='/fixture/r2'), \
             contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            result = bridge.main()
            return result, run.call_args_list

    def test_production_rejects_weaker_status_or_cfg_before_running_tools(self):
        for extra in (['--threshold', 'TRANSCRIBED'], ['--cfg-threshold', '0.84'],
                      ['--cfg-threshold', 'nan'], ['--cfg-threshold', 'inf']):
            with self.subTest(extra=extra), self.assertRaises(SystemExit) as caught:
                self.invoke(['--full-production', *extra])
            self.assertEqual(caught.exception.code, 2)

    def test_capture_failure_stops_production_gate(self):
        code, calls = self.invoke(['--full-production'], [subprocess.CompletedProcess([], 7)])
        self.assertEqual(code, 7)
        self.assertEqual(len(calls), 1)

    def test_probe_failure_stops_development_gate(self):
        code, calls = self.invoke(['--development'], [subprocess.CompletedProcess([], 0, '0x00401000\n', ''),
                                                   subprocess.CompletedProcess([], 8)])
        self.assertEqual(code, 8)
        self.assertEqual(len(calls), 2)

    def test_missing_required_scripts_block(self):
        (self.root / 'scripts/build_attestation.py').unlink()
        code, calls = self.invoke(['--full-production'])
        self.assertEqual(code, 1)
        self.assertEqual(calls, [])
        (self.root / 'scripts/generate_frida_level_probe.py').unlink()
        code, calls = self.invoke(['--development'], [subprocess.CompletedProcess([], 0, '0x00401000\n', '')])
        self.assertEqual(code, 1)
        self.assertEqual(len(calls), 1)

    def test_production_preserves_full_tree_and_strict_defaults(self):
        code, calls = self.invoke(['--full-production'], [subprocess.CompletedProcess([], 0),
                                                       subprocess.CompletedProcess([], 0)])
        self.assertEqual(code, 0)
        argv = calls[-1].args[0]
        self.assertIn('--production-tree', argv)
        self.assertEqual(argv[argv.index('--threshold') + 1], 'VERIFIED')
        self.assertEqual(argv[argv.index('--cfg-threshold') + 1], '0.85')


if __name__ == '__main__':
    unittest.main()
