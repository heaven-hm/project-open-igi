import pathlib
import sys
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
from native_unicorn_runner import compare_outputs, numeric_close


class DifferentialComparisonTests(unittest.TestCase):
    def test_integer_flags_and_descriptors_are_exact(self):
        for value in (0x12345678, 0x00567C74, -0x12345678):
            self.assertTrue(numeric_close(value, value, 1e-6, 1e-6))
            self.assertFalse(numeric_close(value, value + 1, 1e-6, 1e-6))

    def test_float_observables_retain_explicit_tolerance(self):
        self.assertTrue(numeric_close(1.0, 1.0 + 1e-8, 1e-6, 1e-6))
        self.assertFalse(numeric_close(1.0, 1.01, 1e-6, 1e-6))

    def test_bool_does_not_match_integer(self):
        self.assertFalse(numeric_close(True, 1, 1e-6, 1e-6))

    def test_wrong_script_descriptor_fails_output_comparison(self):
        reference = {"return_value": 0, "state_delta": {"block_type": 0x00567C74},
                     "side_effects": []}
        candidate = {**reference, "state_delta": {"block_type": 0x00567C75}}
        self.assertTrue(compare_outputs(reference, candidate))


if __name__ == "__main__":
    unittest.main()
