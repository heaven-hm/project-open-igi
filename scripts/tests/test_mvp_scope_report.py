import json
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from mvp_scope_report import inventory


class MvpInventoryTests(unittest.TestCase):
    def test_unregistered_engine_native_and_reference_only_config_are_visible(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            files = {
                'docs/fidelity/native-verification.json': json.dumps({'schema_version': 1, 'natives': []}),
                'engine/src/retail/human/player.cpp': '// Native: Human_Test; retail VA 0x00401000; signature unknown.\n',
                'engine/src/retail/script/vm.cpp': '// Native: Qvm_Test; retail VA 0x00402000; signature unknown.\n',
                'scripts/GameNatives/config/00405850-Config_Read.cpp': '// reference\n',
            }
            for name, contents in files.items():
                path = root / name
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text(contents)
            report = inventory(root)
            self.assertEqual(report['count'], 2)
            self.assertEqual(report['missing_record_count'], 2)
            self.assertEqual(report['reference_only_count'], 1)
            self.assertEqual(report['reference_only_candidates'][0]['address'], '0x00405850')
            self.assertFalse(report['reference_only_candidates'][0]['engine_implementation_identified'])


if __name__ == '__main__':
    unittest.main()
