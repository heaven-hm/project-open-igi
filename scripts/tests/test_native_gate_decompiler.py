import json
import pathlib
import sys
import tempfile
import unittest

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[1]))
import native_gate as gate


class DecompilerGateTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = pathlib.Path(self.temp.name)
        self.address = '0x00401000'
        self.binary = self.root / 'retail.exe'
        self.binary.write_bytes(b'fixture identity only; no binary execution in this test')
        self.digest = gate.sha256(self.binary)
        self.relative = 'engine/src/retail/example.cpp'
        self.impl = self.root / self.relative
        self.impl.parent.mkdir(parents=True)
        self.impl.write_text('int example() { return 0; }\n')
        reference = self.root / 'scripts/GameNatives/00401000-example.cpp'
        reference.parent.mkdir(parents=True)
        reference.write_text('int example() { return 0; }\n')
        self.record = {
            'name': 'example', 'address': self.address, 'binary_sha256': self.digest,
            'implementation': {'path': self.relative, 'sha256': gate.sha256(self.impl), 'signature': 'int(void)'},
            'game_natives': {'path': str(reference.relative_to(self.root)), 'sha256': gate.sha256(reference), 'signature': 'int(void)'},
            'verification': {'status': 'STRUCTURALLY_VERIFIED', 'selected_decompiler': 'ghidra',
                             'signature_comparison': {'reviewed': True, 'result': 'exact'},
                             'cfg_similarity': 1.0},
        }
        for tool in ('ghidra', 'radare2', 'binary_ninja'):
            self.install_artifact(tool)

    def install_artifact(self, tool, nodes=None):
        path = self.root / ('docs/fidelity/evidence/' + tool + '.json')
        path.parent.mkdir(parents=True, exist_ok=True)
        artifact = {'schema_version': 1, 'kind': 'native-disassembly', 'tool': tool,
                    'binary_sha256': self.digest, 'function_entry': self.address,
                    'signature': 'int(void)',
                    'cfg': {'nodes': [{'address': n} for n in (nodes or [self.address])], 'edges': []}}
        path.write_text(json.dumps(artifact))
        self.record['verification'][tool] = {'evidence_path': str(path.relative_to(self.root)),
                                            'evidence_sha256': gate.sha256(path),
                                            'evidence_id': 'fixture:' + tool, 'signature': 'int(void)'}

    def verify(self):
        native = {'name': 'example', 'address': self.address, 'line': 1,
                  'comment_lines': [], 'is_canonical_source': True}
        angr = {'functions': {self.address: {'result': 'pass', 'function_entry': self.address,
                                            'nodes': [self.address], 'has_unresolved_jumps': False}}}
        return gate.verify_record(self.root, self.binary, self.digest,
                                  {self.address: self.record}, native, self.impl, self.relative,
                                  'STRUCTURALLY_VERIFIED', .85, False, False, False, angr, [])

    def test_ghidra_and_radare2_are_sufficient_without_second_decompiler(self):
        del self.record['verification']['binary_ninja']
        errors, details = self.verify()
        self.assertEqual(errors, [])
        self.assertEqual(details['selected_decompiler'], 'ghidra')
        self.assertEqual(details['cfg_similarity'], 1.0)

    def test_missing_selected_artifact_cannot_be_rescued_by_binary_ninja(self):
        del self.record['verification']['ghidra']
        errors, _ = self.verify()
        self.assertTrue(any('ghidra' in e for e in errors), errors)

    def test_independent_radare2_remains_required(self):
        del self.record['verification']['radare2']
        errors, _ = self.verify()
        self.assertTrue(any('radare2' in e for e in errors), errors)

    def test_bad_selected_graph_cannot_be_rescued_by_matching_unused_graph(self):
        self.install_artifact('ghidra', [self.address, '0x00401010'])
        errors, details = self.verify()
        self.assertLess(details['cfg_similarity'], .85)
        self.assertTrue(any('below required' in e for e in errors), errors)

    def test_legacy_selection_and_invalid_explicit_selection(self):
        self.assertEqual(gate.selected_analysis_tools({'binary_ninja': {'evidence_path': 'legacy'}}), ('binary_ninja', 'radare2'))
        with self.assertRaises(gate.GateError):
            gate.selected_analysis_tools({})
        for invalid in ('both', '', [], True, None):
            with self.subTest(invalid=invalid), self.assertRaises(gate.GateError):
                gate.selected_analysis_tools({'selected_decompiler': invalid})


if __name__ == '__main__':
    unittest.main()
