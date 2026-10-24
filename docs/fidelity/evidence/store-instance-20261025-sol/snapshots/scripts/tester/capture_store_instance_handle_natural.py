#!/usr/bin/env python3
"""Observe the second WinMain native naturally in retail then the new x86 game."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys
import signal

ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'scripts'))
from candidate_build_receipt import validate_receipt
from frida_capture_lease import capture_lease
import frida

def sha(path): return hashlib.sha256(Path(path).read_bytes()).hexdigest()

def compare(reports):
    rows={}; checks=[]
    for side,r in reports.items():
        terminal=[e for e in r.get('events',[]) if e.get('type')=='store_instance_handle_complete']
        valid=(r.get('result')=='observed' and not r.get('failures') and
               r.get('cleanup',{}).get('owned_process_exited') is True and
               len(r.get('entries',[]))==len(r.get('returns',[]))==len(terminal)==1 and
               terminal[0].get('entries')==terminal[0].get('returns')==1)
        checks.append({'side':side,'check':'one complete natural call and owned-process cleanup','passed':valid})
        if not valid:continue
        entry,leave=r['entries'][0],r['returns'][0]
        before,after=entry['neighborsBefore'],leave['neighborsAfter']
        rows[side]=dict(inputIsModuleBase=entry['argumentWord']==entry['moduleBaseWord'],globalBefore=entry['globalBefore'],
            returnedInput=leave['returnWord']==entry['argumentWord'],
            storedInput=leave['globalAfter']==entry['argumentWord'],
            inputStillModuleBase=leave['inputStillModuleBase'],
            neighborsUnchanged=before[:8]==after[:8] and before[16:]==after[16:])
        checks.append({'side':side,'check':'instance-handle store/return and unchanged sampled neighboring bytes',
                       'passed':rows[side]['globalBefore']==0 and all(rows[side][k] for k in
                        ['inputIsModuleBase','returnedInput','storedInput','inputStillModuleBase','neighborsUnchanged'])})
    checks.append({'check':'same complete normalized observations','passed':len(rows)==2 and rows['retail']==rows['candidate']})
    return dict(kind='igi-store-instance-handle-natural-pair',verified=False,retail_va='0x0048F0D0',
        observations_match=all(c['passed'] for c in checks),checks=checks,observations=rows,
        scope='Second natural WinMain StoreInstanceHandle invocation, window level1',
        limits=['Full WinMain later stops at the untranscribed 0x00402930 startup sequence.',
                'Only the observed valid instance-handle word is runtime-tested; no universal pointer-domain proof.'])

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--out',type=Path,required=True)
    ap.add_argument('--use-built-candidate',action='store_true',
                    help='Use the same freshly built image after validating its complete receipt')
    args=ap.parse_args()
    args.out=args.out.resolve();args.out.mkdir(parents=True,exist_ok=False)
    retail=Path('/Users/haseeb-mir/Downloads/IGI/igi.exe');build=ROOT/'build-first-native-sol'
    candidate=build/'igi1.exe';receipt=build/'diagnostic-receipt.json';source=ROOT/'engine/src/retail/core/app_startup_natives.cpp'
    probe=ROOT/'scripts/tester/frida_store_instance_handle_natural.js';driver=ROOT/'scripts/tester/frida_probe_driver.py'
    identity={str(p):sha(p) for p in [retail,source,probe,driver,Path(__file__)]}
    if identity[str(retail)]!='dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01':raise ValueError('retail mismatch')
    d=frida.get_device_manager().add_remote_device('127.0.0.1:27042')
    games=lambda dev:[p.name for p in dev.enumerate_processes() if p.name.lower() in ('igi.exe','igi1.exe','open-igi.exe') or p.name.lower().startswith('open-igi-probe-')]
    reports={}
    with capture_lease(d,games,timeout=2):
        for side,image,entry,state in [('retail',retail,0x48f0d0,0x5c8bc0),('candidate',candidate,None,None)]:
            if side=='candidate':
                if not args.use_built_candidate:
                    command=[sys.executable,str(ROOT/'scripts/candidate_build_receipt.py'),'--root',str(ROOT),'--build-dir',str(build),'--receipt',str(receipt)]
                    with (args.out/'build.log').open('w') as log:built=subprocess.run(command,stdout=log,stderr=subprocess.STDOUT,timeout=180)
                    if built.returncode:raise RuntimeError('candidate clean build failed')
                validate_receipt(receipt,ROOT,candidate,required_sources=[source])
                symbols=subprocess.check_output(['i686-w64-mingw32-nm',str(candidate)],text=True)
                table={p[2]:int(p[0],16) for line in symbols.splitlines() if len(p:=line.split())==3}
                entry,state=table['_StoreInstanceHandle'],table['_InstanceHandle']
                identity.update({str(candidate):sha(candidate),str(receipt):sha(receipt)})
            command=[sys.executable,str(driver),'--exe',str(image),'--probe',str(probe),'--target',hex(entry),
                     '--name','StoreInstanceHandle','--native-size','10','--arg-words','1',
                     '--watch',f'{hex(state)}:0:4','--observe','5','--level','1',
                     '--expect-exe-sha256',sha(image),'--out',str(args.out/(side+'.json'))]
            process=subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
            try:stdout,stderr=process.communicate(timeout=40)
            except subprocess.TimeoutExpired:
                journal=args.out/(side+'.owned.json')
                if journal.is_file():
                    owned=json.loads(journal.read_text());matching=[p for p in d.enumerate_processes() if p.pid==owned['pid']]
                    if (matching and owned['executable']['sha256']==sha(image) and
                        matching[0].name.lower()==image.name.lower()):d.kill(owned['pid'])
                process.send_signal(signal.SIGINT);stdout,stderr=process.communicate(timeout=20)
                (args.out/(side+'-driver.log')).write_text(stdout+stderr)
                raise RuntimeError(side+' capture timeout; rejected')
            (args.out/(side+'-driver.log')).write_text(stdout+stderr)
            if process.returncode or games(d):raise RuntimeError(side+' capture/cleanup failed')
            reports[side]=json.loads((args.out/(side+'.json')).read_text())
        validate_receipt(receipt,ROOT,candidate,required_sources=[source])
        for path,value in identity.items():
            if sha(path)!=value:raise ValueError('capture input changed: '+path)
    report=compare(reports);report['identities']=identity
    (args.out/'comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
    return 0 if report['observations_match'] else 1

if __name__=='__main__':raise SystemExit(main())
