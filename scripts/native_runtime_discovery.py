#!/usr/bin/env python3
"""Generate natural-call probes and attempt live paired discovery, never acceptance.

Unknown ABI, argument meanings and side effects stay unresolved. Generated probes
are observations for writing the typed contract, not automatically reviewed code.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import signal
import struct
from contextlib import ExitStack
from native_test_config import load_config
from native_test_runtime import runtime_endpoint
import subprocess
import sys
from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
from verification_scope import stage_candidate

ROOT = Path(__file__).resolve().parents[1]


def sha(p):
    return hashlib.sha256(p.read_bytes()).hexdigest()


def image_bytes(path,va,length):
    data=path.read_bytes();pe=struct.unpack_from('<I',data,0x3c)[0]
    count=struct.unpack_from('<H',data,pe+6)[0]
    optional=struct.unpack_from('<H',data,pe+20)[0]
    if struct.unpack_from('<H',data,pe+24)[0]!=0x10b:raise ValueError('expected x86 PE')
    base=struct.unpack_from('<I',data,pe+52)[0];rva=va-base
    for i in range(count):
        section=pe+24+optional+i*40
        virtual_size,start,size,offset=struct.unpack_from('<IIII',data,section+8)
        if start<=rva and rva+length<=start+size:
            return data[offset+rva-start:offset+rva-start+length]
    raise ValueError('native bytes outside PE file sections')


def getter_probe(retail,candidate,va,candidate_va):
    """Generate typed probes only for an exact two-load/ret machine-code rule."""
    left=image_bytes(retail,va,9);right=image_bytes(candidate,candidate_va,9)
    if not (left[0]==right[0]==0xa1 and left[5:7]==right[5:7]==bytes.fromhex('8b40')
            and left[8]==right[8]==0xc3 and left[7]==right[7]):
        return None
    globals=[struct.unpack_from('<I',b,1)[0] for b in (left,right)]
    offset=left[7]
    # disp8 >=128 is signed: reject rather than infer an object before its base.
    if offset>=128:return None
    proof=dict(rule='x86-a1-8b40-disp8-c3',retail_bytes=left.hex(),candidate_bytes=right.hex(),
               field_offset=offset,access_width=4,no_arguments=True,no_callees=True,
               writes_in_native=False,scope='two-load-u32-getter')
    code="""'use strict';
const VA=ADDRESS,GLOBALS=GLOBAL_MAP,OFFSET=FIELD;
let hook,entries=0,returns=0;
function value(global){
 const context=global.readPointer(),field=context.add(OFFSET),range=Process.findRangeByAddress(field);
 if(!range || !range.protection.includes('r') || field.add(4).compare(range.base.add(range.size))>0)
  throw new Error('evidenced DWORD field unreadable');
 return field.readU32();
}
rpc.exports={hook(address,layout){
 const image=Process.getModuleByName(layout.moduleName);
 const relocate=x=>image.base.add(ptr(x).sub(ptr(layout.preferredBase)));
 const global=relocate(image.name.toLowerCase()==='igi.exe'?GLOBALS[0]:GLOBALS[1]);
 hook=Interceptor.attach(relocate(address),{
  onEnter(){this.index=++entries;this.before=value(global);send({type:'observed',phase:'entry',address:VA,callIndex:this.index,fieldBefore:this.before});},
  onLeave(ret){returns++;send({type:'observed',phase:'return',address:VA,callIndex:this.index,
   typedState:{scope:'two-load-u32-getter',inputs:{field_before:this.before},
    outputs:{return_value:ret.toUInt32()},sideEffects:{field_after:value(global)}}});}
 });return {installed:true,address:VA};
},summary(){return {entries,returns};},detach(){if(hook)hook.detach();}};
""".replace('ADDRESS',json.dumps(f'0x{va:08X}')).replace('GLOBAL_MAP',json.dumps(globals)).replace('FIELD',str(offset))
    return code,proof


def compare_captures(folder,typed):
    reports=[json.loads((folder/(side+'.json')).read_text()) for side in ('retail','candidate')]
    checks=[]
    for side,report in zip(('retail','candidate'),reports):
        entries,returns=report.get('entries',[]),report.get('returns',[])
        complete=bool(entries) and len(entries)==len(returns)
        checks.append(dict(side=side,check='complete_observed_calls',passed=complete))
        if typed and complete:
            for enter,leave in zip(entries,returns):
                state=leave.get('typedState',{})
                before=state.get('inputs',{}).get('field_before')
                checks.append(dict(side=side,check='getter_input_return_and_unchanged_field',
                    passed=enter.get('callIndex')==leave.get('callIndex') and before is not None
                    and before==enter.get('fieldBefore')==state.get('outputs',{}).get('return_value')
                    and before==state.get('sideEffects',{}).get('field_after')))
    if typed:
        sequences=[[r.get('typedState') for r in report.get('returns',[])] for report in reports]
        checks.append(dict(check='same_ordered_inputs_outputs_and_field_effects',passed=bool(sequences[0]) and sequences[0]==sequences[1]))
    return dict(kind='igi-generated-probe-comparison',verified=False,
                result='observations_match' if typed and checks and all(c['passed'] for c in checks) else 'inconclusive_or_mismatched',
                checks=checks,typed_probe=typed,
                limitation='Generated comparison never bypasses the strict source-semantic acceptance gate.')


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--address', required=True)
    ap.add_argument('--level', type=int, default=1, choices=range(1,15))
    ap.add_argument('--out', type=Path, required=True)
    args = ap.parse_args()
    cfg = load_config(ROOT)
    cfg['frida_host'] = os.environ.get('FRIDA_HOST', cfg['frida_host'])
    try:
        import frida
    except ImportError:
        os.execv(cfg['frida_python'], [cfg['frida_python'], str(Path(__file__).resolve()), *sys.argv[1:]])
    va = f'0x{int(args.address,0):08X}'
    record = next(r for r in json.loads((ROOT / 'docs/fidelity/native-verification.json').read_text())['natives'] if int(r['address'],16)==int(va,16))
    folder = args.out.resolve(); folder.mkdir(parents=True,exist_ok=True)
    # No guessed parameter count, pointer dereferences or NativeFunction calls.
    probe = folder / 'generated-probe.js'
    probe.write_text("""'use strict';
let listener, entries=0, returns=0;
rpc.exports={hook(address,layout){
 const image=Process.getModuleByName(layout.moduleName);
 const target=image.base.add(ptr(address).sub(ptr(layout.preferredBase)));
 listener=Interceptor.attach(target,{
  onEnter(){this.index=++entries;send({type:'observed',phase:'entry',address:VA,callIndex:this.index,abi:'unresolved',argumentsObserved:false});},
  onLeave(ret){returns++;send({type:'observed',phase:'return',address:VA,callIndex:this.index,eaxWord:ret.toUInt32(),returnMeaning:'unresolved',sideEffectsObserved:false});}
 });return {installed:true,address:VA,target:target.toString()};
},summary(){return {entries,returns,verified:false};},detach(){if(listener)listener.detach();}};
""".replace('VA',json.dumps(va)))
    result = dict(kind='igi-generated-runtime-discovery',address=va,verified=False,
                  result='blocked',probe_sha256=sha(probe),captures={},
                  limitations=['Generated natural entry/return observation only; arguments and effects unresolved.',
                               'No automatic verification promotion. Typed semantic contract still required.'])
    runtime = ExitStack()
    try:
        candidate = ROOT / cfg['candidate']; retail = Path(cfg['retail_exe'])
        source = ROOT / record['implementation']['path']
        receipt = ROOT / cfg['build_directory'] / 'candidate-receipt.json'
        try:
            validate_receipt(receipt,ROOT,candidate,[source])
        except (ValueError,OSError,KeyError):
            with (folder/'configure.log').open('w') as log:
                configured=subprocess.run(['cmake','-S',str(ROOT),'-B',str(ROOT/cfg['build_directory']),
                    '-DCMAKE_SYSTEM_NAME=Windows','-DCMAKE_CXX_COMPILER=i686-w64-mingw32-g++',
                    '-DCMAKE_BUILD_TYPE=Release'],stdout=log,stderr=subprocess.STDOUT,timeout=120)
            if configured.returncode:raise ValueError('dedicated test build configuration failed')
            with (folder/'build.log').open('w') as log:
                built=subprocess.run([sys.executable,str(ROOT/'scripts/candidate_build_receipt.py'),
                    '--root',str(ROOT),'--build-dir',str(ROOT/cfg['build_directory']),
                    '--receipt',str(receipt)],stdout=log,stderr=subprocess.STDOUT,timeout=180)
            if built.returncode: raise ValueError('fresh candidate build/receipt failed; see build.log')
            validate_receipt(receipt,ROOT,candidate,[source])
        if sha(retail)!=cfg['retail_sha256']: raise ValueError('retail identity mismatch')
        symbols = subprocess.check_output(['i686-w64-mingw32-nm','--defined-only',str(candidate)],text=True,timeout=30)
        symbols = {v[2]:int(v[0],16) for line in symbols.splitlines() if len(v:=line.split())==3}
        symbol = record.get('verification',{}).get('candidate_symbol','_'+record['name'])
        if symbol not in symbols: raise ValueError('candidate native symbol missing: '+symbol)
        generated=getter_probe(retail,candidate,int(va,16),symbols[symbol])
        if not generated:
            try:
                from native_probe_agent import generate_probe
                generated=generate_probe(ROOT,folder/'probe-agent',va,source,retail,candidate,symbols[symbol],symbols)
            except (ValueError,OSError,KeyError,subprocess.SubprocessError) as error:
                result['probe_agent_error']=str(error)
        if generated:
            code,proof=generated;probe.write_text(code)
            result['generated_contract']=proof
            result['probe_sha256']=sha(probe)
        result.update(candidate_sha256=sha(candidate),source_sha256=sha(source),retail_sha256=sha(retail))
        cfg['frida_host']=runtime.enter_context(runtime_endpoint(cfg,folder))
        def preflight_timeout(signum, frame):
            raise TimeoutError('Frida connection/process preflight exceeded 15 seconds; no game launched')
        signal.signal(signal.SIGALRM, preflight_timeout)
        signal.alarm(15)
        device=frida.get_device_manager().add_remote_device(cfg['frida_host'])
        games=lambda d:[p.name for p in d.enumerate_processes() if p.name.lower() in ('igi.exe','igi1.exe','open-igi.exe') or p.name.lower().startswith('open-igi-probe-')]
        with capture_lease(device,games,timeout=2), stage_candidate(candidate,retail.parent) as staged:
            signal.alarm(0)
            if sha(staged) != result["candidate_sha256"]:
                raise ValueError("candidate changed before staging")
            result["staged_candidate_sha256"] = sha(staged)
            for side,image,target in [('retail',retail,int(va,16)),('candidate',staged,symbols[symbol])]:
                output=folder/(side+'.json')
                command=[sys.executable,str(ROOT/'scripts/tester/frida_probe_driver.py'),
                         '--exe',str(image),'--hook',f'{probe}:{va}:{record["name"]}:{hex(target)}',
                         '--level',str(args.level),'--observe','20','--startup-escape','--arg-words','0',
                         '--expect-exe-sha256',sha(image),'--out',str(output)]
                with (folder/(side+'.log')).open('w') as log:
                    process=subprocess.Popen(command,stdout=log,stderr=subprocess.STDOUT,env=dict(os.environ,FRIDA_HOST=cfg['frida_host']))
                    try: code=process.wait(timeout=55)
                    except subprocess.TimeoutExpired:
                        journal=output.with_suffix('.owned.json')
                        if journal.is_file():
                            owned=json.loads(journal.read_text())
                            if owned['executable']['sha256']==sha(image):
                                matching=[p for p in device.enumerate_processes() if p.pid==owned['pid'] and p.name.lower()==image.name.lower()]
                                if matching:device.kill(owned['pid'])
                        process.send_signal(signal.SIGINT)
                        try:process.wait(timeout=10)
                        except subprocess.TimeoutExpired:process.kill();process.wait()
                        raise ValueError(side+' live capture timed out')
                captured=json.loads(output.read_text()) if output.is_file() else {}
                result['captures'][side]=dict(exit_code=code,report=str(output),
                    capture_result=captured.get('result'),entries=len(captured.get('entries',[])),
                    returns=len(captured.get('returns',[])),cleanup=captured.get('cleanup'))
                if games(device): raise ValueError('owned capture cleanup incomplete; second game not launched')
            comparison=compare_captures(folder,result.get('generated_contract',{}).get('rule')=='x86-a1-8b40-disp8-c3')
            (folder/'comparison.json').write_text(json.dumps(comparison,indent=2)+'\n')
            result['comparison']=comparison
            validate_receipt(receipt,ROOT,candidate,[source])
        result['result']='observations_collected'
        result['paired_complete_calls']=all(c.get('entries',0)>0 and c['entries']==c['returns']
                                           for c in result['captures'].values())
    except Exception as error:
        result['error']=str(error)
    runtime.close()
    signal.alarm(0)
    (folder/'result.json').write_text(json.dumps(result,indent=2)+'\n')
    print(json.dumps(result,indent=2))
    return 0 if result['result']=='observations_collected' else 1

if __name__=='__main__':raise SystemExit(main())
