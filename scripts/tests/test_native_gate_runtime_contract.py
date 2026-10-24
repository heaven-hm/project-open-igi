from pathlib import Path
import sys
import unittest
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import native_gate as gate


class RuntimeContractTests(unittest.TestCase):
    def contract(self):
        return dict(scope='typed-native-fixture', probe_sha256='a'*64,
                    required_observables=dict(inputs=['enabled'], outputs=['return_value'],
                                              sideEffects=['written_byte']))

    def test_prose_semantic_checklist_without_runtime_contract_blocks(self):
        errors = []
        self.assertIsNone(gate.validate_semantic_runtime_contract({'reviewed': True}, '0x00405850', errors))
        self.assertTrue(errors)

    def test_unknown_effects_cannot_be_declared_as_empty_without_review(self):
        contract = self.contract()
        contract['required_observables']['sideEffects'] = []
        errors = []
        gate.validate_semantic_runtime_contract({'runtime_contract': contract}, '0x00405850', errors)
        self.assertTrue(errors)

    def test_matching_return_without_required_memory_effect_blocks(self):
        contract = self.contract()
        output = dict(return_value=1, state_delta={}, side_effects={})
        report = dict(scope=contract['scope'], probe_sha256=contract['probe_sha256'],
                      cases=[dict(id='case-0', input={'enabled': 1}, reference_output=output, candidate_output=output)])
        errors = []
        gate.validate_live_runtime_contract(report, contract, '0x00405850', errors)
        self.assertTrue(any('sideEffects' in error for error in errors))

    def test_different_injected_probe_blocks_even_matching_observations(self):
        contract = self.contract()
        output = dict(return_value=1, state_delta={}, side_effects={'written_byte': 1})
        report = dict(scope=contract['scope'], probe_sha256='b'*64,
                      cases=[dict(id='case-0', input={'enabled': 1}, reference_output=output, candidate_output=output)])
        errors = []
        gate.validate_live_runtime_contract(report, contract, '0x00405850', errors)
        self.assertEqual(len(errors), 1)
        self.assertIn('authorized probe/scope', errors[0])

    def test_void_noarg_fixture_can_observe_state_changes(self):
        contract = self.contract()
        contract['required_observables'].update(inputs=[], outputs=[])
        output = dict(return_value=None, state_delta={'byte': 1}, side_effects={'written_byte': 1})
        report = dict(scope=contract['scope'], probe_sha256=contract['probe_sha256'],
                      cases=[dict(id='void', input={'args': [], 'initial_state': {'byte': 0}},
                                  reference_output=output, candidate_output=output)])
        errors = []
        gate.validate_live_runtime_contract(report, contract, '0x00405850', errors)
        self.assertEqual(errors, [])
        report['cases'][0]['input']['args'] = [1]
        gate.validate_live_runtime_contract(report, contract, '0x00405850', errors)
        self.assertTrue(any('inputs' in error for error in errors))
        report['cases'][0]['input']['args'] = []
        output['return_value'] = 7
        errors = []
        gate.validate_live_runtime_contract(report, contract, '0x00405850', errors)
        self.assertTrue(any('outputs' in error for error in errors))


if __name__ == '__main__':
    unittest.main()
