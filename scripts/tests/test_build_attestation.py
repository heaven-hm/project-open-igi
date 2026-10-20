"""Receipt tests use synthetic PE headers/reports in isolated temporary directories."""
import importlib.util
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

HELPER = Path(__file__).resolve().parents[1] / "build_attestation.py"
spec = importlib.util.spec_from_file_location("build_attestation", HELPER)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class BuildAttestationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="igi-attestation-test-")
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for name in ["CMakeLists.txt", "build.sh", "run.sh", "engine/test.cpp",
                     "docs/fidelity/native-verification.json", "docs/fidelity/gate-integrity.sha256"]:
            p = self.root / name
            p.parent.mkdir(parents=True, exist_ok=True)
            p.write_text("fixture\n")
        self.binary = self.root / "fixture.exe"
        data = bytearray(256)
        data[:2] = b"MZ"
        struct.pack_into("<I", data, 0x3C, 0x80)
        data[0x80:0x84] = b"PE\0\0"
        struct.pack_into("<H", data, 0x84, 0x14C)
        struct.pack_into("<H", data, 0x98, 0x10B)
        self.binary.write_bytes(data)
        # A test fixture, never an authorization report for project natives.
        self.report = {"result": "passed", "mode": "production-tree", "threshold": "VERIFIED",
                       "cfg_threshold": 0.85, "failures": [], "checked_native_count": 1,
                       "semantic_review_required": True, "behavioral_tests_required": True,
                       "binary_sha256": "0" * 64,
                       "checked_natives": [{"path": "engine/test.cpp", "address": "0x00401000", "errors": [],
                           "angr": {"implementation_sha256": module.digest(self.root / "engine/test.cpp")}}]}
        self.report['checked_natives'][0]['runtime_contract'] = dict(scope='fixture', probe_sha256='a'*64,
            required_observables=dict(inputs=['value'], outputs=['result'], sideEffects=['counter']))
        self.gate = self.root / "fixture-gate.json"
        self.write_report()
        self.snapshot = self.root / "snapshot.json"
        self.inputs = self.root / "inputs.json"
        self.receipt = self.root / "receipt.json"
        compiler = self.root / "fixture-compiler"
        compiler.write_text("fixture compiler identity\n")
        cache = self.root / "build-win32/CMakeCache.txt"
        cache.parent.mkdir()
        cache.write_text("CMAKE_CXX_COMPILER:FILEPATH=" + str(compiler) + "\n")

    def write_report(self):
        self.gate.write_text(json.dumps(self.report))

    def invoke(self, action, *args):
        return subprocess.run([sys.executable, str(HELPER), action, "--root", str(self.root), *map(str, args)],
                              capture_output=True, text=True)

    def make_receipt(self):
        self.make_snapshot()
        self.assertEqual(self.invoke("configure", "--snapshot", self.snapshot, "--output", self.snapshot).returncode, 0)
        runtime = self.root / 'runtime.json'
        comparison = dict(scope='fixture',
            inputs=dict(retail=[{'value': 1}], candidate=[{'value': 1}]),
            outputs=dict(retail=[{'result': 2}], candidate=[{'result': 2}]),
            sideEffects=dict(retail=[{'counter': 3}], candidate=[{'counter': 3}]))
        runtime.write_text(json.dumps(dict(staticGateSha256=module.digest(self.gate), probeSha256='a'*64,
            binarySha256=self.report['binary_sha256'], candidateSha256=module.digest(self.binary),
            results=[dict(address='0x00401000', name='Fixture', retailCalled=True,
                candidateCalled=True, candidateReachable=True, argumentsMatch=True,
                scalarReturn=True, valueMatch=True, sideEffectsMatch=True,
                semanticComparator='a'*64, comparison=comparison)])))
        extra = ['--runtime-report', runtime]
        document = json.loads(runtime.read_text())
        state = dict(scope='fixture', inputs={'value': 1}, outputs={'result': 2}, sideEffects={'counter': 3})
        document['results'][0].update(retailCalls=[dict(typedState=state)], candidateCalls=[dict(typedState=state)])
        runtime.write_text(json.dumps(document))
        result = self.invoke("seal", "--snapshot", self.snapshot, "--binary", self.binary,
                             "--output", self.receipt, *extra)
        self.assertEqual(result.returncode, 0, result.stderr)

    def make_snapshot(self):
        self.assertEqual(self.invoke("capture", "--output", self.inputs).returncode, 0)
        self.assertEqual(self.invoke("snapshot", "--gate-report", self.gate,
                                     "--input-snapshot", self.inputs, "--output", self.snapshot).returncode, 0)

    def check(self):
        return self.invoke("check", "--receipt", self.receipt, "--binary", self.binary,
                           "--gate-report", self.gate)

    def test_unchanged_fixture_receipt_matches(self):
        self.make_receipt()
        self.assertEqual(self.check().returncode, 0)

    def test_changed_native_scope_is_preserved_in_receipt(self):
        self.report['mode'] = 'changed'
        self.report['checked_natives'][0]['address'] = '0x00401000'
        self.write_report()
        self.make_receipt()
        receipt = json.loads(self.receipt.read_text())
        self.assertEqual(receipt['verification_scope'], 'changed')
        self.assertEqual(receipt['verified_addresses'], ['0x00401000'])
        self.assertEqual(self.check().returncode, 0)

    def test_no_scope_can_seal_without_paired_runtime(self):
        for mode in ('changed', 'production-tree'):
            with self.subTest(mode=mode):
                self.report['mode'] = mode
                self.write_report()
                self.make_snapshot()
                self.assertEqual(self.invoke('configure', '--snapshot', self.snapshot,
                                             '--output', self.snapshot).returncode, 0)
                result = self.invoke('seal', '--snapshot', self.snapshot, '--binary', self.binary,
                                     '--output', self.receipt)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('paired runtime', result.stderr)

    def test_changed_runtime_report_cannot_be_replaced_after_sealing(self):
        self.report['mode'] = 'changed'
        self.report['checked_natives'][0]['address'] = '0x00401000'
        self.write_report()
        self.make_receipt()
        (self.root / 'runtime.json').write_text('{}')
        self.assertNotEqual(self.check().returncode, 0)

    def test_modified_executable_is_blocked(self):
        self.make_receipt()
        self.binary.write_bytes(self.binary.read_bytes() + b"changed")
        self.assertNotEqual(self.check().returncode, 0)

    def test_modified_source_is_blocked(self):
        self.make_receipt()
        (self.root / "engine/test.cpp").write_text("changed\n")
        self.assertNotEqual(self.check().returncode, 0)

    def test_change_during_build_is_blocked(self):
        self.make_snapshot()
        (self.root / "run.sh").write_text("changed\n")
        self.assertNotEqual(self.invoke("seal", "--snapshot", self.snapshot, "--binary", self.binary,
                                        "--output", self.receipt).returncode, 0)

    def test_blocked_report_cannot_authorize_snapshot(self):
        self.assertEqual(self.invoke("capture", "--output", self.inputs).returncode, 0)
        self.report["result"] = "blocked"
        self.write_report()
        self.assertNotEqual(self.invoke("snapshot", "--gate-report", self.gate,
                                        "--input-snapshot", self.inputs, "--output", self.snapshot).returncode, 0)
        self.assertFalse(self.snapshot.exists())

    def test_structural_gate_cannot_authorize_snapshot(self):
        self.assertEqual(self.invoke("capture", "--output", self.inputs).returncode, 0)
        self.report["threshold"] = "STRUCTURALLY_VERIFIED"
        self.write_report()
        self.assertNotEqual(self.invoke("snapshot", "--gate-report", self.gate,
                                        "--input-snapshot", self.inputs, "--output", self.snapshot).returncode, 0)

    def test_x64_header_is_blocked(self):
        data = bytearray(self.binary.read_bytes())
        struct.pack_into("<H", data, 0x84, 0x8664)
        self.binary.write_bytes(data)
        with self.assertRaises(ValueError):
            module.require_x86_pe(self.binary)

    def test_arm32_header_is_blocked(self):
        data = bytearray(self.binary.read_bytes())
        struct.pack_into("<H", data, 0x84, 0x1C4)
        self.binary.write_bytes(data)
        with self.assertRaises(ValueError):
            module.require_x86_pe(self.binary)

    def test_flags_changed_after_build_are_blocked(self):
        self.make_receipt()
        with (self.root / "build-win32/CMakeCache.txt").open("a") as stream:
            stream.write("CMAKE_CXX_FLAGS:STRING=-ffast-math\n")
        self.assertNotEqual(self.check().returncode, 0)

    def test_source_change_during_gate_is_blocked(self):
        self.assertEqual(self.invoke("capture", "--output", self.inputs).returncode, 0)
        (self.root / "run.sh").write_text("changed\n")
        result = self.invoke("snapshot", "--gate-report", self.gate,
                             "--input-snapshot", self.inputs, "--output", self.snapshot)
        self.assertNotEqual(result.returncode, 0)
        self.assertFalse(self.snapshot.exists())


if __name__ == "__main__":
    unittest.main()
