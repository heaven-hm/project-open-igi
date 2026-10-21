#!/usr/bin/env python3
"""Default build acceptance for the exact changed-native scope; empty scopes block."""
import argparse
import json
import os
from pathlib import Path
import sys
from datetime import datetime, timezone
import uuid
from candidate_build_receipt import validate_receipt
from smart_end_to_end_verification import ROOT, digest, invoke, read_config
from verification_scope import load_scope


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--probe',type=Path,help='Reviewed typed probe covering the complete changed-native scope')
    parser.add_argument('--level',type=int,default=1,choices=range(1,15))
    args=parser.parse_args(); c=read_config()
    folder=ROOT/'build/feature-acceptance'/(datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ-')+uuid.uuid4().hex[:8])
    folder.mkdir(parents=True); gate_path=folder/'native-verification.json'; pair_path=folder/'frida-pair.json'
    report=dict(kind='igi-changed-feature-acceptance',schema_version=1,result='blocked',verified=False,phases={})
    env=dict(os.environ,LC_ALL='C',LANG='C',PYTHONDONTWRITEBYTECODE='1')
    env.setdefault('IGI_ANGR_PYTHON',c['angr_python']); env.setdefault('FRIDA_HOST',c['frida_host'])
    candidate=ROOT/c['candidate']; build=ROOT/c['build_directory']; receipt=build/'candidate-receipt.json'
    try:
        diagnostic=invoke([str(ROOT/'build.sh'),'--diagnostic'],env)
        report['phases']['diagnostic_compile']=diagnostic
        if diagnostic['exit_code']: raise ValueError('diagnostic compilation failed')
        compiled=invoke([sys.executable,str(ROOT/'scripts/candidate_build_receipt.py'),'--root',str(ROOT),
            '--build-dir',str(build),'--candidate',str(candidate),'--receipt',str(receipt)],env)
        report['phases']['build_receipt']=compiled
        if compiled['exit_code']: raise ValueError('current build receipt failed')
        validate_receipt(receipt,ROOT,candidate)
        env.update(IGI_CANDIDATE_EXE=str(candidate),IGI_CANDIDATE_BUILD_RECEIPT=str(receipt))
        command=[sys.executable,str(ROOT/'scripts/native_gate.py'),'--root',str(ROOT),'--changed',
                 '--threshold','VERIFIED','--cfg-threshold','0.85','--require-semantic-evidence',
                 '--require-behavioral-tests','--report',str(gate_path)]
        gate=invoke(command,env,1800);report['phases']['changed_native_gate']=gate
        if gate['exit_code']: raise ValueError('changed-native static/semantic/behavioral gate failed')
        addresses,_,data=load_scope(gate_path)
        records=json.loads((ROOT/'docs/fidelity/native-verification.json').read_text())['natives']
        sources=[]; probes=set()
        for address in addresses:
            matches=[r for r in records if int(r['address'],16)==int(address,16)]
            if len(matches)!=1: raise ValueError('required native has no unique record')
            r=matches[0];source=(ROOT/r['implementation']['path']).resolve()
            if not source.is_relative_to(ROOT/'engine') or not source.is_file():
                raise ValueError('required native is not implemented in this clean engine')
            if digest(source)!=r['implementation']['sha256']: raise ValueError('source hash changed')
            sources.append(source)
            path=r.get('verification',{}).get('runtime_probe',{}).get('path')
            if path: probes.add(ROOT/path)
        validate_receipt(receipt,ROOT,candidate,sources)
        probe=args.probe or (next(iter(probes)) if len(probes)==1 else None)
        if not probe or not probe.is_file(): raise ValueError('missing reviewed typed probe for complete changed scope')
        if any(n.get('runtime_contract',{}).get('probe_sha256')!=digest(probe) for n in data['checked_natives']):
            raise ValueError('probe differs from reviewed runtime contracts')
        capture=invoke([sys.executable,str(ROOT/'scripts/native_frida_diff.py'),'--gate-report',str(gate_path),
            '--candidate-exe',str(candidate),'--probe',str(probe),'--level',str(args.level),'--out',str(pair_path)],env)
        report['phases']['paired_capture']=capture
        if capture['exit_code']: raise ValueError('paired capture failed')
        checked=invoke([sys.executable,str(ROOT/'scripts/check_frida_differential.py'),'--report',str(pair_path),
            '--gate-report',str(gate_path),'--candidate-exe',str(candidate)],env)
        report['phases']['typed_comparison']=checked
        if checked['exit_code']: raise ValueError('paired typed runtime comparison failed')
        validate_receipt(receipt,ROOT,candidate,sources)
        if digest(Path(c['retail_exe']))!=c['retail_sha256']: raise ValueError('retail changed during capture')
        report.update(result='passed',verified=True,addresses=sorted(addresses),candidate_sha256=digest(candidate))
        return 0
    except (OSError,ValueError,KeyError,TypeError,__import__('subprocess').SubprocessError) as error:
        report['error']=str(error);return 1
    finally:
        output=folder/'report.json';output.write_text(json.dumps(report,indent=2)+'\n')
        print(f"{report['result']}: {report.get('error','exact changed scope accepted')}");print(output)


if __name__=='__main__': raise SystemExit(main())
