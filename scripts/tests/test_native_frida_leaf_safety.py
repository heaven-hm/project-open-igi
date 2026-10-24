import hashlib
import json
from pathlib import Path
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import native_frida_leaf_diff_runner as runner


class LeafProbeSafetyTests(unittest.TestCase):
    def test_case_insensitive_address_is_not_counted_twice(self):
        calls = [{'retval': '0x1'}]
        self.assertEqual(runner.calls_for({'0x00405850': calls}, '0x00405850'), calls)

    def test_unknown_pointer_observations_cannot_pass_by_count(self):
        with self.assertRaisesRegex(ValueError, 'typed comparator'):
            runner.typed_observation([{'args': ['0xdeadbeef'], 'retval': '0x0'}], False)

    def test_scalar_comparison_ignores_stack_words_beyond_the_signature(self):
        calls = [{'args': ['0x1', 'unrelated-stack-word'], 'retval': '0x2'}]
        result = runner.typed_observation(calls, True, 1)
        self.assertEqual(result['inputs'], [['0x1']])

    def validate(self, signature, contract):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            source, binary, probe = (root / name for name in ('native.cpp', 'igi.exe', 'probe.js'))
            for path in (source, binary, probe):
                path.write_text('fixture')
            review = root / 'docs/fidelity/evidence/review.json'
            review.parent.mkdir(parents=True)
            review.write_text(json.dumps({'runtime_contract': contract}))
            manifest = {'natives': [{'address': '0x00405850', 'verification': {'semantic_review': {
                'evidence_path': str(review.relative_to(root)),
                'evidence_sha256': hashlib.sha256(review.read_bytes()).hexdigest()}}}]}
            (root / 'docs/fidelity/native-verification.json').write_text(json.dumps(manifest))
            args = SimpleNamespace(root=root, implementation=source, binary=binary,
                                   binary_sha256=runner.verification_scope.file_digest(binary),
                                   implementation_sha256=runner.verification_scope.file_digest(source),
                                   address='0x00405850')
            with patch.object(runner.nfd, 'PROBE', probe), patch.object(runner.subprocess, 'run') as command:
                try:
                    return runner.validate_leaf_inputs(args, {}, signature)
                finally:
                    command.assert_not_called()

    def test_pointer_parameter_is_rejected_even_with_scalar_return(self):
        with self.assertRaisesRegex(ValueError, 'unknown/pointer ABI'):
            self.validate('int32_t Native(void* context)', {})

    def test_unproven_side_effects_cannot_be_replaced_with_an_empty_list(self):
        with self.assertRaisesRegex(ValueError, 'absence of side effects'):
            self.validate('int32_t Native(int32_t value)', {})

    def test_narrow_return_needs_explicit_width_observation(self):
        with self.assertRaisesRegex(ValueError, 'unspecified bits'):
            self.validate('uint8_t Native(int32_t value)', {})


if __name__ == '__main__':
    unittest.main()
