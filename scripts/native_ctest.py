#!/usr/bin/env python3
"""CTest bridge to existing native analysis and paired-runtime verification.

Static collection is not native acceptance. Runtime acceptance requires the
existing strict gate, candidate receipt and typed retail/candidate comparison.
"""
import re
import argparse
import ast
import datetime
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
import sys
import uuid

ROOT = Path(__file__).resolve().parents[1]


def normalize(value):
    number = int(value, 0) if isinstance(value, str) else int(value)
    if not 0 <= number <= 0xffffffff:
        raise ValueError('native address must fit Windows x86')
    return f'0x{number:08X}'


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def scope_fingerprint(root):
    paths={root/'CMakeLists.txt',root/'docs/fidelity/native-verification.json',root/'scripts/native_ctest.py'}
    for pattern in ['*.py','*.js']:
        paths.update((root/'scripts').glob(pattern))
    paths.add(root/'scripts/igi-workflow.json')
    for folder in [root/'engine',root/'cmake']:
        if folder.exists():
            paths.update(p for p in folder.rglob('*') if p.is_file() and p.suffix in {'.cpp','.c','.hpp','.h','.inl','.cmake'})
    value={p.relative_to(root).as_posix():digest(p) for p in sorted(paths) if p.is_file()}
    return hashlib.sha256(json.dumps(value,sort_keys=True).encode()).hexdigest()


def discover(root, addresses=(), scope='changed', sources=(), compile_only_sources=()):
    document = json.loads((root/'docs/fidelity/native-verification.json').read_text())
    records = {}
    for row in document['natives']:
        address = normalize(row['address'])
        if address in records:
            raise ValueError('duplicate manifest address '+address)
        records[address] = row
    selected = [normalize(x) for x in addresses]
    if len(selected) != len(set(selected)):
        raise ValueError('duplicate selected native addresses')
    changed = set()
    compile_only={Path(x).resolve().relative_to(root.resolve()).as_posix() if Path(x).is_absolute() else Path(x).as_posix() for x in compile_only_sources}
    if not selected:
        if scope == 'compiled':
            if not sources:
                raise ValueError('compiled scope requires the configured target source list')
            for value in [*sources,*compile_only_sources]:
                if '$<' in value:
                    raise ValueError('unresolved configured source expression: '+value)
                path=Path(value)
                if path.is_absolute():
                    if not path.resolve().is_relative_to(root.resolve()):
                        continue
                    path=path.resolve().relative_to(root.resolve())
                changed.add(path.as_posix())
        elif scope == 'changed':
            dependency_changes=subprocess.run(['git','diff','--name-only','-z','HEAD','--','engine/include','cmake','CMakeLists.txt'],cwd=root,capture_output=True,text=True,check=True).stdout
            if dependency_changes.strip('\0'):
                raise ValueError('header/build changes require compiled scope or an explicit reviewed native closure')
            for command in [['git','diff','--name-only','-z','HEAD','--','engine/src/retail'],
                            ['git','ls-files','--others','--exclude-standard','-z','--','engine/src/retail']]:
                run = subprocess.run(command,cwd=root,capture_output=True,text=True,check=True)
                changed.update(x for x in run.stdout.split('\0') if x)
        elif scope == 'engine':
            changed = {p.relative_to(root).as_posix() for p in (root/'engine/src/retail').rglob('*.cpp')}
        else:
            raise ValueError('unsupported native scope')
        selected = [address for address,row in records.items()
                    if row.get('implementation',{}).get('path') in changed]
    rows = []
    for address in sorted(selected):
        if address not in records:
            raise ValueError('selected native has no manifest record: '+address)
        row = records[address]
        path = row.get('implementation',{}).get('path','')
        implementation = (root/path).resolve()
        if not implementation.is_relative_to((root/'engine').resolve()) or not implementation.is_file():
            raise ValueError('native lacks a current engine implementation: '+address)
        rows.append({'address':address,'implementation':path,
                     'source_hash_matches_record':digest(implementation)==row['implementation'].get('sha256'),
                     'link_state':'compile_only' if path in compile_only else ('linked' if scope=='compiled' and sources else 'unestablished')})
    bound = {row.get('implementation',{}).get('path') for row in records.values()}
    # A source that is unbound AND defines no routine is storage: definitions only, whose
    # owning routine lives in another file (see docs/fidelity/native-verification.json). It is
    # classified, not bound, and it must not be given a record of its own -- a storage file has
    # no retail entry point, so any address written for it would be invented.
    #
    # Storage-only status is DERIVED from the file's own content, never from a trusted list. A
    # list would be a suppression mechanism: anyone could add a path and silence a genuinely
    # unclassified native while the scope reported clean. Here a file cannot be classified as
    # storage unless it actually is storage. The test fails closed in every uncertain direction
    # (unreadable file, any NATIVE_DECL mention, any ')' immediately followed by '{') so that a
    # file defining a routine can never take this path; the refusal path for such a file is
    # unchanged and as strict as before.
    unbound = sorted(x for x in changed if x.startswith('engine/src/retail/') and x.endswith('.cpp')
                     and x not in bound and defines_routine(os.path.join(root, x)))
    return {'kind':'igi-native-test-scope','natives':rows,'unbound_sources':unbound,
            'scope':scope,'verified':False,'fingerprint':scope_fingerprint(root)}



def defines_routine(path):
    """True if this translation unit defines a function body.

    Deliberately conservative and content-derived. Returns True (i.e. NOT storage) on any
    doubt: an unreadable file, any mention of NATIVE_DECL, or any ')' immediately followed by
    '{'. Storage files hold only definitions, so they contain none of those. The asymmetry is
    intentional -- a storage file left unclassified keeps blocking (safe), whereas a file that
    defines a routine being classified as storage would silently hide a real gap (unsafe).
    """
    try:
        text = open(path, errors='replace').read()
    except OSError:
        return True
    if 'NATIVE_DECL' in text:
        return True
    return re.search(r'\)\s*\{', text) is not None


def require_scope(document):
    if not document['natives']:
        raise ValueError('empty native scope cannot pass; select addresses or compiled/changed sources')
    if document['unbound_sources']:
        raise ValueError('unbound required sources need classification: '+', '.join(document['unbound_sources']))


def validate_report(phase, document, address, source_hash, binary_hash, candidate_hash=None):
    if document.get('address') != address:
        raise ValueError('backend report native address differs from selected scope')
    if document.get('binary_sha256') != binary_hash:
        raise ValueError('backend report retail binary hash differs')
    if phase == 'static':
        if document.get('kind') != 'igi-retail-analysis-capture' or document.get('result') != 'analysis_collected':
            raise ValueError('fresh static analysis did not complete')
        summary=document.get('structural_summary',{})
        if not isinstance(summary,dict):raise ValueError('invalid static structural summary')
        score=summary.get('cfg_similarity')
        if isinstance(score,bool) or not isinstance(score,(int,float)) or not math.isfinite(score) or not 0.85<=score<=1 or summary.get('angr_exact_entry') is not True:
            raise ValueError('static CFG or exact-entry analysis did not pass')
        if document.get('verified') is not False:
            raise ValueError('static analysis must not claim native acceptance')
    else:
        if document.get('kind') != 'igi-one-native-verification' or document.get('result') != 'passed' or document.get('verified') is not True:
            raise ValueError('full native acceptance did not pass')
        if document.get('implementation_sha256') != source_hash or document.get('candidate_sha256') != candidate_hash:
            raise ValueError('runtime report is not bound to the current source/candidate')
        phases=document.get('phases',{})
        for key in ['static_semantic_behavioral','paired_runtime_capture','typed_runtime_comparison']:
            if not isinstance(phases.get(key),dict) or phases[key].get('exit_code') != 0:
                raise ValueError('missing or failed runtime acceptance phase: '+key)


def invoke(command, root, timeout):
    result=subprocess.run(command,cwd=root,capture_output=True,text=True,timeout=timeout,
                          env=dict(os.environ,LC_ALL='C',LANG='C',PYTHONDONTWRITEBYTECODE='1'))
    return {'command':command,'exit_code':result.returncode,'stdout':result.stdout,'stderr':result.stderr}


def resolve_probe(root,args,address):
    if args.probe:
        probe=args.probe
    else:
        mapping=json.loads(args.probe_map.read_text()) if args.probe_map else {}
        names={normalize(k):v for k,v in mapping.items()}
        path=names.get(address)
        if path is None:
            rows=json.loads((root/'docs/fidelity/native-verification.json').read_text())['natives']
            row=next(r for r in rows if normalize(r['address'])==address)
            path=row.get('verification',{}).get('runtime_probe',{}).get('path')
        probe=Path(path) if path else None
    if probe is None:
        raise ValueError('missing reviewed typed probe for '+address+'; configure a probe/map or a source-reviewed manifest runtime_probe')
    if not probe.is_absolute():probe=root/probe
    if not probe.is_file():raise ValueError('typed probe does not exist for '+address+': '+str(probe))
    return probe.resolve()


def validate_expected_scope(scope,expected,phase):
    if expected.get('fingerprint')!=scope['fingerprint']:
        raise ValueError('configured native/source scope is stale; rerun CMake before testing')
    registered={x['address'] for x in expected['natives']}
    selected={x['address'] for x in scope['natives']}
    if (phase=='scope' and selected!=registered) or not selected.issubset(registered):
        raise ValueError('live native scope differs from configured CTest registration')


def runtime_preflight(root,config,scope):
    records=json.loads((root/'docs/fidelity/native-verification.json').read_text())['natives']
    selected={x['address'] for x in scope['natives']}
    for row in records:
        if normalize(row['address']) not in selected:continue
        runner=row.get('verification',{}).get('behavioral_runner',{}).get('path','')
        if runner:
            path=root/runner
            if not path.is_file():
                return {'exit_code':1,'error':'behavioral runner is missing: '+runner}
            try:
                tree=ast.parse(path.read_text())
            except (OSError,SyntaxError,UnicodeError):
                return {'exit_code':1,'error':'cannot inspect behavioral runner safety: '+runner}
            nodes=list(ast.walk(tree))
            imports_frida=any((isinstance(n,ast.Import) and any(x.name.split('.')[0]=='frida' for x in n.names))
                             or (isinstance(n,ast.ImportFrom) and (n.module or '').split('.')[0]=='frida') for n in nodes)
            spawns=any(isinstance(n,ast.Call) and isinstance(n.func,ast.Attribute) and n.func.attr in {'spawn','add_remote_device'} for n in nodes)
            frida_command=any(isinstance(n,ast.Constant) and isinstance(n.value,str) and ('frida-server' in n.value or 'frida-ps' in n.value) for n in nodes)
            leased=any(isinstance(n,(ast.With,ast.AsyncWith)) and any(isinstance(item.context_expr,ast.Call)
                       and isinstance(item.context_expr.func,ast.Name) and item.context_expr.func.id=='capture_lease' for item in n.items) for n in nodes)
            if (imports_frida or spawns or frida_command) and not leased:
                return {'exit_code':1,'error':'live behavioral runner must use the shared capture lease before execution: '+runner}
    code="import frida,json,sys; d=frida.get_device_manager().add_remote_device(sys.argv[1]); games=[dict(pid=p.pid,name=p.name) for p in d.enumerate_processes() if p.name.lower() in ('igi.exe','igi1.exe','open-igi.exe') or p.name.lower().startswith('open-igi-probe-')]; print(json.dumps(dict(active_games=games))); sys.exit(1 if games else 0)"
    try:
        return invoke([config['frida_python'],'-c',code,os.getenv('FRIDA_HOST',config['frida_host'])],root,20)
    except (OSError,subprocess.SubprocessError) as error:
        return {'exit_code':1,'error':str(error)}


def execute(args):
    root=args.root.resolve()
    scope=discover(root,args.address,args.scope,args.source,args.compile_only_source)
    if args.phase=='list':
        print(json.dumps(scope));return 0
    # A unique run directory prevents independent harnesses from clobbering reports.
    folder=args.output/datetime.datetime.now(datetime.timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')/uuid.uuid4().hex[:8]
    folder.mkdir(parents=True,exist_ok=False)
    report={'kind':'igi-native-ctest','phase':args.phase,'result':'blocked','verified':False,'scope':scope,'natives':[]}
    try:
        require_scope(scope)
        if args.expected_scope:
            expected=json.loads(args.expected_scope.read_text())
            validate_expected_scope(scope,expected,args.phase)
            states={x['address']:x.get('link_state','unestablished') for x in expected['natives']}
            for item in scope['natives']:item['link_state']=states[item['address']]
        if args.phase=='scope':
            report['result']='scope_resolved'
        elif args.phase=='prepare':
            for item in scope['natives']:
                if item.get('link_state')!='compile_only':resolve_probe(root,args,item['address'])
            if not args.candidate or not args.receipt or not args.build_dir:
                raise ValueError('runtime fixture requires candidate, receipt and build directory')
            command=[sys.executable,str(root/'scripts/candidate_build_receipt.py'),'--root',str(root),
                     '--build-dir',str(args.build_dir.resolve()),'--candidate',str(args.candidate.resolve()),
                     '--receipt',str(args.receipt.resolve())]
            result=invoke(command,root,args.timeout);report['fixture']=result
            if result['exit_code']!=0:raise ValueError('candidate receipt build failed')
            sys.path.insert(0,str(root/'scripts'))
            from candidate_build_receipt import validate_receipt
            validate_receipt(args.receipt,root,args.candidate,[root/x['implementation'] for x in scope['natives'] if x.get('link_state')!='compile_only'])
            report['result']='candidate_fixture_ready'
        else:
            if args.phase=='runtime':
                unlinked=[x['address'] for x in scope['natives'] if x.get('link_state')=='compile_only']
                if unlinked:raise ValueError('native is compiled as an object but not linked into the candidate: '+', '.join(unlinked))
                for label,path in [('candidate',args.candidate),('candidate receipt',args.receipt)]:
                    if path is None or not path.is_file():raise ValueError('missing '+label+'; runtime test is blocked, not skipped')
            probes={item['address']:resolve_probe(root,args,item['address']) for item in scope['natives']} if args.phase=='runtime' else {}
            config=json.loads((root/'scripts/igi-workflow.json').read_text())
            if args.phase=='runtime':
                check=runtime_preflight(root,config,scope)
                report['runtime_preflight']=check
                if check.get('exit_code')!=0:
                    raise ValueError('runtime preflight blocked: active game, unsafe runner, or unavailable Frida; no backend launched')
            binary=Path(config['retail_exe']);binary_hash=digest(binary)
            if binary_hash!=config['retail_sha256']:raise ValueError('retail executable identity differs from configuration')
            all_passed=True
            for item in scope['natives']:
                address=item['address'];source=root/item['implementation'];source_hash=digest(source)
                candidate_hash=digest(args.candidate) if args.phase=='runtime' else None
                output=folder/(address+'-backend.json')
                if args.phase=='static':
                    command=[sys.executable,str(root/'scripts/igi_workflow.py'),'analyze',address,'--report',str(output)]
                else:
                    command=[sys.executable,str(root/'scripts/smart_end_to_end_verification.py'),'--address',address,
                             '--level',str(args.level),'--candidate',str(args.candidate.resolve()),
                             '--receipt',str(args.receipt.resolve()),'--probe',str(probes[address]),
                             '--skip-build','--report',str(output)]
                entry={'address':address,'source_sha256':source_hash,'result':'blocked','backend_report':str(output)}
                try:
                    result=invoke(command,root,args.timeout);entry['command_result']=result
                    if result['exit_code']!=0:raise ValueError('backend command failed')
                    data=json.loads(output.read_text());validate_report(args.phase,data,address,source_hash,binary_hash,candidate_hash)
                    if digest(source)!=source_hash or digest(binary)!=binary_hash:
                        raise ValueError('source or retail changed during analysis')
                    if args.phase=='runtime' and digest(args.candidate)!=candidate_hash:
                        raise ValueError('candidate changed during runtime verification')
                    entry.update(result='analysis_collected' if args.phase=='static' else 'passed',backend_report=str(output))
                except (OSError,ValueError,KeyError,TypeError,subprocess.SubprocessError) as error:
                    entry['error']=str(error);all_passed=False
                report['natives'].append(entry)
            report['result']=('analysis_collected' if args.phase=='static' else 'passed') if all_passed else 'blocked'
            report['verified']=args.phase=='runtime' and all_passed
    except (OSError,ValueError,KeyError,TypeError,subprocess.SubprocessError) as error:
        report['error']=str(error)
    if report['result']!='blocked' and scope_fingerprint(root)!=scope['fingerprint']:
        report.update(result='blocked',verified=False,error='source/manifest scope changed during testing; reconfigure and rerun')
    (folder/'report.json').write_text(json.dumps(report,indent=2)+'\n')
    print(json.dumps({'result':report['result'],'verified':report['verified'],'report':str(folder/'report.json'),'error':report.get('error')},indent=2))
    return 1 if report['result']=='blocked' else 0


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('phase',choices=['list','scope','static','prepare','runtime'])
    parser.add_argument('--root',type=Path,default=ROOT)
    parser.add_argument('--address',action='append',default=[])
    parser.add_argument('--scope',choices=['changed','compiled','engine'],default='changed')
    parser.add_argument('--source',action='append',default=[])
    parser.add_argument('--compile-only-source',action='append',default=[])
    parser.add_argument('--candidate',type=Path);parser.add_argument('--receipt',type=Path)
    parser.add_argument('--build-dir',type=Path);parser.add_argument('--probe',type=Path)
    parser.add_argument('--probe-map',type=Path,help='JSON address-to-reviewed-probe mapping; otherwise use manifest runtime_probe')
    parser.add_argument('--expected-scope',type=Path)
    parser.add_argument('--level',type=int,choices=range(1,15),default=1)
    parser.add_argument('--timeout',type=int,default=1800)
    parser.add_argument('--output',type=Path,default=ROOT/'build/native-ctest')
    args=parser.parse_args()
    if args.timeout<=0:parser.error('--timeout must be positive')
    try:return execute(args)
    except (OSError,ValueError,KeyError,TypeError,subprocess.SubprocessError) as error:
        print('BLOCKED:',error,file=sys.stderr);return 1

if __name__=='__main__':raise SystemExit(main())
