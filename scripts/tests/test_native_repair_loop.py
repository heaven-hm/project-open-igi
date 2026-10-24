import importlib.util
import json
from pathlib import Path
from unittest import mock
from types import SimpleNamespace
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).parents[1]))
import native_repair_loop as repair


class RepairTests(unittest.TestCase):
    def test_behavior_prioritized_and_reference_mapped(self):
        report = dict(checked_natives=[
            dict(path='engine/status.cpp', address='0x00400001', errors=['status is provisional']),
            dict(path='scripts/GameNatives/leaf.cpp', address='0x00400002', errors=['missing behavioral runner'])], failures=[])
        manifest = dict(natives=[dict(address='0x00400002', implementation=dict(path='engine/leaf.cpp'))])
        jobs, grouped = repair.select_jobs(report, None, 1, manifest)
        self.assertEqual(jobs[0]['path'], 'engine/leaf.cpp')
        self.assertEqual(jobs[0]['original_paths'], ['scripts/GameNatives/leaf.cpp'])
        self.assertEqual(grouped['checked_record_count'], 2)
        with self.assertRaises(ValueError):
            repair.select_jobs(report, {'0x00400003'}, 1)

    def test_missing_record_discovered_without_inventing_implementation(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            reference = root / 'scripts/GameNatives/human/004137A0-HumanPlayer.cpp'
            reference.parent.mkdir(parents=True)
            reference.write_text('reference only')
            (root / 'engine').mkdir()
            binary = root / 'retail.exe'
            binary.write_bytes(b'retail')
            report = dict(checked_natives=[dict(path=str(reference.relative_to(root)), address='0x004137A0',
                           errors=['no binary-bound verification record'])], failures=[])
            jobs, _ = repair.select_jobs(report, {'0x004137A0'}, 1, dict(natives=[]), root)
            self.assertEqual(jobs[0]['kind'], 'missing-record')
            result = repair.missing_record_dossier(root, root / 'out', jobs[0], binary, report)
            self.assertEqual(result['result'], 'missing-record-evidence-required')
            self.assertFalse(result['gate_passed'])
            dossier = json.loads((root / 'out/dossier.json').read_text())
            self.assertEqual(dossier['engine_address_mentions'], [])
            self.assertEqual(dossier['references'][0]['sha256'], repair.digest(reference))
            self.assertFalse((root / repair.MANIFEST).exists())

    def test_missing_analysis_requests_one_decompiler_and_references(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            reference = root / 'scripts/GameNatives/config/00405850-Config_Read.cpp'
            reference.parent.mkdir(parents=True)
            reference.write_text('candidate')
            (root / 'engine').mkdir()
            binary = root / 'retail.exe'
            binary.write_bytes(b'retail')
            job = dict(address='0x00405850', path=str(reference.relative_to(root)),
                       original_paths=[str(reference.relative_to(root))], failures=['missing record'])
            with mock.patch.object(repair, 'owned_process', return_value=dict(returncode=0, timed_out=False, interrupted=False)) as child:
                result = repair.missing_record_dossier(root, root / 'job', job, binary, {}, True, 30)
            prompt = child.call_args.args[4]
            self.assertIn('Binary Ninja MCP first', prompt)
            self.assertIn('Only if Binary Ninja is unavailable', prompt)
            self.assertIn('scripts/GameNatives/config/00405850-Config_Read.cpp', prompt)
            self.assertEqual(result['analysis_attempt']['source_mutations'], [])
            self.assertFalse(result['gate_passed'])

    def test_discovery_rejects_incomplete_gate(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            binary = root / 'retail.exe'
            binary.write_bytes(b'retail')
            with mock.patch.object(repair, 'owned_process', return_value=dict(returncode=1, timed_out=False, interrupted=False)):
                with self.assertRaisesRegex(ValueError, 'did not complete'):
                    repair.discover_addresses(root, root / 'out', binary, ['0x004137A0'], 5)

    def test_snapshot_uses_dirty_bytes_and_isolates_edits(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary) / 'root'
            (root / 'engine').mkdir(parents=True)
            (root / 'engine/leaf.cpp').write_text('dirty current source\n')
            (root / 'build').mkdir()
            (root / 'build/ignored').write_text('not source')
            (root / '.git').mkdir()
            replica = Path(temporary) / 'replica'
            repair.snapshot(root, replica)
            self.assertEqual((replica / 'engine/leaf.cpp').read_text(), 'dirty current source\n')
            (replica / 'engine/leaf.cpp').write_text('proposed change\n')
            self.assertEqual((root / 'engine/leaf.cpp').read_text(), 'dirty current source\n')
            self.assertFalse((replica / '.git').exists())
            self.assertFalse((replica / 'build').exists())

    def test_scope_bounds_and_denied_edits(self):
        info = dict(implementation=dict(path='engine/leaf.cpp'), evidence_paths=['docs/evidence.json'],
                    behavioral_runner='scripts/leaf_runner.py', address='0x00400001')
        self.assertTrue(repair.allowed('engine/leaf.cpp', info))
        self.assertTrue(repair.allowed('scripts/new_runner.py', info, set()))
        self.assertFalse(repair.allowed('scripts/native_gate.py', info))
        self.assertTrue(repair.allowed('docs/fidelity/native-verification.json', info))
        self.assertFalse(repair.allowed('engine/other.cpp', info))
        self.assertFalse(repair.allowed('scripts/other_runner.py', info, {'scripts/other_runner.py'}))
        with self.assertRaises(ValueError):
            repair.safe_relative('../escape')
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            a, b = root / 'a', root / 'b'
            a.mkdir(); b.mkdir()
            (a / 'safe.txt').write_text('before')
            (b / 'safe.txt').write_text('after')
            changed, denied = repair.changes(a, b, info, root / 'proposal.patch')
            self.assertEqual(denied, ['safe.txt'])
            self.assertEqual(changed, ['safe.txt'])

    def test_owned_timeout_records_child_and_never_accepts_claim(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            result = repair.owned_process([sys.executable, '-c', 'import time; time.sleep(5)'],
                                          root, root / 'child.log', .05)
            self.assertTrue(result['timed_out'])
            self.assertLess(result['returncode'], 0)
            self.assertIn('pid', json.loads((root / 'child.process.json').read_text()))
        self.assertFalse(repair.gate_passed(0, {'result':'passed', 'claim':'all fixed'}))
        self.assertFalse(repair.gate_passed(1, dict(result='passed', failures=[], threshold='VERIFIED',
                    cfg_threshold=1, semantic_review_required=True, behavioral_tests_required=True,
                    checked_natives=[{'address':'0x00400001'}])))


    def test_manifest_policy_preserves_unselected_records_and_status(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            a, b = root / 'a.json', root / 'b.json'
            original = dict(binary_sha256='image', natives=[
                dict(address='0x00400001', verification=dict(status='TRANSCRIBED'), implementation=dict(sha256='old')),
                dict(address='0x00400002', verification=dict(status='VERIFIED'))])
            a.write_text(json.dumps(original))
            changed = json.loads(a.read_text())
            changed['natives'][0]['implementation']['sha256'] = 'new'
            b.write_text(json.dumps(changed))
            self.assertTrue(repair.selected_manifest_only(a, b, '0x00400001'))
            changed['natives'][1]['verification']['status'] = 'TRANSCRIBED'
            b.write_text(json.dumps(changed))
            self.assertFalse(repair.selected_manifest_only(a, b, '0x00400001'))
            changed = json.loads(a.read_text())
            changed['natives'][0]['verification']['status'] = 'VERIFIED'
            b.write_text(json.dumps(changed))
            self.assertFalse(repair.selected_manifest_only(a, b, '0x00400001'))


    def test_generated_products_are_excluded_but_source_changes_retained(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for relative in ('build/out.o', 'build-win32/a.exe', 'scripts/__pycache__/test.pyc',
                             'scripts/real.py', 'engine/build/source.cpp', 'docs/evidence.json'):
                path = root / relative
                path.parent.mkdir(parents=True, exist_ok=True)
                path.write_text('fixture')
            self.assertEqual(set(repair.inventory(root)), {'scripts/real.py', 'engine/build/source.cpp', 'docs/evidence.json'})
        passed = dict(result='passed', failures=[], threshold='VERIFIED', cfg_threshold=.85,
                      semantic_review_required=True, behavioral_tests_required=True,
                      checked_native_count=1, binary_sha256='image',
                      checked_natives=[dict(address='0x00400001', path='engine/a.cpp', errors=[],
                                           angr=dict(implementation_sha256='source', binary_sha256='image'))])
        self.assertTrue(repair.gate_passed(0, passed, '0x00400001', 'engine/a.cpp', 'image', 'source'))
        self.assertFalse(repair.gate_passed(0, passed, '0x00400001', 'engine/a.cpp', 'wrong-image', 'source'))
        self.assertFalse(repair.gate_passed(0, passed, '0x00400001', 'engine/a.cpp', 'image', 'stale-source'))
        self.assertFalse(repair.gate_passed(0, passed, '0x00400002', 'engine/a.cpp'))
        self.assertFalse(repair.gate_passed(0, passed, '0x00400001', 'engine/b.cpp'))


    def test_sandbox_write_boundaries_and_post_gate_mutations(self):
        profile = repair.sandbox_profile(Path('/tmp/job/workspace'), Path('/tmp/job/tmp'),
                                         Path('/tmp/job/native-gate.json'), Path('/tmp/job/gate.log'))
        self.assertIn('(deny file-write*)', profile)
        self.assertIn('(subpath "/private/tmp/job/workspace")' if Path('/tmp').resolve() != Path('/tmp') else
                      '(subpath "/tmp/job/workspace")', profile)
        self.assertNotIn('(subpath "/tmp/job")', profile)
        self.assertNotIn('baseline', profile)
        self.assertNotIn('dossier', profile)
        self.assertEqual(repair.source_mutations({'engine/a.cpp':'old', 'docs/evidence':'same'},
                                                {'engine/a.cpp':'changed', 'docs/evidence':'same'}), ['engine/a.cpp'])


    def test_cli_flags_and_nonzero_child_skip_gate(self):
        command = repair.child_command(Path('/replica'), Path('/summary'))
        self.assertIn('--approve-for-me', command)
        self.assertNotIn('--sandbox', command)
        self.assertNotIn('--model', command)
        self.assertNotIn('--dangerously-bypass-approvals-and-sandbox', command)
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary) / 'root'
            (root / 'engine').mkdir(parents=True)
            (root / 'engine/leaf.cpp').write_text('fixture')
            binary = root / 'igi.exe'
            binary.write_text('fixture image')
            job = dict(address='0x00400001', path='engine/leaf.cpp', failures=['runner missing'])
            record = dict(address=job['address'], implementation=dict(path=job['path']), verification={})
            with mock.patch.object(repair, 'owned_process', return_value=dict(returncode=2, timed_out=False, interrupted=False)) as run:
                result = repair.run_job(root, Path(temporary) / 'job', job, record, binary, 1)
            self.assertEqual(run.call_count, 1)
            self.assertEqual(result['result'], 'candidate-incomplete')
            self.assertNotIn('gate', result)


    def test_nullable_fastpath_requires_missing_runner_fresh_exact_evidence(self):
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for folder in ('scripts', 'engine/src/retail/sys', 'engine/include/igi/retail/sys', 'docs/fidelity'):
                (root / folder).mkdir(parents=True, exist_ok=True)
            source = root / 'engine/src/retail/sys/leaf.cpp'
            source.write_text('#include "igi/retail/sys/leaf.hpp"\nnamespace igi::retail::sys {\nstd::int32_t Leaf(const void* context) { return 0; }\n}')
            (root / 'engine/include/igi/retail/sys/leaf.hpp').write_text('std::int32_t Leaf(const void* context);')
            (root / 'scripts/angr_analyze.py').write_text('helper')
            (root / 'scripts/nullable_getter_diff_runner.py').write_text('runner')
            binary = root / 'igi.exe'
            binary.write_text('fixture image')
            image, source_hash = repair.digest(binary), repair.digest(source)
            job = dict(address='0x00400001', path=source.relative_to(root).as_posix(),
                       failures=['missing verification.behavioral_runner'])
            record = dict(address=job['address'], name='Leaf', binary_sha256=image,
                          implementation=dict(path=job['path'], sha256=source_hash),
                          verification=dict(status='STRUCTURALLY_VERIFIED'))
            report = dict(binary_sha256=image, checked_natives=[dict(address=job['address'], path=job['path'],
                cfg_similarity=1.0, angr=dict(result='pass', function_entry=job['address'], nodes=[job['address']],
                binary_sha256=image, implementation_sha256=source_hash, has_unresolved_jumps=False,
                helper_sha256=repair.digest(root / 'scripts/angr_analyze.py')))])
            helper = SimpleNamespace(recognize_nullable_member_getter=lambda *_:dict(offset=16, code_hex='fixture'))
            with mock.patch.dict(sys.modules, nullable_getter_diff_runner=helper):
                profile = repair.nullable_getter_profile(root, job, record, binary, report)
                self.assertEqual(profile['symbol'], 'igi::retail::sys::Leaf')
                broken = dict(report, binary_sha256='wrong image')
                self.assertIsNone(repair.nullable_getter_profile(root, job, record, binary, broken))
                self.assertIsNone(repair.nullable_getter_profile(root, dict(job, failures=['behavioral mismatch']), record, binary, report))
                report['checked_natives'][0]['angr']['has_unresolved_jumps'] = True
                self.assertIsNone(repair.nullable_getter_profile(root, job, record, binary, report))
            manifest = root / repair.MANIFEST
            manifest.write_text(json.dumps(dict(natives=[record])))
            repair.register_nullable_runner(root, record, profile)
            updated = json.loads(manifest.read_text())['natives'][0]
            self.assertEqual(updated['verification']['status'], 'STRUCTURALLY_VERIFIED')
            self.assertEqual(updated['verification']['behavioral_runner']['args'][-1], profile['symbol'])


if __name__ == '__main__':
    unittest.main()
