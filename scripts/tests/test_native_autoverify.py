"""Automation fixtures only. These do not verify any game native."""
import argparse
import contextlib
import io
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import native_autoverify as auto


class AutomaticNativeVerification(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        for folder in ('engine/src/retail/core', 'engine/include', 'scripts', 'docs/fidelity', 'cmake'):
            (self.root / folder).mkdir(parents=True)
        (self.root / 'CMakeLists.txt').write_text('project(automation_fixture)\n')
        (self.root / 'scripts/igi-workflow.json').write_text(json.dumps({'candidate':'build/fixture.exe'}))
        self.source = self.root / 'engine/src/retail/core/fixture.cpp'
        self.source.write_text('// Native: Fixture; retail VA 0x0048F0C0.\nvoid Fixture() {}\n')
        self.manifest = self.root / 'docs/fidelity/native-verification.json'
        self.record = dict(address='0x0048F0C0', name='Fixture',
            implementation={'path':str(self.source.relative_to(self.root)), 'sha256':auto.digest(self.source)},
            verification={'status':'TRANSCRIBED'})
        self.manifest.write_text(json.dumps({'natives':[self.record]}))
        for args in [('init','-q'),('config','user.email','fixture@example.invalid'),
                     ('config','user.name','Automation fixture'),('add','.'),('commit','-qm','fixture baseline')]:
            subprocess.run(['git',*args],cwd=self.root,check=True,capture_output=True)

    def test_committed_native_is_still_in_all_scope(self):
        jobs, _, issues, _ = auto.inventory(self.root)
        self.assertFalse(issues)
        self.assertEqual(auto.selected_paths(self.root, 'changed'), set())
        self.assertEqual(auto.select(self.root,jobs,None,[]), {'0x0048F0C0'})

    def test_header_and_source_edits_invalidate_fingerprint(self):
        before = auto.inventory(self.root)[0]['0x0048F0C0']['fingerprint']
        (self.root/'engine/include/fixture.hpp').write_text('struct Changed {};\n')
        after = auto.inventory(self.root)[0]['0x0048F0C0']['fingerprint']
        self.assertNotEqual(before,after)
        self.source.write_text(self.source.read_text()+'// changed\n')
        self.assertNotEqual(after,auto.inventory(self.root)[0]['0x0048F0C0']['fingerprint'])

    def test_unregistered_file_cannot_hide_behind_no_provenance(self):
        path=self.root/'engine/src/retail/core/unregistered.cpp'
        path.write_text('void Unregistered() {}\n')
        issues=auto.inventory(self.root)[2]
        self.assertTrue(any(i['path'].endswith('unregistered.cpp') for i in issues))

    def test_real_cmake_build_change_selects_all_natives(self):
        jobs=auto.inventory(self.root)[0]
        (self.root/'CMakeLists.txt').write_text('project(automation_fixture)\nadd_compile_definitions(CHANGED)\n')
        self.assertEqual(auto.select(self.root,jobs,{'CMakeLists.txt'},[]),{'0x0048F0C0'})

    def test_verifier_only_targets_are_not_a_native_acceptance_scope(self):
        module=Path(auto.__file__).resolve().parents[1]/'cmake/NativeAutoVerification.cmake'
        (self.root/'cmake/NativeAutoVerification.cmake').write_bytes(module.read_bytes())
        with (self.root/'CMakeLists.txt').open('a') as stream:
            stream.write('include(cmake/NativeAutoVerification.cmake)\n')
        jobs=auto.inventory(self.root)[0]
        paths={'CMakeLists.txt','cmake/NativeAutoVerification.cmake'}
        self.assertEqual(auto.select(self.root,jobs,paths,[]),set())
        with (self.root/'cmake/NativeAutoVerification.cmake').open('a') as stream:
            stream.write('add_compile_definitions(CHANGED)\n')
        self.assertEqual(auto.select(self.root,jobs,paths,[]),{'0x0048F0C0'})

    def test_native_order_starts_with_main(self):
        path=self.root/'docs/fidelity/evidence/chronological-native-order.md'
        path.parent.mkdir(parents=True)
        path.write_text('| 1 | `0x48F530` | WinMain |\n| 2 | `0x48F0C0` | StoreCommandLine |\n')
        self.assertEqual(auto.work_order(self.root,{'0x0048F0C0','0x0048F530'}),
                         ['0x0048F530','0x0048F0C0'])

    def test_changed_inputs_do_not_skip_later_requested_native(self):
        from unittest.mock import patch
        second=dict(self.record,address='0x0048F0D0',name='FixtureSecond')
        self.manifest.write_text(json.dumps({'natives':[self.record,second]}))
        args=argparse.Namespace(state=None,scope='all',base=None,head='HEAD',address=[],
            command='run',level=1,timeout=1,retry=True)
        visited=[]
        def verify(root,job,record,folder,args):
            visited.append(job['address'])
            if len(visited)==1:
                self.source.write_text(self.source.read_text()+'// concurrent fixture edit\n')
            return dict(job,status='blocked',verified=False,error='infrastructure fixture')
        with patch.object(auto,'verify',side_effect=verify), patch.object(auto,'refresh_ledger'), contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(auto.cycle(self.root,args),1)
        self.assertEqual(visited,['0x0048F0C0','0x0048F0D0'])

    def new_candidate(self, name='NewCandidate', address='0x00460C30', basename='new.cpp'):
        path=self.root/'engine/src/retail/core'/basename
        path.write_text(f'// Native: {name}; retail VA {address}.\nvoid {name}() {{}}\n')
        return path

    def test_intake_registers_and_changed_run_executes_new_native(self):
        from unittest.mock import patch
        path=self.new_candidate()
        args=argparse.Namespace(state=None,scope='changed',base=None,head='HEAD',address=[],
            command='run',level=1,timeout=1,retry=True)
        visited=[]
        def verify(root,job,record,folder,args):
            visited.append(job['address'])
            self.assertEqual(record['verification']['status'],'TRANSCRIBED')
            self.assertEqual(record['implementation']['sha256'],auto.digest(path))
            return dict(job,status='blocked',verified=False,error='fixture runtime failure')
        output=io.StringIO()
        with patch.object(auto,'verify',side_effect=verify),patch.object(auto,'refresh_ledger'),contextlib.redirect_stdout(output):
            self.assertEqual(auto.cycle(self.root,args),1)
        report=json.loads(output.getvalue())
        self.assertEqual(visited,['0x00460C30'])
        self.assertEqual(report['selected_count'],1)
        self.assertEqual(report['issue_count'],0)
        self.assertFalse(report['verified'])
        self.assertEqual(report['intake'][0]['address'],'0x00460C30')

    def test_intake_is_idempotent_and_preserves_other_records(self):
        path=self.new_candidate();paths={str(path.relative_to(self.root))}
        self.assertEqual(len(auto.intake_candidates(self.root,paths,[])),1)
        first=self.manifest.read_bytes()
        self.assertEqual(auto.intake_candidates(self.root,paths,[]),[])
        self.assertEqual(first,self.manifest.read_bytes())
        self.assertEqual(json.loads(first)['natives'][0],self.record)

    def test_intake_cannot_bind_duplicate_va_definitions(self):
        self.new_candidate(basename='one.cpp');self.new_candidate(basename='two.cpp')
        self.assertEqual(auto.intake_candidates(self.root,None,[]),[])
        self.assertNotIn('0x00460C30',auto.inventory(self.root)[0])

    def test_intake_preserves_conflicting_existing_engine_binding(self):
        self.new_candidate(name='Conflicting',address='0x0048F0C0')
        before=self.manifest.read_bytes()
        self.assertEqual(auto.intake_candidates(self.root,None,[]),[])
        self.assertEqual(self.manifest.read_bytes(),before)
        self.assertTrue(any(i.get('address')=='0x0048F0C0' for i in auto.inventory(self.root)[2]))

    def test_intake_downgrades_legacy_catalog_label_and_retains_reference(self):
        path=self.new_candidate()
        old=dict(address='0x00460C30',name='OldCatalog',implementation={'path':'scripts/GameNatives/old.cpp'},
                 verification={'status':'VERIFIED'})
        self.manifest.write_text(json.dumps({'natives':[self.record,old]}))
        added=auto.intake_candidates(self.root,None,[])
        self.assertEqual(len(added),1)
        row=json.loads(self.manifest.read_text())['natives'][1]
        self.assertEqual(row['verification']['status'],'TRANSCRIBED')
        prior=row['verification']['prior_catalog_record']
        self.assertEqual(json.loads((self.root/prior['path']).read_text()),old)

    def test_intake_does_not_register_a_comment_or_prototype(self):
        path=self.new_candidate()
        path.write_text('// Native: NewCandidate; retail VA 0x00460C30.\n// void NewCandidate() {}\nvoid NewCandidate();\n')
        self.assertEqual(auto.intake_candidates(self.root,None,[]),[])

    def test_partial_staging_is_blocked(self):
        self.source.write_text(self.source.read_text()+'// staged\n')
        subprocess.run(['git','add','.'],cwd=self.root,check=True)
        self.source.write_text(self.source.read_text()+'// unstaged\n')
        args=argparse.Namespace(state=None,scope='staged',base=None,head='HEAD',address=[],
            command='scan',level=1,timeout=1,retry=False)
        with contextlib.redirect_stdout(io.StringIO()), __import__("unittest.mock", fromlist=["patch"]).patch.object(auto, "refresh_ledger"):
            code=auto.cycle(self.root,args)
        self.assertEqual(code,1)
        data=json.loads((self.root/'build/native-auto/state.json').read_text())
        self.assertFalse(data['verified'])
        self.assertTrue(any('partial-stage' in i.get('error','') for i in data['issues']))

    def test_passing_flag_without_paired_phases_is_not_evidence(self):
        job={'address':'0x0048F0C0','source_sha256':'source'}
        report=dict(schema_version=1,kind='igi-one-native-verification',verified=True,result='passed',
            address=job['address'],implementation_sha256='source',binary_sha256='retail',
            candidate_sha256='candidate',phases={})
        self.assertFalse(auto.complete_report(report,job,{'retail_sha256':'retail'},'candidate'))
        report['phases']={k:{'exit_code':0} for k in
            ('static_semantic_behavioral','paired_runtime_capture','typed_runtime_comparison')}
        self.assertTrue(auto.complete_report(report,job,{'retail_sha256':'retail'},'candidate'))
        self.assertFalse(auto.complete_report(report,job,{'retail_sha256':'retail'},'different image'))


if __name__ == '__main__':
    unittest.main()
