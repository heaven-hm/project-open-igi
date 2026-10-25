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
