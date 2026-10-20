"""Negative tests for scoped feature acceptance; fixtures are not retail evidence."""
import contextlib
import copy
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import check_frida_differential as checker
from native_frida_diff import scalar_abi_types
from verification_scope import file_digest, load_scope, pair_typed_sequences, stage_candidate
from verify_v4_catalog import verify as verify_catalog


class FeatureVerificationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        root = Path(self.temp.name)
        self.gate = root / 'gate.json'
        self.runtime = root / 'runtime.json'
        self.binary = root / 'candidate.exe'
        self.binary.write_bytes(b'fixture candidate')
        self.static = dict(result='passed', mode='changed', threshold='VERIFIED',
                           failures=[], cfg_threshold=0.85, semantic_review_required=True,
                           behavioral_tests_required=True, binary_sha256='retail-fixture',
                           checked_native_count=1,
                           checked_natives=[dict(address='0x00401000', errors=[])])
        self.static['checked_natives'][0]['runtime_contract'] = dict(scope='fixture', probe_sha256='a'*64,
            required_observables=dict(inputs=['value'], outputs=['result'], sideEffects=['counter']))
        self.gate.write_text(json.dumps(self.static))
        self.row = dict(address='0x00401000', name='Fixture', retailCalled=True,
                        candidateReachable=True, candidateCalled=True,
                        argumentsMatch=True, scalarReturn=True, valueMatch=True,
                        sideEffectsMatch=True, semanticComparator="a"*64)
        self.row['comparison'] = dict(scope='fixture',
            inputs=dict(retail=[{'value': 1}], candidate=[{'value': 1}]),
            outputs=dict(retail=[{'result': 2}], candidate=[{'result': 2}]),
            sideEffects=dict(retail=[{'counter': 3}], candidate=[{'counter': 3}]))
        state = dict(scope='fixture', inputs={'value': 1}, outputs={'result': 2}, sideEffects={'counter': 3})
        self.row['retailCalls'] = [dict(typedState=copy.deepcopy(state))]
        self.row['candidateCalls'] = [dict(typedState=copy.deepcopy(state))]
        self.doc = dict(probeSha256='a'*64, staticGateSha256=file_digest(self.gate),
                        binarySha256='retail-fixture', candidateSha256=file_digest(self.binary),
                        results=[copy.deepcopy(self.row)])

    def check(self, doc=None, extra=()):
        self.runtime.write_text(json.dumps(self.doc if doc is None else doc))
        argv = ['checker', '--report', str(self.runtime), '--gate-report', str(self.gate),
                '--candidate-exe', str(self.binary), *extra]
        with mock.patch.object(sys, 'argv', argv), contextlib.redirect_stdout(io.StringIO()), \
                contextlib.redirect_stderr(io.StringIO()):
            return checker.main()

    def test_complete_scoped_fixture_passes(self):
        self.assertEqual(self.check(), 0)

    def test_reviewed_typed_state_can_compare_pointer_based_behavior(self):
        row = dict(self.row, scalarReturn=False, valueMatch=False, semanticMatch=True)
        self.assertEqual(self.check(dict(self.doc, results=[row])), 0)

    def test_missing_extra_duplicate_native_block(self):
        for rows in ([], [self.row, self.row],
                     [dict(self.row, address='0x00402000')]):
            with self.subTest(rows=rows):
                self.assertEqual(self.check(dict(self.doc, results=rows)), 1)

    def test_call_count_or_matching_eax_does_not_prove_behavior(self):
        for field, value in [('argumentsMatch', None), ('sideEffectsMatch', None),
                             ('valueMatch', False), ('candidateCalled', False),
                             ('retailCalled', False), ('candidateReachable', False)]:
            with self.subTest(field=field):
                row = dict(self.row, **{field: value})
                self.assertEqual(self.check(dict(self.doc, results=[row])), 1)

    def test_skip_flag_cannot_reduce_required_scope(self):
        self.assertEqual(self.check(extra=['--allow-unobserved']), 1)

    def test_stale_static_report_or_candidate_blocks(self):
        self.assertEqual(self.check(dict(self.doc, staticGateSha256='old')), 1)
        self.binary.write_bytes(b'different candidate')
        self.assertEqual(self.check(), 1)

    def test_failed_semantics_cannot_advance_to_runtime(self):
        self.static['semantic_review_required'] = False
        self.gate.write_text(json.dumps(self.static))
        with self.assertRaises(ValueError):
            load_scope(self.gate)

    def test_pointer_unknown_float_and_wide_abi_never_become_zero_args(self):
        for signature in ('int F(void *ctx)', 'unknown', 'int F(float x)',
                          'int F(uint64_t x)', 'int F(double x)'):
            with self.subTest(signature=signature):
                self.assertIsNone(scalar_abi_types(signature)[1])
        self.assertEqual(scalar_abi_types('int F(void)'), (True, []))
        self.assertEqual(scalar_abi_types('int F(int value)'), (True, ['int']))
        self.assertFalse(scalar_abi_types('float F(int value)')[0])

    def test_malformed_runtime_records_block(self):
        self.assertEqual(self.check(dict(self.doc, results=['not a record'])), 1)

    def test_pass_flags_without_equal_underlying_state_block(self):
        row = copy.deepcopy(self.row)
        del row['comparison']
        self.assertEqual(self.check(dict(self.doc, results=[row])), 1)
        row = copy.deepcopy(self.row)
        row['comparison']['sideEffects']['candidate'] = {'changed': 1}
        self.assertEqual(self.check(dict(self.doc, results=[row])), 1)

    def test_catalog_checks_selected_feature_only(self):
        root = Path(self.temp.name)
        path = root / 'assets/IGINatives_v4.json'
        path.parent.mkdir()
        path.write_text(json.dumps(dict(Natives=[
            dict(address='0x00401000', name='Fixture', signature='int Fixture(int x)'),
            dict(address='0x00402000', name='Unrelated', signature='undefined4 Unrelated()')])))
        errors, _, summary = verify_catalog(root, 'fixture', selected_addresses={'0x00401000'})
        self.assertEqual(errors, [])
        self.assertEqual(summary['entries'], 1)

    def test_exact_comparison_preserves_observed_types(self):
        row = copy.deepcopy(self.row)
        row['retailCalls'][0]['typedState']['outputs'] = {'result': True}
        row['candidateCalls'][0]['typedState']['outputs'] = {'result': 1}
        row['comparison'] = pair_typed_sequences(row['retailCalls'], row['candidateCalls'])
        self.assertEqual(self.check(dict(self.doc, results=[row])), 1)

    def test_typed_probe_pair_requires_explicit_same_scope_state(self):
        state = dict(scope='fixture', inputs=[1], outputs=2, sideEffects={'counter': 3})
        pair = pair_typed_sequences([dict(typedState=state)], [dict(typedState=state)])
        self.assertEqual(pair['outputs'], dict(retail=[2], candidate=[2]))
        self.assertIsNone(pair_typed_sequences([dict(typedState=state)], []))
        other = dict(state, scope='different scenario')
        self.assertIsNone(pair_typed_sequences([dict(typedState=state)], [dict(typedState=other)]))

    def test_earlier_divergence_cannot_be_hidden_by_last_equal_call(self):
        row = copy.deepcopy(self.row)
        early = copy.deepcopy(row['candidateCalls'][0])
        early['typedState']['outputs']['result'] = 99
        row['retailCalls'].insert(0, copy.deepcopy(row['retailCalls'][0]))
        row['candidateCalls'].insert(0, early)
        row['comparison'] = pair_typed_sequences(row['retailCalls'], row['candidateCalls'])
        self.assertEqual(self.check(dict(self.doc, results=[row])), 1)

    def test_placeholder_states_and_unreviewed_contracts_block(self):
        row = copy.deepcopy(self.row)
        for side in ('retailCalls', 'candidateCalls'):
            row[side] = [dict(typedState=dict(scope='fixture', inputs={}, outputs={}, sideEffects={}))]
        row['comparison'] = pair_typed_sequences(row['retailCalls'], row['candidateCalls'])
        self.assertEqual(self.check(dict(self.doc, results=[row])), 1)
        self.static['checked_natives'][0]['runtime_contract']['probe_sha256'] = 'b' * 64
        self.gate.write_text(json.dumps(self.static))
        self.doc['staticGateSha256'] = file_digest(self.gate)
        self.assertEqual(self.check(), 1)

    def test_candidate_is_staged_beside_assets_without_replacing_installed_game(self):
        game = Path(self.temp.name) / 'game'
        game.mkdir()
        installed = game / 'open-igi.exe'
        installed.write_bytes(b'previous installed game')
        with stage_candidate(self.binary, game) as staged:
            self.assertEqual(staged.parent, game)
            self.assertNotEqual(staged, installed)
            self.assertEqual(staged.read_bytes(), self.binary.read_bytes())
        self.assertFalse(staged.exists())
        self.assertEqual(installed.read_bytes(), b'previous installed game')


if __name__ == '__main__':
    unittest.main()
