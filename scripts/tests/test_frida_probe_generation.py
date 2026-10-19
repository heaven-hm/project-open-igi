import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
from generate_frida_level_probe import implemented_targets  # noqa: E402


class FridaProbeTargetTests(unittest.TestCase):
    def test_target_count_tracks_added_and_removed_implementations(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = pathlib.Path(temporary)
            (root / "engine").mkdir()
            (root / "engine/native_a.cpp").write_text("void NativeA() {}", encoding="utf-8")
            (root / "engine/native_b.cpp").write_text("void NativeB() {}", encoding="utf-8")
            records = [
                {"address": "0x00401000", "name": "NativeA",
                 "implementation": {"path": "engine/native_a.cpp"}},
            ]
            self.assertEqual(len(implemented_targets(root, records)), 1)
            records.append({"address": "0x00402000", "name": "NativeB",
                            "implementation": {"path": "engine/native_b.cpp"}})
            self.assertEqual(len(implemented_targets(root, records)), 2)
            records.pop()
            self.assertEqual(len(implemented_targets(root, records)), 1)


if __name__ == "__main__":
    unittest.main()
