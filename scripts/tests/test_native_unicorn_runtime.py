import pathlib
import sys
import unittest
from subprocess import CompletedProcess
from tempfile import TemporaryDirectory
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import native_unicorn_runner as runner  # noqa: E402


class UnicornRuntimeTests(unittest.TestCase):
    def test_mem_map_sigill_uses_configured_compatible_interpreter(self):
        with TemporaryDirectory() as temporary:
            interpreter = pathlib.Path(temporary) / "python-x86_64"
            interpreter.write_text("#!/bin/sh\nexit 0\n")
            interpreter.chmod(0o755)
            calls = []

            def fake_probe(args, **_kwargs):
                calls.append(args[0])
                code = -4 if len(calls) == 1 else 0
                return CompletedProcess(args, code, "", "")

            class ExecCalled(Exception):
                pass

            with patch.dict("os.environ", {"IGI_UNICORN_PYTHON": str(interpreter)}), \
                    patch.object(runner.subprocess, "run", side_effect=fake_probe), \
                    patch.object(runner.os, "execv", side_effect=ExecCalled) as execv:
                with self.assertRaises(ExecCalled):
                    runner.ensure_emulator_runtime()

            self.assertEqual(calls, [sys.executable, str(interpreter)])
            execv.assert_called_once_with(str(interpreter), [str(interpreter), *sys.argv])

    def test_preflight_failure_identifies_sigill_and_rosetta_remedy(self):
        with patch.object(
            runner.subprocess,
            "run",
            return_value=CompletedProcess([], -4, "", ""),
        ), patch.dict("os.environ", {"IGI_UNICORN_PYTHON": ""}):
            with self.assertRaisesRegex(SystemExit, "x86_64 Python runtime"):
                runner.ensure_emulator_runtime()


if __name__ == "__main__":
    unittest.main()
