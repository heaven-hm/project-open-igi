from pathlib import Path
import sys
import unittest
from unittest.mock import patch
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import native_gate as gate


class WrongCandidateTests(unittest.TestCase):
    def test_unbound_source_blocks_without_executing_a_different_candidate(self):
        native = dict(address='0x00405850', name='Config_Read', line=1, comment_lines=[],
                      is_canonical_source=True)
        record = dict(name='Config_Read', implementation=dict(path='engine/src/retail/config/read.cpp'))
        with patch.object(gate, 'sha256', side_effect=AssertionError('must not test wrong implementation')):
            errors, details = gate.verify_record(
                Path('.'), Path('igi.exe'), 'image-digest', {'0x00405850': record}, native,
                Path('scripts/GameNatives/config/00405850-Config_Read.cpp'),
                'scripts/GameNatives/config/00405850-Config_Read.cpp',
                'VERIFIED', 0.85, True, True, False, {}, [])
        self.assertEqual(len(errors), 1)
        self.assertIn('implementation path does not match', errors[0])
        self.assertIsNone(details['cfg_similarity'])


if __name__ == '__main__':
    unittest.main()
