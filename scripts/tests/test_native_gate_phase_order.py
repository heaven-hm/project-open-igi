import hashlib
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import native_gate as gate


class PhaseOrderTests(unittest.TestCase):
    def test_failed_static_or_semantic_checks_cannot_launch_runtime(self):
        with tempfile.TemporaryDirectory() as temp:
            root = Path(temp)
            source = root / 'engine/src/retail/test.cpp'
            source.parent.mkdir(parents=True)
            source.write_text('int Candidate() { return 0; }\n')
            record = {'name': 'Candidate', 'implementation': {
                'path': 'engine/src/retail/test.cpp',
                'sha256': hashlib.sha256(source.read_bytes()).hexdigest(), 'signature': 'int Candidate()'},
                'verification': {'status': 'TRANSCRIBED', 'selected_decompiler': 'binary_ninja',
                                 'behavioral_runner': {'path': 'scripts/test.py'}}}
            native = dict(address='0x00405850', name='Candidate', line=1,
                          comment_lines=[], is_canonical_source=True)
            with patch.object(gate, 'run_live_behavioral_runner', side_effect=AssertionError('unsafe runtime')) as live:
                errors, details = gate.verify_record(
                    root, root / 'igi.exe', 'image-sha', {'0x00405850': record}, native,
                    source, 'engine/src/retail/test.cpp', 'VERIFIED', 0.85,
                    True, True, False, False, {}, [])
            self.assertTrue(errors)
            self.assertEqual(details['behavioral_tests']['result'], 'not-run')
            live.assert_not_called()


if __name__ == '__main__':
    unittest.main()
