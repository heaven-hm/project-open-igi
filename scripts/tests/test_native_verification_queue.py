import importlib.util
import json
from pathlib import Path
import tempfile
from types import SimpleNamespace
from unittest import mock
import unittest

SPEC = importlib.util.spec_from_file_location("native_verification_queue", Path(__file__).parents[1] / "native_verification_queue.py")
queue = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(queue)


class QueueTests(unittest.TestCase):
    def test_exact_grouping_and_engine_priority_preserve_duplicates(self):
        report = {"checked_natives": [
            {"path": "scripts/GameNatives/a.cpp", "address": "0x00400000", "errors": []},
            {"path": "engine/a.cpp", "address": "0x00400000", "errors": ["bad"]},
            {"path": "engine/a.cpp", "address": "0x00400000", "errors": ["bad"]}],
            "failures": ["0x00400000: failed", "global failure"]}
        result = queue.group_report(report)
        self.assertEqual(result["checked_record_count"], 3)
        self.assertEqual(result["group_count"], 2)
        self.assertEqual(result["groups"][0]["path"], "engine/a.cpp")
        self.assertEqual(len(result["groups"][0]["records"]), 2)
        self.assertEqual(result["unassigned_failures"], ["global failure"])

    def test_uncovered_sources_are_not_hidden_by_address_filter(self):
        report = {"checked_natives": [], "failures": [
            "engine/missing.cpp: no address-bound native identity", 
            "scripts/GameNatives/b.cpp:1 0x00401234: missing record"]}
        result = queue.group_report(report)
        self.assertEqual(result["group_count"], 2)
        self.assertIsNone(result["groups"][0]["address"])
        filtered = queue.group_report(report, {"0x00401234"})
        self.assertEqual(filtered["group_count"], 2)
        self.assertEqual(filtered["selected_group_count"], 1)
        self.assertEqual(filtered["groups"][0]["address"], "0x00401234")
        partial = queue.group_report(report, {"0x00401234", "0x00405678"})
        self.assertEqual(partial["unmatched_addresses"], ["0x00405678"])

    def test_fingerprint_invalidates_inputs_and_inventory(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for folder in ("engine", "scripts", "docs/fidelity", "assets"):
                (root / folder).mkdir(parents=True)
            binary, manifest, report = [root / value for value in ("igi.exe", "manifest.json", "report.json")]
            for path in (binary, manifest, report):
                path.write_text("fixture")
            first = queue.fingerprint(root, binary, manifest, report)
            cpp = root / "engine/a.cpp"
            cpp.write_text("changed")
            self.assertNotEqual(first, queue.fingerprint(root, binary, manifest, report))
            second = queue.fingerprint(root, binary, manifest, report)
            cpp.write_text("changed again")
            self.assertNotEqual(second, queue.fingerprint(root, binary, manifest, report))
            queue.atomic_json(root / "nested/checkpoint.json", {"value": 1})
            self.assertEqual(json.loads((root / "nested/checkpoint.json").read_text()), {"value": 1})


    @mock.patch.object(queue, "tool_identities", return_value={"fixture": {"sha256": "fixture"}})
    def test_strict_subprocess_and_exact_resume(self, _identities):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "scripts").mkdir()
            (root / "engine").mkdir()
            source = root / "engine/a.cpp"
            source.write_text("native fixture")
            binary, manifest, report = [root / value for value in ("igi.exe", "manifest.json", "report.json")]
            binary.write_bytes(b"retail fixture")
            manifest.write_text("{}")
            report.write_text("{}")
            gate = root / "scripts/native_gate.py"
            gate.write_text("import json, pathlib, sys\n"
                            "a=sys.argv\n"
                            "assert a[a.index('--threshold')+1]=='VERIFIED'\n"
                            "assert a[a.index('--cfg-threshold')+1]=='0.85'\n"
                            "assert '--require-semantic-evidence' in a and '--require-behavioral-tests' in a\n"
                            "pathlib.Path(a[a.index('--report')+1]).write_text(json.dumps({'result':'blocked'}))\n"
                            "raise SystemExit(1)\n")
            output = root / "build/queue"
            output.mkdir(parents=True)
            args = SimpleNamespace(root=root, binary=binary, manifest=manifest, output=output,
                                   resume=True, timeout=5, report_data={"binary_sha256":queue.digest(binary)})
            data = {"scope":"diagnostic", "unassigned_failures":[],
                    "groups":[{"path":"engine/a.cpp", "address":"0x00401234"}]}
            self.assertEqual(queue.run_queue(args, data, report), 1)
            checkpoint = json.loads((output / "checkpoint.json").read_text())
            saved = checkpoint["runs"]["engine/a.cpp:0x00401234"]
            self.assertTrue(saved["completed"])
            first = Path(saved["log"]).stat().st_mtime_ns
            self.assertEqual(queue.run_queue(args, data, report), 1)
            self.assertEqual(first, Path(saved["log"]).stat().st_mtime_ns)
            source.write_text("changed native fixture")
            self.assertEqual(queue.run_queue(args, data, report), 1)
            self.assertNotEqual(first, Path(saved["log"]).stat().st_mtime_ns)


    def test_tool_replacement_and_compiler_flags_invalidate_fingerprint(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            paths = [root / name for name in ("igi.exe", "manifest.json", "report.json")]
            for path in paths:
                path.write_text("fixture")
            tool = root / "compiler"
            tool.write_text("first binary")
            tool.chmod(0o755)
            with mock.patch.dict("os.environ", {"CXX":str(tool), "CXXFLAGS":"-O1"}):
                self.assertEqual(queue.tool_identities(root)["CXX"]["sha256"], queue.digest(tool))
                first = queue.fingerprint(root, *paths)
                tool.write_text("replacement binary")
                second = queue.fingerprint(root, *paths)
                self.assertNotEqual(first, second)
                with mock.patch.dict("os.environ", {"CXXFLAGS":"-O2"}):
                    self.assertNotEqual(second, queue.fingerprint(root, *paths))
                with mock.patch.dict("os.environ", {"CXX":str(root / "missing")}):
                    self.assertIsNone(queue.tool_identities(root)["CXX"]["resolved"])


if __name__ == "__main__":
    unittest.main()
