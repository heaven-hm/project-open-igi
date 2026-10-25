"""Repair-controller fixtures only; these never verify a game native."""
import argparse
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import native_autoverify as agent
from native_runtime_discovery import getter_probe


class NativeRepairController(unittest.TestCase):
    def test_private_server_identity_excludes_launchers_and_other_ports(self):
        from native_test_runtime import server_processes
        from types import SimpleNamespace
        windows=r'Z:\private\tmp\frida-server.exe';host='127.0.0.1:12345'
        listing=f'101 {windows} -l {host} \n102 winewrapper.exe --run -- {windows} -l {host}\n103 {windows} -l 127.0.0.1:12346\n'
        with patch('native_test_runtime.subprocess.run',return_value=SimpleNamespace(stdout=listing)):
            self.assertEqual(server_processes(windows,host),{101:f'{windows} -l {host}'})

    def test_private_server_cleanup_signals_only_owned_unchanged_identity(self):
        from native_test_runtime import stop_owned_server
        import signal
        with patch('native_test_runtime.server_processes',side_effect=[{101:'owned',102:'foreign'},{102:'foreign'}]),patch('native_test_runtime.os.kill') as kill:
            stop_owned_server('fixture','host',{101:'owned'})
            kill.assert_called_once_with(101,signal.SIGTERM)
        with patch('native_test_runtime.server_processes',return_value={101:'reused-pid'}),patch('native_test_runtime.os.kill') as kill:
            stop_owned_server('fixture','host',{101:'owned'})
            kill.assert_not_called()

    def test_concurrent_input_drift_retries_full_pipeline_on_fresh_fingerprint(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            job=dict(address='0x00400000',source='engine/fixture.cpp',fingerprint='before',status='pending')
            fresh=dict(job,fingerprint='after')
            seen=[]
            def run(root,current,record,folder,args):
                seen.append(current['fingerprint'])
                return dict(current,status='passed',verified=True)
            captured=({job['address']:fresh},{job['address']:{}},[],'new-context')
            with patch.object(agent,'verify_once',side_effect=run),patch.object(agent,'inventory',return_value=captured),patch.object(agent.time,'sleep'):
                result=agent.verify(root,job,{},root/'run',argparse.Namespace(max_attempts=3),context='old-context')
            self.assertEqual(seen,['before','after'])
            self.assertTrue(result['verified'])
            self.assertEqual(result['validated_context'],'new-context')
            rejected=json.loads((root/'run/attempt-1/result.json').read_text())
            self.assertFalse(rejected['verified'])
            self.assertEqual(result['attempts'][0]['result'],'pending')

    def test_concurrent_mapping_change_never_retargets_or_accepts_a_native(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp)
            job=dict(address='0x00400000',source='engine/fixture.cpp',fingerprint='before')
            other=dict(job,source='engine/other.cpp',fingerprint='after')
            captured=({job['address']:other},{job['address']:{}},[],'new-context')
            with patch.object(agent,'verify_once',return_value=dict(job,status='passed',verified=True)) as run,patch.object(agent,'inventory',return_value=captured):
                result=agent.verify(root,job,{},root/'run',argparse.Namespace(max_attempts=3),context='old-context')
            self.assertEqual(run.call_count,1)
            self.assertFalse(result['verified'])
            self.assertEqual(result['status'],'blocked')
            self.assertEqual(job['source'],'engine/fixture.cpp')

    def test_transient_failure_retries_and_retains_attempts(self):
        with tempfile.TemporaryDirectory() as folder:
            args=argparse.Namespace(max_attempts=3)
            job=dict(address='0x00400000',source_sha256='fixture')
            failures=[dict(job,status='blocked',verified=False,error='Frida timed out'),
                      dict(job,status='blocked',verified=False,error='semantic evidence gap')]
            with patch.object(agent,'verify_once',side_effect=failures) as once,patch.object(agent.time,'sleep'):
                result=agent.verify(Path(folder),job,{},Path(folder)/'job',args)
            self.assertEqual(once.call_count,2)
            self.assertFalse(result['verified'])
            self.assertEqual(len(result['attempts']),2)
            self.assertTrue((Path(folder)/'job/result.json').is_file())

    def test_behavior_gap_does_not_get_repaired_into_pass(self):
        with tempfile.TemporaryDirectory() as folder:
            failed=dict(address='0x00400000',status='blocked',verified=False,error='semantic evidence gap')
            with patch.object(agent,'verify_once',return_value=failed) as once:
                result=agent.verify(Path(folder),failed,{},Path(folder)/'job',argparse.Namespace(max_attempts=3))
            self.assertEqual(once.call_count,1)
            self.assertFalse(result['verified'])

    def test_worker_waits_for_existing_run_without_parallel_execution(self):
        import io
        with patch.object(agent.fcntl,'flock',side_effect=[BlockingIOError(),None]) as lock,patch.object(agent.time,'sleep'):
            agent.acquire_worker_lock(io.StringIO(),1)
        self.assertEqual(lock.call_count,2)
        with patch.object(agent.fcntl,'flock',side_effect=BlockingIOError()):
            with self.assertRaises(TimeoutError):agent.acquire_worker_lock(io.StringIO(),0)

    def test_model_backend_does_not_send_source_without_approval(self):
        from native_probe_agent import ask,CRITIC
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);(root/'scripts').mkdir()
            (root/'scripts/igi-workflow.json').write_text('{}')
            with patch('native_probe_agent.subprocess.run') as process:
                with self.assertRaisesRegex(ValueError,'source-egress approval'):
                    ask('codex',root,root/'proposal','critic',CRITIC,'fixture source')
                process.assert_not_called()

    def test_watch_releases_worker_between_cycles_and_stops_only_on_real_pass(self):
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);visits=[]
            def cycle(root,args):
                visits.append(1)
                passed=len(visits)==2
                agent.save(root/'build/native-auto/state.json',dict(verified=passed,selected_count=1))
                return 0 if passed else 1
            argv=['native_autoverify.py','watch','--root',str(root),'--until-passed','--watch-timeout','5']
            with patch.object(sys,'argv',argv),patch.object(agent,'cycle',side_effect=cycle),patch.object(agent,'acquire_worker_lock') as lock,patch.object(agent.time,'sleep'):
                self.assertEqual(agent.main(),0)
            self.assertEqual(len(visits),2)
            self.assertEqual(lock.call_count,2)

    def test_annotation_identity_rejects_code_or_location_changes(self):
        from native_source_review import annotation_equivalent
        old=b'// Native fixture\n// Verified: historical fixture\n// Tools: fixture\nint f(){return 1;}\n'
        moved=b'// Native fixture\n// Tools: fixture\n// Verified: historical fixture\nint f(){return 1;}\n'
        self.assertTrue(annotation_equivalent(old,moved))
        self.assertFalse(annotation_equivalent(old,moved.replace(b'return 1',b'return 2')))
        self.assertFalse(annotation_equivalent(old,moved.replace(b'int f(){return 1;}',b'int f(){return __LINE__;}')))
        self.assertFalse(annotation_equivalent(old,b'// Native fixture\n// Tools: fixture\nint f(){return 1;}\n// Verified: historical fixture\n'))

    def test_executed_build_overlay_does_not_promote_native_or_edit_catalog(self):
        from smart_end_to_end_verification import build_evidence_manifest
        with tempfile.TemporaryDirectory() as tmp:
            root=Path(tmp);(root/'docs/fidelity').mkdir(parents=True)
            record=dict(address='0x00400000',implementation={'path':'engine/fixture.cpp'},verification={'status':'TRANSCRIBED'})
            manifest=root/'docs/fidelity/native-verification.json'
            manifest.write_text(json.dumps({'natives':[record]}));original=manifest.read_bytes()
            candidate=root/'fixture.exe';candidate.write_bytes(b'infrastructure fixture')
            receipt=root/'receipt.json';receipt.write_text('{}')
            folder=root/'run';folder.mkdir()
            with patch('smart_end_to_end_verification.validate_receipt',return_value={'command':['cmake','--build','fixture']}):
                path=build_evidence_manifest(root,'0x00400000',record,'retail','source',candidate,receipt,folder)
            data=json.loads(path.read_text())['natives'][0]
            self.assertEqual(data['verification']['status'],'TRANSCRIBED')
            self.assertEqual(manifest.read_bytes(),original)
            self.assertTrue((root/data['verification']['build']['evidence_path']).is_file())

    def test_startup_completion_requires_explicit_uniform_contract(self):
        from frida_scoped_capture import startup_completion
        self.assertIsNone(startup_completion({'0x0048F0C0':{}}))
        approved=dict(mode='native-return',expected_calls=1,actions='none')
        self.assertEqual(startup_completion({'0x0048F0C0':{'captureCompletion':approved}}),approved)
        for value in [dict(approved,expected_calls=True),dict(approved,expected_calls=2),dict(approved,actions='escape')]:
            with self.assertRaises(ValueError):startup_completion({'0x0048F0C0':{'captureCompletion':value}})
        with self.assertRaises(ValueError):startup_completion({'a':{'captureCompletion':approved},'b':{}})

    def test_typed_generator_requires_exact_opcode_rule_in_both_images(self):
        retail=bytes.fromhex('a18c7c56008b403cc3')
        candidate=bytes.fromhex('a1000050008b403cc3')
        with patch('native_runtime_discovery.image_bytes',side_effect=[retail,candidate]):
            code,proof=getter_probe(Path('fixture'),Path('fixture'),0x400000,0x500000)
        self.assertEqual(proof['field_offset'],0x3c)
        self.assertIn('typedState',code)
        self.assertFalse(proof['writes_in_native'])
        with patch('native_runtime_discovery.image_bytes',side_effect=[retail,bytes(9)]):
            self.assertIsNone(getter_probe(Path('fixture'),Path('fixture'),0x400000,0x500000))


if __name__=='__main__':unittest.main()
