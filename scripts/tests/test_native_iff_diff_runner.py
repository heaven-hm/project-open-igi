import json
import pathlib
import subprocess
import sys
import tempfile
import types
import unittest
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import native_iff_diff_runner as runner  # noqa: E402


class IffRunnerSafetyTests(unittest.TestCase):
    def test_server_health_requires_successful_enumeration(self):
        from unittest.mock import Mock
        device = Mock()
        device.enumerate_processes.side_effect = [RuntimeError("unreachable"), []]
        fake_frida = SimpleNamespace(get_device_manager=lambda: SimpleNamespace(
            add_remote_device=lambda _host: device))
        with tempfile.TemporaryDirectory() as temp:
            home = pathlib.Path(temp)
            server = home / ".cache/frida-crossover/frida-server-x86.exe"
            server.parent.mkdir(parents=True)
            server.write_bytes(b"server fixture")
            with patch.dict(sys.modules, {"frida": fake_frida}), \
                    patch.object(runner.pathlib.Path, "home", return_value=home), \
                    patch.object(runner.time, "sleep"), \
                    patch.object(runner.subprocess, "Popen") as spawn:
                runner.ensure_frida_server()
            spawn.assert_called_once()
            self.assertEqual(device.enumerate_processes.call_count, 2)

    def test_healthy_server_is_reused_without_launching_another(self):
        from unittest.mock import Mock
        device = Mock()
        device.enumerate_processes.return_value = []
        fake_frida = SimpleNamespace(get_device_manager=lambda: SimpleNamespace(
            add_remote_device=lambda _host: device))
        with patch.dict(sys.modules, {"frida": fake_frida}), \
                patch.object(runner.subprocess, "Popen") as spawn:
            runner.ensure_frida_server()
        spawn.assert_not_called()
        device.enumerate_processes.assert_called_once()

    def test_candidate_trace_must_be_an_ordered_subsequence_of_retail_trace(self):
        retail = [("BOBJ", True), ("BOBH", True), ("BOBH", False), ("BOBJ", True)]
        candidate = [("BOBJ", True), ("BOBJ", True)]
        self.assertEqual(runner.ordered_subsequence_positions(retail, candidate), [0, 3])
        self.assertIsNone(
            runner.ordered_subsequence_positions(retail, [("BOBH", False), ("BOBH", True)])
        )

    def test_process_selection_never_reuses_baseline_game_pid(self):
        device = SimpleNamespace(enumerate_processes=lambda: [
            SimpleNamespace(pid=20, name="open-igi.exe"),
            SimpleNamespace(pid=21, name="OPEN-IGI.EXE"),
        ])
        self.assertEqual(runner.find_test_process(device, "open-igi.exe", {20}).pid, 21)

    def test_process_selection_rejects_ambiguous_new_game_processes(self):
        device = SimpleNamespace(enumerate_processes=lambda: [
            SimpleNamespace(pid=21, name="open-igi.exe"),
            SimpleNamespace(pid=22, name="open-igi.exe"),
        ])
        with self.assertRaisesRegex(RuntimeError, "refusing to guess"):
            runner.find_test_process(device, "open-igi.exe", set())

    def test_runner_refuses_preexisting_game_without_spawning_or_killing_it(self):
        process = SimpleNamespace(pid=20, name="open-igi.exe")
        device = SimpleNamespace(
            enumerate_processes=lambda: [process],
            spawn=lambda *_args, **_kwargs: None,
        )
        fake_frida = SimpleNamespace(
            get_device_manager=lambda: SimpleNamespace(add_remote_device=lambda _host: device)
        )
        with patch.dict(sys.modules, {"frida": fake_frida}), patch.object(
            device, "spawn", side_effect=AssertionError("must not spawn")
        ) as spawn:
            with self.assertRaisesRegex(runner.GameAlreadyRunningError, "leaving them untouched"):
                runner.run_side("candidate", "open-igi.exe", 4, {}, pathlib.Path("candidate.exe"))
        spawn.assert_not_called()

    def test_runner_installs_probe_before_resuming_with_game_data_cwd(self):
        with tempfile.TemporaryDirectory() as temp:
            root = pathlib.Path(temp)
            game = root / "IGI"
            game.mkdir()
            (game / "igi.exe").write_bytes(b"retail fixture")
            event_order = []

            class Script:
                exports_sync = SimpleNamespace(stop=lambda: {
                    "observed": [], "escape": {"posted": True}, "hooks": [],
                })

                def on(self, _event, _callback):
                    event_order.append("message-handler")

                def load(self):
                    event_order.append("load")

            class Session:
                def create_script(self, _source):
                    return Script()

                def detach(self):
                    event_order.append("detach")

            class Device:
                def enumerate_processes(self):
                    return []

                def spawn(self, program, *, argv, cwd):
                    event_order.append("spawn")
                    self.spawn_args = (program, argv, cwd)
                    return 55

                def attach(self, pid):
                    self.attached_pid = pid
                    event_order.append("attach")
                    return Session()

                def resume(self, pid):
                    self.resumed_pid = pid
                    event_order.append("resume")

                def kill(self, pid):
                    self.killed_pid = pid
                    event_order.append("kill")

            device = Device()
            fake_frida = SimpleNamespace(
                get_device_manager=lambda: SimpleNamespace(add_remote_device=lambda _host: device)
            )
            probe = root / "probe.js"
            probe.write_text("const config = Process.argv[2];")
            with patch.dict(sys.modules, {"frida": fake_frida}), \
                    patch.object(runner, "GAME", game), \
                    patch.object(runner, "PROBE", probe), \
                    patch.object(runner, "DURATION_SECONDS", 0):
                result = runner.run_side("retail", "igi.exe", 4, {}, root / "candidate.exe")

            self.assertEqual(event_order[:4], ["spawn", "attach", "message-handler", "load"])
            self.assertEqual(event_order[4:], ["resume", "detach", "kill"])
            program, argv, cwd = device.spawn_args
            self.assertEqual(program, runner.windows_z_path(game / "igi.exe"))
            self.assertEqual(argv, [program, "window", "level4"])
            self.assertEqual(cwd, runner.windows_z_path(game))
            self.assertEqual(device.resumed_pid, 55)
            self.assertEqual(device.killed_pid, 55)
            self.assertTrue(result["escape"]["posted"])

    def test_windows_game_path_is_derived_from_current_executable(self):
        self.assertEqual(
            runner.windows_z_path(pathlib.Path("/private/tmp/build/open-igi.exe")),
            "Z:\\private\\tmp\\build\\open-igi.exe",
        )

    def test_crossover_launch_uses_a_locale_supported_by_its_wrapper(self):
        with patch.dict("os.environ", {"LC_ALL": "C.UTF-8", "LC_CTYPE": "C.UTF-8", "LANG": "C.UTF-8"}):
            environment = runner.crossover_environment()
        self.assertEqual(environment["LC_ALL"], "C")
        self.assertEqual(environment["LC_CTYPE"], "C")
        self.assertEqual(environment["LANG"], "C")

    def test_candidate_addresses_are_resolved_from_the_selected_executable(self):
        with tempfile.TemporaryDirectory() as temp:
            root = pathlib.Path(temp)
            symbol_map = root / "symbols.json"
            candidate = root / "candidate.exe"
            candidate.write_bytes(b"candidate fixture")
            entries = []
            nm_lines = []
            for index, (address, name) in enumerate(runner.FAMILY.items()):
                symbol = f"_native_{index}"
                entries.append({
                    "address": address,
                    "implemented": True,
                    "candidateSymbol": {"symbol": symbol, "vaddr": "0xDEADBEEF"},
                })
                nm_lines.append(f"0040{index + 100:04X} T {symbol}")
            symbol_map.write_text(json.dumps(entries))
            with patch.object(runner, "SYMBOL_MAP", symbol_map), patch.object(
                runner.subprocess, "run",
                return_value=SimpleNamespace(stdout="\n".join(nm_lines)),
            ) as nm:
                symbols = runner.candidate_symbols(candidate)

            self.assertEqual(nm.call_args.args[0][-1], str(candidate))
            for index, address in enumerate(runner.FAMILY):
                self.assertEqual(
                    symbols[address.lower()]["candidateSymbol"]["vaddr"],
                    f"0x0040{index + 100:04X}",
                )
            scheme = runner.build_scheme("candidate", symbols, "fresh-open-igi.exe")
            self.assertEqual(scheme["mode"], "candidate")
            self.assertTrue(all(t["module"] == "fresh-open-igi.exe" for t in scheme["targets"]))

    def test_game_cleanup_has_no_name_wide_kill(self):
        import inspect

        source = inspect.getsource(runner.run_side)
        self.assertNotIn("pkill", source)
        self.assertIn("device.kill(target_pid)", source)


if __name__ == "__main__":
    unittest.main()
