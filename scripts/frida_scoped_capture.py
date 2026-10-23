#!/usr/bin/env python3
"""Paired pre-resume Frida capture of a strict gate's exact typed native scope.

No legacy symbol-map inventory or post-launch attachment. Only owned processes
are terminated. Unknown ABI/layout/probe contracts and pre-existing games block.
"""
from __future__ import annotations
import argparse
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import time
from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
from verification_scope import (file_digest, load_scope, pair_typed_sequences,
                                stage_candidate, validate_runtime_contract,
                                validate_native_symbol_binding)
from check_frida_differential import behavior_matches

ROOT=Path(__file__).resolve().parents[1]


def win_path(path): return 'Z:\\'+str(Path(path).resolve())[1:].replace('/','\\')


def process_names(device):
    return [p.name for p in device.enumerate_processes() if p.name.lower() in ('igi.exe','open-igi.exe','igi1.exe')
            or p.name.lower().startswith('open-igi-probe-')]


def image_base(path):
    raw=path.read_bytes(); pe=struct.unpack_from('<I',raw,0x3c)[0]
    if raw[pe:pe+4]!=b'PE\0\0' or struct.unpack_from('<H',raw,pe+24)[0]!=0x10b:
        raise ValueError('expected PE32 image')
    return struct.unpack_from('<I',raw,pe+52)[0]


def capture(device, executable, rows, side, primary_probe, action_probe, args, folder, game):
    record=dict(executable=str(executable),executable_sha256=file_digest(executable),result='incomplete',
                cwd=str(game),arguments=['window',f'level{args.level}'],calls={a:[] for a in rows},hooks=[])
    pid=None; session=None; script=None; actions=None; failures=[]
    raw=(folder/f'{side}.jsonl').open('w')
    try:
        pid=device.spawn([win_path(executable),'window',f'level{args.level}'],cwd=win_path(game))
        record['owned_pid']=pid;session=device.attach(pid)
        script=session.create_script(primary_probe)
        def on_message(message,data):
            raw.write(json.dumps(message)+'\n');raw.flush()
            payload=message.get('payload') or {}
            if data is not None: failures.append('unexpected unrecorded binary payload')
            if message.get('type')=='error' or payload.get('type')=='hook_failed': failures.append(message)
            if payload.get('type')=='observed':
                address=payload.get('address')
                if address not in record['calls']: failures.append('unexpected observed native address');return
                values=record['calls'][address]
                if len(values)>=10000: failures.append('call capacity exceeded; no truncated acceptance');return
                values.append(payload)
        script.on('message',on_message);script.load()
        for address,layout in rows.items():
            hook=script.exports_sync.hook(address,dict(layout,moduleName=executable.name,preferredBase=f'0x{image_base(executable):08X}'))
            record['hooks'].append(hook)
            if not isinstance(hook,dict) or hook.get('installed') is not True or hook.get('address')!=address:
                raise ValueError('typed probe did not confirm installation for every native')
        if failures: raise ValueError(f'probe installation error: {failures}')
        record['hook_phase']='before-primary-thread-resume';device.resume(pid)
        started=time.monotonic();deadline=started+args.settle+args.observe;first=None
        record['escape']={'sent':False}
        while time.monotonic()<deadline:
            if failures: raise ValueError(f'capture errors: {failures}')
            if pid not in {p.pid for p in device.enumerate_processes()}:
                raise ValueError('owned game exited during required observation')
            if all(record['calls'].values()) and first is None:
                first=time.monotonic();deadline=first+args.observe
            if time.monotonic()-started>=2 and not record['escape']['sent']:
                if actions is None: actions=session.create_script(action_probe);actions.load()
                record['escape']=actions.exports_sync.press_escape()
            time.sleep(0.2)
        if not record['escape'].get('sent'): raise ValueError('requested Escape was not delivered')
        if any(not values for values in record['calls'].values()): raise ValueError('required method unobserved')
        if failures: raise ValueError(f'capture errors: {failures}')
        record['result']='captured'
    except Exception as error:
        record.update(result='failed',error=repr(error))
    finally:
        if pid is not None:
            try:
                device.kill(pid)
                timeout=time.monotonic()+5
                while pid in {p.pid for p in device.enumerate_processes()} and time.monotonic()<timeout: time.sleep(0.1)
                record['owned_process_terminated']=pid not in {p.pid for p in device.enumerate_processes()}
                if not record['owned_process_terminated']: record.update(result='failed',cleanup_error='owned process remains')
            except Exception as error: record.update(result='failed',cleanup_error=repr(error))
        if session:
            try: session.detach()
            except Exception: pass
        raw.close()
        (folder/f'{side}.json').write_text(json.dumps(record,indent=2)+'\n')
    return record


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--gate-report',required=True,type=Path)
    parser.add_argument('--candidate-exe',required=True,type=Path)
    parser.add_argument('--receipt',type=Path)
    parser.add_argument('--probe',required=True,type=Path)
    parser.add_argument('--level',type=int,default=1,choices=range(1,15))
    parser.add_argument('--observe',type=float,default=5.0)
    parser.add_argument('--settle',type=float,default=45.0)
    parser.add_argument('--out',required=True,type=Path)
    args=parser.parse_args()
    if args.observe<2 or args.settle<0: parser.error('observe must be >=2s; settle must be nonnegative')
    config=json.loads((ROOT/'scripts/igi-workflow.json').read_text())
    try: import frida
    except ImportError:
        python=config['frida_python']
        if Path(python).resolve()==Path(sys.executable).resolve(): raise
        os.execv(python,[python,str(Path(__file__).resolve()),*sys.argv[1:]])
    doc=dict(kind='igi-paired-native-runtime',schema_version=1,result='blocked',results=[],
             captureMode='sequential-before-resume',level=args.level,arguments=['window',f'level{args.level}'])
    folder=args.out.parent/'captures';folder.mkdir(parents=True,exist_ok=True)
    try:
        required,scope_digest,gate=load_scope(args.gate_report)
        candidate=args.candidate_exe.resolve();retail=Path(config['retail_exe']);game=retail.parent
        receipt=args.receipt or Path(os.environ.get('IGI_CANDIDATE_BUILD_RECEIPT',str(ROOT/config['build_directory']/'candidate-receipt.json')))
        if file_digest(retail)!=config['retail_sha256'] or gate.get('binary_sha256')!=file_digest(retail):
            raise ValueError('retail hash differs from the selected gate or tool profile')
        manifest=json.loads((ROOT/'docs/fidelity/native-verification.json').read_text())['natives']
        nm=subprocess.run(['i686-w64-mingw32-nm','--defined-only',str(candidate)],capture_output=True,text=True,check=True,timeout=30)
        symbols={parts[2]:parts[0] for line in nm.stdout.splitlines() if len(parts:=line.split())==3}
        layouts={'retail':{},'candidate':{}};sources=[];records={}
        action_path=ROOT/'scripts/frida_scenario_actions.js';action_hash=file_digest(action_path)
        for address in required:
            matches=[r for r in manifest if int(r['address'],16)==int(address,16)]
            if len(matches)!=1: raise ValueError('required native has no unique manifest record')
            r=matches[0];records[address]=r;v=r['verification'];source=(ROOT/r['implementation']['path']).resolve()
            if not source.is_relative_to(ROOT/'engine') or file_digest(source)!=r['implementation']['sha256']:
                raise ValueError('source is not current engine code')
            sources.append(source)
            mapping=v.get('runtime_layout')
            if not isinstance(mapping,dict) or set(mapping)!= {'retail','candidate'}:
                raise ValueError(f'{address}: missing reviewed retail/candidate layout')
            symbol=v.get('candidate_symbol')
            validate_native_symbol_binding(dict(address=address,
                enginePath=r['implementation']['path'],candidateSymbol={'symbol':symbol}))
            if symbol not in symbols: raise ValueError(f'{address}: missing compiled candidate symbol')
            layouts['retail'][address]=dict(entryVA=address,**mapping['retail'])
            layouts['candidate'][address]={'entryVA':'0x'+symbols[symbol]}
            for key,value in mapping['candidate'].items():
                if value not in symbols: raise ValueError(f'{address}: missing compiled state symbol')
                layouts['candidate'][address][key]='0x'+symbols[value]
        validate_receipt(receipt,ROOT,candidate,sources)
        contracts={n['address']:n.get('runtime_contract',{}) for n in gate['checked_natives']}
        if any(c.get('probe_sha256')!=file_digest(args.probe) or c.get('actions_sha256')!=action_hash for c in contracts.values()):
            raise ValueError('reviewed probe/actions contract is missing or stale')
        identity_files=[retail,candidate,*sources,game/'config.qvm',game/'MISSIONS/location0'/f'LEVEL{args.level}'/'OBJECTS.QVM']
        before={str(p):file_digest(p) for p in identity_files}
        doc.update(binarySha256=file_digest(retail),candidateSha256=file_digest(candidate),staticGateSha256=scope_digest,
                   probeSha256=file_digest(args.probe),actionsSha256=action_hash,sourceAndScenarioHashes=before,
                   requiredAddresses=sorted(required),gameDirectory=str(game))
        device=frida.get_device_manager().add_remote_device(os.environ.get('FRIDA_HOST',config['frida_host']))
        with capture_lease(device,process_names):
            doc['retailCapture']=capture(device,retail,layouts['retail'],'retail',args.probe.read_text(),action_path.read_text(),args,folder,game)
            if doc['retailCapture']['result']!='captured': raise ValueError('retail capture failed')
            with stage_candidate(candidate,game) as staged:
                doc['candidateCapture']=capture(device,staged,layouts['candidate'],'candidate',args.probe.read_text(),action_path.read_text(),args,folder,game)
            if doc['candidateCapture']['result']!='captured': raise ValueError('candidate capture failed')
        for address in sorted(required):
            left=doc['retailCapture']['calls'][address];right=doc['candidateCapture']['calls'][address]
            row=dict(address=address,name=records[address]['name'],retailCalled=True,candidateReachable=True,
                     candidateCalled=True,scalarReturn=False,retailCalls=left,candidateCalls=right,
                     semanticComparator=doc['probeSha256'],comparison=pair_typed_sequences(left,right))
            row['argumentsMatch']=row['comparison'] is not None and row['comparison']['inputs']['retail']==row['comparison']['inputs']['candidate']
            row['semanticMatch']=behavior_matches(row)
            doc['results'].append(row)
            validate_runtime_contract(row,contracts[address],doc['probeSha256'])
            if not row['semanticMatch']: raise ValueError(f'{address}: earliest typed state divergence')
        validate_receipt(receipt,ROOT,candidate,sources)
        if before!={str(p):file_digest(p) for p in identity_files}: raise ValueError('source/image/scenario changed during capture')
        doc.update(result='passed',scope='Recorded same-input typed observations for the exact gate scope only')
        return 0
    except Exception as error:
        doc['error']=repr(error);return 1
    finally:
        args.out.parent.mkdir(parents=True,exist_ok=True);args.out.write_text(json.dumps(doc,indent=2)+'\n')
        print(f"{doc['result']}: {doc.get('error','paired observations captured')}");print(args.out)


if __name__=='__main__':raise SystemExit(main())
