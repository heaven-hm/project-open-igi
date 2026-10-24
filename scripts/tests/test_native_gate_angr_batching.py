import json
import pathlib
import sys
import tempfile
import unittest
from types import SimpleNamespace
from unittest.mock import patch

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import native_gate as gate  # noqa: E402


class AngrBatchingTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.helper = self.root / "scripts/angr_analyze.py"
        self.helper.parent.mkdir(parents=True)
        self.helper.write_text("# fixture helper\n")
        self.binary = self.root / "igi.exe"
        self.binary.write_bytes(b"retail image fixture")
        self.digest = gate.sha256(self.binary)

    def artifact(self, command):
        addresses = [command[i + 1] for i, value in enumerate(command[:-1]) if value == "--address"]
        return {
            "schema_version": 1,
            "kind": "angr-analysis",
            "result": "pass",
            "tool": "angr",
            "angr_version": "10.0.4.post1",
            "binary_path": str(self.binary.resolve()),
            "binary_sha256": self.digest,
            "bits": 32,
            "mapped_base": "0x400000",
            "functions": {
                gate.normalize_address(address): {
                    "result": "pass",
                    "function_entry": gate.normalize_address(address),
                    "nodes": [gate.normalize_address(address)],
                    "edges": [],
                    "has_unresolved_jumps": False,
                }
                for address in addresses
            },
        }

    @patch.dict("os.environ", {"IGI_ANGR_PYTHON": "python-fixture"})
    def test_large_address_set_uses_one_image_recovery_without_loss(self):
        addresses = [f"0x0040{i:04X}" for i in range(780)]

        def run(command, **kwargs):
            self.assertEqual(kwargs["timeout"], 180)
            return SimpleNamespace(stdout=json.dumps(self.artifact(command)), stderr="", returncode=0)

        with patch.object(gate.subprocess, "run", side_effect=run) as run_mock:
            result = gate.run_angr_analysis(self.root, self.binary, self.digest, addresses)

        self.assertNotIn("error", result)
        self.assertEqual(result["result"], "pass")
        self.assertEqual(result["batch_size"], 780)
        self.assertEqual([len(batch["addresses"]) for batch in result["batches"]], [780])
        self.assertEqual(len(result["functions"]), len(addresses))
        self.assertEqual(run_mock.call_count, 1)
        self.assertEqual(len(result["command"]), 1)

    def test_missing_function_result_blocks_the_whole_analysis(self):
        def run(command, **kwargs):
            artifact = self.artifact(command)
            artifact["functions"].pop("0x00401000")
            return SimpleNamespace(stdout=json.dumps(artifact), stderr="", returncode=0)
        with patch.object(gate.subprocess, "run", side_effect=run):
            result = gate.run_angr_analysis(self.root, self.binary, self.digest,
                                            ["0x00401000", "0x00401010"])
        self.assertEqual(result["functions"], {})
        self.assertIn("address mismatch", result["error"])

    def test_wrong_image_hash_blocks_the_whole_analysis(self):
        def run(command, **kwargs):
            artifact = self.artifact(command)
            artifact["binary_sha256"] = "0" * 64
            return SimpleNamespace(stdout=json.dumps(artifact), stderr="", returncode=0)
        with patch.object(gate.subprocess, "run", side_effect=run):
            result = gate.run_angr_analysis(self.root, self.binary, self.digest, ["0x00401000"])
        self.assertEqual(result["functions"], {})
        self.assertIn("digest differs", result["error"])

    @patch.dict("os.environ", {"IGI_ANGR_PYTHON": "python-fixture"})
    def test_invalid_batch_output_is_reported_with_batch_and_process_details(self):
        with patch.object(
            gate.subprocess,
            "run",
            return_value=SimpleNamespace(stdout="", stderr="killed", returncode=-9),
        ):
            result = gate.run_angr_analysis(self.root, self.binary, self.digest, ["0x00401000"])

        self.assertEqual(result["functions"], {})
        self.assertIn("batch 1/1", result["error"])
        self.assertIn("exit=-9", result["error"])
        self.assertIn("killed", result["error"])


if __name__ == "__main__":
    unittest.main()
