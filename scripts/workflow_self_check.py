#!/usr/bin/env python3
"""Infrastructure regression: valid report shape and deliberately corrupted reports.

Fixture observations are parser inputs, never retail observations or native tests.
"""
import copy
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import tempfile
from verification_scope import pair_typed_sequences

ROOT = Path(__file__).resolve().parents[1]


def main():
    cases = []
    with tempfile.TemporaryDirectory(prefix='igi-report-check-') as folder:
        folder = Path(folder)
        gate_path, pair_path = folder/'gate.json', folder/'pair.json'
        probe = hashlib.sha256(b'validator fixture only').hexdigest()
        contract = dict(scope='validator-fixture',probe_sha256=probe,
                        required_observables=dict(inputs=['fixture'],outputs=['fixture'],sideEffects=['fixture']))
        gate = dict(result='passed',failures=[],threshold='VERIFIED',semantic_review_required=True,
                    behavioral_tests_required=True,cfg_threshold=0.85,checked_native_count=1,
                    binary_sha256=hashlib.sha256(b'fixture retail').hexdigest(),
                    checked_natives=[dict(address='0x0048F0D0',errors=[],runtime_contract=contract)])
        gate_path.write_text(json.dumps(gate))
        state = dict(scope='validator-fixture',inputs=dict(fixture='input'),
                     outputs=dict(fixture='output'),sideEffects=dict(fixture='changed-state'))
        calls = [dict(typedState=state)]
        row = dict(address='0x0048F0D0',name='validator fixture; not a native claim',retailCalled=True,
                   candidateReachable=True,candidateCalled=True,scalarReturn=False,
                   semanticComparator=probe,argumentsMatch=True,semanticMatch=True,sideEffectsMatch=True,
                   retailCalls=copy.deepcopy(calls),candidateCalls=copy.deepcopy(calls),
                   comparison=pair_typed_sequences(calls,calls))
        good = dict(results=[row],probeSha256=probe,binarySha256=gate['binary_sha256'],
                    staticGateSha256=hashlib.sha256(gate_path.read_bytes()).hexdigest())
        variants = [('complete_validator_fixture',good,True)]
        def variant(name, mutate):
            value=copy.deepcopy(good); mutate(value); variants.append((name,value,False))
        variant('empty_scope',lambda d:d.update(results=[]))
        variant('duplicate_address',lambda d:d['results'].append(copy.deepcopy(d['results'][0])))
        variant('stale_gate_hash',lambda d:d.update(staticGateSha256='0'*64))
        variant('different_probe',lambda d:d.update(probeSha256='0'*64))
        variant('candidate_unobserved',lambda d:d['results'][0].update(candidateCalled=False))
        variant('dropped_call',lambda d:d['results'][0].update(candidateCalls=[]))
        variant('changed_side_effect',lambda d:d['results'][0]['candidateCalls'][0]['typedState']['sideEffects'].update(fixture='different'))
        variant('missing_required_field',lambda d:d['results'][0]['candidateCalls'][0]['typedState']['outputs'].clear())
        for name, data, expected in variants:
            pair_path.write_text(json.dumps(data))
            command=[sys.executable,str(ROOT/'scripts/check_frida_differential.py'),'--report',str(pair_path),'--gate-report',str(gate_path)]
            run=subprocess.run(command,cwd=ROOT,capture_output=True,text=True)
            passed=(run.returncode==0)==expected
            cases.append(dict(name=name,expected_acceptance=expected,checker_exit_code=run.returncode,
                              passed=passed,stdout=run.stdout,stderr=run.stderr))
        blocked=subprocess.run([sys.executable,str(ROOT/'scripts/smart_end_to_end_verification.py'),'--address','0x00405850'],
                               cwd=ROOT,capture_output=True,text=True)
        cases.append(dict(name='historical_reference_is_not_current_implementation',passed=blocked.returncode!=0 and '\nFalse\n' in '\n'+blocked.stdout,
                          stdout=blocked.stdout,stderr=blocked.stderr))
    result=dict(kind='igi-workflow-self-check',schema_version=1,result='passed' if all(c['passed'] for c in cases) else 'failed',
                native_verified=False,scope='report-validator infrastructure only; generated fixtures are not game evidence',cases=cases)
    path=ROOT/'build/workflow-self-check.json'; path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(json.dumps(result,indent=2)+'\n')
    print(f"{result['result']}: {sum(c['passed'] for c in cases)}/{len(cases)} infrastructure checks; native_verified=False")
    print(path)
    return 0 if result['result']=='passed' else 1


if __name__=='__main__': raise SystemExit(main())
