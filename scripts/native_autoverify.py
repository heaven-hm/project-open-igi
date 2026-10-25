#!/usr/bin/env python3
"""Persistent change detection and sequential strict native verification.

Inventory is not acceptance. Only the existing source-bound, typed, paired
retail/candidate verifier can pass a task. This program never edits statuses.
"""
import argparse
from contextlib import contextmanager
from datetime import datetime, timezone
import fcntl
import hashlib
import json
import math
import os
from native_test_config import load_config
from pathlib import Path
import re
import signal
import subprocess
import sys
import time
import uuid

ROOT = Path(__file__).resolve().parents[1]
SUFFIXES = {'.cpp', '.c', '.cc', '.hpp', '.h', '.inl', '.cmake'}
# This exact module only adds explicit verifier targets; it does not alter igi's
# sources, flags or link. Any other module revision is a native build change.
VERIFIER_TARGET_SHA256 = '09eb0c7cef47013a0709422316da9f6b35465168e5252ef4067de4fd5083b465'


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def save(path, value, *, sort_keys=True):
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + '.' + uuid.uuid4().hex + '.tmp')
    temporary.write_text(json.dumps(value, indent=2, sort_keys=sort_keys) + '\n')
    os.replace(temporary, path)


def git(root, *args):
    return subprocess.check_output(['git', *args], cwd=root).decode().split('\0')


def intake_candidates(root, paths, addresses):
    """Register unique provenance-bound definitions, never verification claims."""
    from native_gate import implementation_natives, normalize_address
    manifest=root/'docs/fidelity/native-verification.json'
    original=manifest.read_bytes()
    document=json.loads(original)
    records={}
    for row in document['natives']:
        records.setdefault(normalize_address(row['address']),[]).append(row)
    definitions={}
    for path in sorted((root/'engine/src/retail').rglob('*.cpp')):
        relative=str(path.relative_to(root))
        for native in implementation_natives(path,relative):
            definitions.setdefault(native['address'],[]).append((path,relative,native))
    added=[]
    for address, entries in sorted(definitions.items()):
        # Scan the entire implementation tree for duplicates before scoping.
        # No guessed names, filenames-as-addresses or conflict overwrites.
        if len(entries)!=1 or len(records.get(address,[]))>1:
            continue
        path,relative,native=entries[0]
        if addresses:
            if address not in addresses:continue
        elif paths is not None and relative not in paths:
            continue
        previous=records.get(address,[None])[0]
        if previous and previous.get('implementation',{}).get('path','').startswith('engine/'):
            continue
        identifier=re.match(r'([A-Za-z_]\w*)(?=\s|;|\(|$)',native['name'])
        if not identifier:continue
        name=identifier[1]
        # A comment alone is not an implementation or a resolved ABI.
        body=path.read_text()
        code=re.sub(r'/\*.*?\*/|//[^\n]*','',body,flags=re.S)
        if not re.search(r'\b'+re.escape(name)+r'\s*\([^;{}]*\)\s*(?:noexcept\s*)?\{',code):
            continue
        artifact=re.search(r'Artifact:.*?SHA256\s+([0-9a-fA-F]{64})',body)
        expected=document.get('binary_sha256')
        if artifact and expected and artifact[1].lower()!=expected.lower():
            continue
        row=dict(address=address,name=name,binary_sha256=expected,
                 implementation=dict(path=relative,sha256=digest(path)),
                 verification=dict(status='TRANSCRIBED',selected_decompiler='binary_ninja',
                     candidate_symbol='_'+name,
                     limitations=['Automatically registered from a unique source provenance VA and definition; ABI, static semantics and paired runtime remain unverified.']))
        if previous:
            archive=root/'build/native-auto/intake'/('prior-'+address+'-'+hashlib.sha256(json.dumps(previous,sort_keys=True).encode()).hexdigest()+'.json')
            save(archive,previous)
            row['verification']['prior_catalog_record']=dict(path=str(archive.relative_to(root)),sha256=digest(archive))
            document['natives'][document['natives'].index(previous)]=row
        else:
            document['natives'].append(row)
        added.append(dict(address=address,path=relative,name=name,status='TRANSCRIBED'))
    if added:
        if manifest.read_bytes()!=original:
            raise ValueError('manifest changed during intake; retry without overwriting concurrent edits')
        save(manifest,document,sort_keys=False)
        save(root/'build/native-auto/intake/latest.json',dict(kind='igi-native-candidate-intake',
             verified=False,registered=added))
    return added


def inventory(root):
    from native_gate import implementation_natives, normalize_address
    manifest = root / 'docs/fidelity/native-verification.json'
    document = json.loads(manifest.read_text())
    records, issues = {}, []
    for record in document['natives']:
        va = normalize_address(record['address'])
        if va in records:
            issues.append({'address': va, 'error': 'duplicate manifest address'})
        records[va] = record
    bound = {}
    for va, record in records.items():
        name = record.get('implementation', {}).get('path', '')
        if name.startswith('engine/'):
            bound.setdefault(name, []).append(va)
    files = {root / 'CMakeLists.txt', manifest, root / 'scripts/igi-workflow.json'}
    for folder in ('engine', 'cmake'):
        files.update(p for p in (root / folder).rglob('*') if p.is_file() and
                     (p.suffix in SUFFIXES or p.name == 'CMakeLists.txt'))
    # Tool/probe edits also invalidate saved results, including custom probes.
    files.update(p for p in (root / 'scripts').rglob('*') if p.is_file() and
                 p.suffix in {'.py', '.js'} and 'GameNatives' not in p.parts)
    context = {str(p.relative_to(root)): digest(p) for p in sorted(files) if p.is_file()}
    context_digest = hashlib.sha256(json.dumps(context, sort_keys=True).encode()).hexdigest()
    jobs = {}
    for name, addresses in sorted(bound.items()):
        path = root / name
        if not path.is_file():
            issues.append({'path': name, 'error': 'recorded engine source is missing'})
            continue
        for va in addresses:
            row = records[va]
            references = {}
            def visit(value):
                if isinstance(value, dict):
                    for key, item in value.items():
                        if key in {'evidence_path', 'path', 'probe_path', 'actions_path', 'authority'} and isinstance(item, str):
                            target = (root / item).resolve()
                            if target.is_relative_to(root) and target.is_file():
                                references[item] = digest(target)
                        visit(item)
                elif isinstance(value, list):
                    for item in value:
                        visit(item)
            visit(row)
            fingerprint = hashlib.sha256(json.dumps([context_digest, row, references], sort_keys=True).encode()).hexdigest()
            jobs[va] = {'address': va, 'name': row['name'], 'source': name,
                        'source_sha256': digest(path), 'fingerprint': fingerprint,
                        'status': 'pending', 'verified': False}
    for path in sorted((root / 'engine/src/retail').rglob('*.cpp')):
        name = str(path.relative_to(root))
        declared = implementation_natives(path, name)
        if name not in bound:
            issues.append({'path': name, 'error': 'unbound engine source; retail address/classification required'})
        for native in declared:
            va = native['address']
            if va not in records or records[va].get('implementation', {}).get('path') != name:
                issues.append({'path': name, 'address': va, 'error': 'native definition is unregistered or bound to a different source'})
    for path in sorted((root / 'engine/include/igi/retail').rglob('*')):
        if path.is_file() and path.suffix in {'.h', '.hpp', '.inl'}:
            name = str(path.relative_to(root))
            declared = implementation_natives(path, name)
            for native in declared:
                if native['address'] not in records:
                    issues.append({'path':name, 'address':native['address'], 'error':'unregistered native address in header'})
    return jobs, records, issues, context_digest


def selected_paths(root, scope, base=None, head='HEAD'):
    if scope == 'all':
        return None
    if scope == 'staged':
        return {p for p in git(root, 'diff', '--cached', '--name-only', '-z') if p}
    if scope == 'range':
        if not base or set(base) == {'0'}:
            raise ValueError('range scope requires a real base commit; use all for the first push')
        actual = subprocess.check_output(['git','rev-parse','HEAD'],cwd=root)
        selected = subprocess.check_output(['git','rev-parse',head],cwd=root)
        if actual != selected:
            raise ValueError('range head must be the checked-out commit')
        return {p for p in git(root, 'diff', '--name-only', '-z', base, head) if p}
    return {p for command in [('diff', '--name-only', '-z', 'HEAD'),
                              ('ls-files', '--others', '--exclude-standard', '-z')]
            for p in git(root, *command) if p}


def select(root, jobs, paths, addresses):
    if addresses:
        absent = set(addresses) - set(jobs)
        if absent:
            raise ValueError('addresses have no engine source: ' + ', '.join(sorted(absent)))
        return set(addresses)
    # Headers/build definitions can affect every linked native. Never silently
    # pretend the edited .cpp files alone are the complete dependency closure.
    dependencies = set() if paths is None else {p for p in paths if p.startswith(('engine/include/', 'cmake/')) or p == 'CMakeLists.txt'}
    if dependencies and dependencies <= {'CMakeLists.txt', 'cmake/NativeAutoVerification.cmake'}:
        module = root / 'cmake/NativeAutoVerification.cmake'
        previous = subprocess.run(['git','show','HEAD:CMakeLists.txt'],cwd=root,capture_output=True,text=True)
        include = 'include(cmake/NativeAutoVerification.cmake)\n'
        if (module.is_file() and digest(module) == VERIFIER_TARGET_SHA256 and previous.returncode == 0
                and previous.stdout.replace(include,'') == (root/'CMakeLists.txt').read_text().replace(include,'')):
            dependencies.clear()
    if paths is None or dependencies:
        return set(jobs)
    return {va for va, job in jobs.items() if job['source'] in paths}


def complete_report(evidence, job, config, candidate_hash):
    phases = evidence.get('phases', {})
    required = ('static_semantic_behavioral', 'paired_runtime_capture', 'typed_runtime_comparison')
    return (evidence.get('schema_version') == 1 and evidence.get('kind') == 'igi-one-native-verification'
        and evidence.get('verified') is True and evidence.get('result') == 'passed'
        and evidence.get('address') == job['address']
        and evidence.get('implementation_sha256') == job['source_sha256']
        and evidence.get('binary_sha256') == config.get('retail_sha256')
        and bool(candidate_hash) and evidence.get('candidate_sha256') == candidate_hash
        and all(isinstance(phases.get(key), dict) and type(phases[key].get('exit_code')) is int
                and phases[key]['exit_code'] == 0 for key in required))


def work_order(root, addresses):
    ranks = {'0x0048F530': 0}
    path = root / 'docs/fidelity/evidence/chronological-native-order.md'
    if path.is_file():
        for line in path.read_text().splitlines():
            match = re.match(r'\|\s*\d+\s*\|\s*`(0x[0-9a-fA-F]+)`', line)
            if match:
                va = f'0x{int(match.group(1),16):08X}'
                ranks.setdefault(va, len(ranks))
    # Indirect/script-only entries not reached by the direct graph remain in
    # the queue; numeric order here is a fallback, not a runtime-flow claim.
    return sorted(addresses, key=lambda va:(ranks.get(va,len(ranks)),int(va,16)))


def execute(root, command, folder, label, timeout):
    log = folder / (label + '.log')
    started = time.monotonic()
    with log.open('w') as stream:
        process = subprocess.Popen(command, cwd=root, stdout=stream, stderr=subprocess.STDOUT,
            start_new_session=True, env=dict(os.environ, PYTHONDONTWRITEBYTECODE='1'))
        try:
            code = process.wait(timeout=timeout)
        except (subprocess.TimeoutExpired, KeyboardInterrupt):
            # Give owned Python capture children their finally cleanup path.
            os.killpg(process.pid, signal.SIGINT)
            try:
                process.wait(timeout=20)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)
                process.wait()
            raise
    return {'command': command, 'exit_code': code, 'log': str(log),
            'log_sha256': digest(log), 'seconds': round(time.monotonic() - started, 2)}


def verify_once(root, job, record, folder, args):
    folder.mkdir(parents=True)
    result = dict(job, status='blocked', verified=False, phases=[])
    try:
        static = folder / 'static.json'
        phase = execute(root, [sys.executable, str(root / 'scripts/igi_workflow.py'), 'analyze',
            job['address'], '--report', str(static)], folder, 'static', args.timeout)
        result['phases'].append(phase)
        if phase['exit_code']:
            raise ValueError('selected-decompiler/r2/angr analysis failed')
        capture = json.loads(static.read_text())
        summary = capture.get('structural_summary', {})
        score = summary.get('cfg_similarity', 0)
        if (capture.get('result') != 'analysis_collected' or isinstance(score, bool) or
                not isinstance(score, (int, float)) or score < .85
                or summary.get('angr_exact_entry') is not True or summary.get('entry_bytes_match') is not True):
            raise ValueError('static entry/bytes/CFG comparison incomplete or below 0.85')
        # A custom reviewed typed probe is mandatory. Do not synthesize raw-word
        # probes or guessed pointer dereferences to make an intake turn green.
        probe_name = record.get('verification', {}).get('runtime_probe', {}).get('path')
        if not probe_name or not (root / probe_name).is_file():
            # Generate and attempt live evidence discovery instead of stopping
            # before runtime. Discovery never substitutes for typed acceptance.
            phase = execute(root, [sys.executable, str(root / 'scripts/native_runtime_discovery.py'),
                '--address', job['address'], '--level', str(args.level),
                '--out', str(folder / 'runtime-discovery')], folder, 'runtime-discovery', args.timeout)
            result['phases'].append(phase)
            discovery_path = folder / 'runtime-discovery/result.json'
            discovery = json.loads(discovery_path.read_text()) if discovery_path.is_file() else {}
            if phase['exit_code'] or discovery.get('result') != 'observations_collected':
                raise ValueError('generated runtime capture failed: ' + discovery.get('error', 'see runtime-discovery.log'))
            raise ValueError('live discovery collected; typed source-semantic side-effect comparison remains incomplete; see runtime-discovery/result.json')
        if digest(root / job['source']) != record['implementation'].get('sha256'):
            raise ValueError('source differs from reviewed record; semantic review required')
        full = folder / 'end-to-end.json'
        command = [sys.executable, str(root / 'scripts/smart_end_to_end_verification.py'),
            '--address', job['address'], '--probe', str(root / probe_name), '--level', str(args.level),
            '--report', str(full)]
        # Reuse one unchanged freshly compiled candidate across a batch. A
        # separate rebuild per native would invalidate the preceding image.
        from candidate_build_receipt import validate_receipt
        config = load_config(root)
        candidate = root / config['candidate']
        receipt = root / config['build_directory'] / 'candidate-receipt.json'
        try:
            validate_receipt(receipt, root, candidate, [root / job['source']])
        except (OSError, ValueError, KeyError):
            pass
        else:
            command += ['--skip-build', '--candidate', str(candidate), '--receipt', str(receipt)]
        phase = execute(root, command, folder, 'end-to-end', args.timeout)
        result['phases'].append(phase)
        evidence = json.loads(full.read_text()) if full.is_file() else {}
        result['diagnostic_report']=str(full)
        result['failure_details']='\n'.join(str(p.get('stdout',''))+str(p.get('stderr','')) for p in evidence.get('phases',{}).values())[-10000:]
        if phase['exit_code'] or not complete_report(evidence, job, config,
                digest(candidate) if candidate.is_file() else None):
            raise ValueError('strict semantic/behavioral/build/paired-runtime verification did not pass')
        # The full verifier checks all inputs, outputs and declared side effects,
        # candidate receipt and source/image hashes. Save its exact report hash.
        result.update(status='passed', verified=True, report=str(full), report_sha256=digest(full),
                      candidate_sha256=evidence['candidate_sha256'])
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        result['error'] = str(error)
    save(folder / 'result.json', result)
    return result


def repair_annotation_source(root,job,record,result):
    if 'source differs from reviewed record' not in result.get('error',''):
        return None
    from native_source_review import refresh_review
    try:
        refreshed=refresh_review(root,record)
    except (OSError,ValueError,KeyError,subprocess.SubprocessError) as error:
        result['source_review_repair_error']=str(error)
        return None
    if not refreshed:return None
    manifest=root/'docs/fidelity/native-verification.json';original=manifest.read_bytes()
    document=json.loads(original)
    matches=[row for row in document['natives'] if row['address']==job['address']]
    if len(matches)!=1 or matches[0]!=record:return None
    if digest(root/job['source'])!=refreshed['source_sha256']:return None
    updated=json.loads(json.dumps(record))
    updated['implementation']['sha256']=refreshed['source_sha256']
    header=updated['implementation'].get('header',{})
    if header.get('path') in refreshed['headers']:
        header['sha256']=refreshed['headers'][header['path']]
    updated['verification']['semantic_review']=refreshed['semantic_review']
    updated['verification']['source_identity_proof']=refreshed['proof']
    updated['verification'].setdefault('limitations',[]).append('Only Verified annotation positions changed; previous runtime artifacts remain historical. Fresh build/live comparison still required.')
    document['natives'][document['natives'].index(matches[0])]=updated
    if manifest.read_bytes()!=original:return None
    save(manifest,document,sort_keys=False)
    record.clear();record.update(updated)
    fresh,_,_,context=inventory(root)
    job.update(fresh[job['address']])
    return context


def repair_scope_path(root,job,record,result):
    """Relocate an intact scope document after the gate reports a path error."""
    if 'path must be under docs/fidelity/scopes/' not in result.get('failure_details',''):
        return None
    jobs,_,issues,_=inventory(root)
    current=jobs.get(job['address'],{})
    if current.get('source_sha256')!=record.get('implementation',{}).get('sha256'):
        return None
    if any(i.get('address')==job['address'] or i.get('path')==job['source'] for i in issues):
        return None
    reference=record.get('verification',{}).get('scope',{})
    source=(root/reference.get('path','')).resolve()
    if not source.is_relative_to((root/'docs/fidelity').resolve()) or not source.is_file():
        return None
    expected=reference.get('sha256')
    if digest(source)!=expected:return None
    target=root/'docs/fidelity/scopes'/('auto-'+job['address']+'-'+expected[:16]+'.md')
    target.parent.mkdir(parents=True,exist_ok=True)
    if target.is_file() and digest(target)!=expected:return None
    manifest=root/'docs/fidelity/native-verification.json';original=manifest.read_bytes()
    document=json.loads(original)
    matches=[row for row in document['natives'] if row['address']==job['address']]
    if len(matches)!=1 or matches[0]!=record:return None
    if not target.exists():target.write_bytes(source.read_bytes())
    if digest(target)!=expected:return None
    updated=dict(path=str(target.relative_to(root)),sha256=expected)
    matches[0]['verification']['scope']=updated
    if manifest.read_bytes()!=original:return None
    save(manifest,document,sort_keys=False)
    record['verification']['scope']=updated
    fresh,_,_,context=inventory(root)
    job.update(fresh[job['address']])
    return context


def repair_policy(result):
    """Bounded recovery, never status/probe-contract weakening."""
    message=(result.get('error','')+'\n'+result.get('failure_details','')).lower()
    if any(word in message for word in ('timeout','timed out','frida','build/receipt',
                                       'inputs changed','compiler inputs','analysis failed')):
        return 'retry_fresh_tools_build_and_capture'
    if 'unobserved' in message:
        return 'retry_longer_same_scenario'
    return 'evidence_or_behavior_gap_no_automatic_promotion'


def verify(root, job, record, folder, args):
    folder.mkdir(parents=True)
    attempts=[]
    maximum=getattr(args,'max_attempts',3)
    result=None
    repaired_context=None
    for number in range(1,maximum+1):
        attempt=folder/f'attempt-{number}'
        print(f"ATTEMPT {number}/{maximum} {job['address']}",flush=True,file=sys.stderr)
        result=verify_once(root,job,record,attempt,args)
        policy=repair_policy(result)
        if not result.get('verified') and number<maximum:
            repaired=repair_scope_path(root,job,record,result)
            if repaired:
                repaired_context=repaired
                policy='relocate_hash_checked_scope_document'
            else:
                repaired=repair_annotation_source(root,job,record,result)
                if repaired:
                    repaired_context=repaired
                    policy='review_verified_annotation_relocation_without_reusing_runtime'
                    result['source_review_repair']=True
        attempts.append(dict(number=number,result=result['status'],error=result.get('error'),
                             repair=policy,report=str(attempt/'result.json')))
        if result.get('verified') or policy=='evidence_or_behavior_gap_no_automatic_promotion':
            break
        # Bounded exponential backoff; no deletion of evidence, edits to game
        # logic, status promotion or parallel native execution as a repair.
        if number<maximum:time.sleep(min(2**(number-1),4))
    result['attempts']=attempts
    if repaired_context:result['repair_context']=repaired_context
    save(folder/'result.json',result)
    return result


def refresh_ledger(root):
    subprocess.run([sys.executable, str(root / "scripts/native_database.py")], cwd=root, check=True, stdout=subprocess.DEVNULL)


def cycle(root, args):
    state_path = args.state or root / 'build/native-auto/state.json'
    prior = json.loads(state_path.read_text()) if state_path.exists() else {}
    paths = selected_paths(root, args.scope, args.base, args.head)
    registrations=[]
    if args.command != 'scan':
        registrations=intake_candidates(root,paths,args.address)
    jobs, records, issues, context = inventory(root)
    required = select(root, jobs, paths, args.address)
    required.update(row['address'] for row in registrations if row['address'] in jobs)
    if not args.address and paths and 'docs/fidelity/native-verification.json' in paths:
        base = args.base if args.scope == 'range' else 'HEAD'
        previous = subprocess.run(['git', 'show', base + ':docs/fidelity/native-verification.json'],
                                  cwd=root, capture_output=True, text=True)
        if previous.returncode:
            raise ValueError('cannot compare changed manifest with its base')
        before = {r['address'].upper(): r for r in json.loads(previous.stdout)['natives']}
        from native_gate import STATUS_RANK
        for va, record in records.items():
            old = before.get(va.upper())
            if va not in jobs:
                continue
            old_status = (old or {}).get('verification', {}).get('status', 'ANALYZED')
            new_status = record.get('verification', {}).get('status', 'ANALYZED')
            if (old is None or old.get('implementation') != record.get('implementation') or
                    STATUS_RANK.get(new_status, -1) > STATUS_RANK.get(old_status, -1)):
                required.add(va)
    chosen_sources = {jobs[va]['source'] for va in required}
    relevant_issues = [i for i in issues if (paths is None and not args.address) or
                      i.get('path') in chosen_sources or i.get('address') in required or
                      (not args.address and paths is not None and i.get('path') in paths)]
    config = load_config(root)
    candidate = root / config.get('candidate', 'build/igi1.exe')
    candidate_hash = digest(candidate) if candidate.is_file() else None
    for va, job in jobs.items():
        old = prior.get('tasks', {}).get(va, {})
        if old.get('fingerprint') == job['fingerprint']:
            job.update(old)
            if job['status'] == 'passed':
                report = Path(job.get('report', ''))
                if (not report.is_file() or digest(report) != job.get('report_sha256') or
                        candidate_hash is None or job.get('candidate_sha256') != candidate_hash):
                    job.update(status='pending', verified=False)
                else:
                    from candidate_build_receipt import validate_receipt
                    try:
                        if not complete_report(json.loads(report.read_text()), job, config, candidate_hash):
                            raise ValueError('saved full report is incomplete or stale')
                        validate_receipt(root / config['build_directory'] / 'candidate-receipt.json',
                                         root, candidate, [root / job['source']])
                    except (OSError, ValueError, KeyError):
                        job.update(status='pending', verified=False)
    if args.scope == 'staged':
        for va in required:
            staged = subprocess.run(['git', 'show', ':' + jobs[va]['source']], cwd=root, capture_output=True)
            if staged.returncode or hashlib.sha256(staged.stdout).hexdigest() != jobs[va]['source_sha256']:
                relevant_issues.append({'address': va, 'error': 'index and working source differ; partial-stage verification blocked'})
        if required:
            dirty = git(root, 'diff', '--name-only', '-z', '--', 'engine', 'scripts', 'cmake',
                        'CMakeLists.txt', 'docs/fidelity/native-verification.json')
            if any(dirty):
                relevant_issues.append({'global':True, 'error':'unstaged verification/build inputs differ from index; freeze and stage the intended inputs first'})
    run_id = datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%SZ-') + uuid.uuid4().hex[:8]
    if args.command != 'scan':
        # Serialize analyses/runtime; another cooperating driver cannot capture
        # concurrently because the backend also takes the global Frida lease.
        for va in work_order(root, required):
            job = jobs[va]
            if any(i.get('global') for i in relevant_issues):
                job.update(status='blocked', verified=False, error='staged build-input snapshot is incomplete')
                continue
            if any(i.get('address') == va or i.get('path') == job['source'] for i in relevant_issues):
                job.update(status='blocked', verified=False, error='inventory mapping/identity issue')
                continue
            if job['status'] == 'passed' or (job['status'] == 'blocked' and not args.retry):
                continue
            print('VERIFY', va, job['name'], flush=True, file=sys.stderr)
            result = verify(root, job, records[va], root / 'build/native-auto/runs' / run_id / va, args)
            fresh, fresh_records, _, current = inventory(root)
            expected_context=result.get('repair_context',context)
            if current != expected_context or fresh.get(va, {}).get('fingerprint') != job['fingerprint']:
                result.update(status='pending', verified=False, error='inputs changed during verification; result invalidated')
                # Reject this snapshot but continue the requested sequential
                # queue against fresh inputs; never silently skip later VAs.
                for next_va, next_job in fresh.items():
                    if jobs.get(next_va, {}).get('fingerprint') != next_job['fingerprint']:
                        jobs[next_va] = next_job
                records, context = fresh_records, current
            jobs[va] = result
            save(state_path, {'schema_version': 1, 'kind': 'igi-native-auto-state', 'tasks': jobs})
            refresh_ledger(root)
            print(result['status'].upper(), va, result.get('error', ''), flush=True,file=sys.stderr)
    passed = [va for va in required if jobs[va]['status'] == 'passed' and jobs[va]['verified']]
    result = {'schema_version': 1, 'kind': 'igi-native-auto-state', 'scope': args.scope,
        'tasks': jobs, 'required': sorted(required), 'issues': relevant_issues,
        'selected_count': len(required), 'passed_count': len(passed), 'intake':registrations,
        'result': 'no_native_changes' if not required and not relevant_issues else
                  ('passed' if required and len(passed) == len(required) and not relevant_issues else 'pending'),
        'verified': bool(required) and len(passed) == len(required) and not relevant_issues}
    save(state_path, result)
    # Derived ledger only; never changes acceptance or native manifest status.
    refresh_ledger(root)
    print(json.dumps({k:result[k] for k in ('scope', 'selected_count', 'passed_count', 'result', 'verified')} |
                     {'issue_count':len(relevant_issues), 'issues':relevant_issues[:8], 'intake':registrations,
                      'state':str(state_path)}), flush=True)
    return 0 if result['result'] in {'passed', 'no_native_changes'} or args.command == 'scan' and not relevant_issues else 1


def audit_demo(root, args):
    """Audit a preserved revision without checking it out or importing its labels."""
    commit = subprocess.check_output(['git', 'rev-parse', '--verify', '--end-of-options',
                                     args.demo_ref + '^{commit}'], cwd=root, text=True).strip()
    manifest = json.loads(subprocess.check_output(['git', 'show',
        commit + ':docs/fidelity/native-verification.json'], cwd=root))
    records = [r for r in manifest['natives'] if r.get('implementation', {}).get('path', '').startswith('engine/')]
    paths = sorted({r['implementation']['path'] for r in records})
    if any(any(c.isspace() for c in p) or '..' in Path(p).parts for p in paths):
        raise ValueError('demo source paths require explicit review')
    # One bounded Git batch; no new worktree and no changes to either revision.
    batch = subprocess.run(['git', 'cat-file', '--batch'], cwd=root,
        input=''.join(commit + ':' + p + '\n' for p in paths).encode(), capture_output=True, check=True).stdout
    hashes, cursor = {}, 0
    for path in paths:
        end = batch.index(b'\n', cursor)
        header = batch[cursor:end].split()
        cursor = end + 1
        if header[-1] == b'missing':
            continue
        size = int(header[2])
        hashes[path] = hashlib.sha256(batch[cursor:cursor + size]).hexdigest()
        cursor += size + 1
    state_path = args.state or root / 'build/native-auto/state.json'
    state = json.loads(state_path.read_text()) if state_path.is_file() else {}
    config = load_config(root)
    image = args.demo_candidate.resolve() if args.demo_candidate else None
    image_hash = digest(image) if image and image.is_file() else None
    rows = []
    for record in records:
        va = f"0x{int(record['address'],16):08X}"
        path = record['implementation']['path']
        source_hash = hashes.get(path)
        saved = state.get('tasks', {}).get(va, {})
        report = Path(saved.get('report', ''))
        passed = False
        if image_hash and source_hash and saved.get('status') == 'passed' and report.is_file():
            job = {'address':va, 'source_sha256':source_hash}
            passed = (digest(report) == saved.get('report_sha256') and
                      complete_report(json.loads(report.read_text()), job, config, image_hash))
        rows.append(dict(address=va, name=record['name'], source=path, source_sha256=source_hash,
            source_matches_historical_record=bool(source_hash) and source_hash==record['implementation'].get('sha256'),
            fresh_paired_pass_for_this_demo_image=passed, reuse_allowed=passed))
    by_source = {}
    for row in rows:
        by_source.setdefault(row['source'], []).append(row)
    eligible = [path for path, natives in by_source.items() if all(n['reuse_allowed'] for n in natives)]
    result = dict(schema_version=1, kind='igi-demo-reuse-audit', result='provisional', verified=False,
        demo_commit=commit, demo_image=str(image) if image else None, demo_image_sha256=image_hash,
        engine_native_records=len(rows), source_hash_matches=sum(r['source_matches_historical_record'] for r in rows),
        fresh_paired_passes=sum(r['fresh_paired_pass_for_this_demo_image'] for r in rows),
        unverified=sum(not r['fresh_paired_pass_for_this_demo_image'] for r in rows),
        copy_eligible_sources=eligible, natives=rows,
        limitations=['No historical VERIFIED label is accepted as current paired runtime evidence.',
                     'Zero tested passes means unverified, not zero percent correct.',
                     'Unmapped scene/controller helpers are not authorized for engine copying.',
                     'This audit does not launch or validate full demo gameplay. Native probes/ABI must be reviewed first.'])
    output = args.output or root / 'build/native-auto/demo-reuse.json'
    save(output, result)
    print(json.dumps({k:v for k,v in result.items() if k != 'natives'}))
    return 1 if result['unverified'] or not image_hash else 0


def acquire_worker_lock(stream,timeout):
    deadline=time.monotonic()+timeout
    announced=False
    while True:
        try:
            fcntl.flock(stream,fcntl.LOCK_EX|fcntl.LOCK_NB)
            return
        except BlockingIOError:
            if time.monotonic()>=deadline:
                raise TimeoutError('another native verifier still owns the sequential worker lock')
            if not announced:
                print('WAITING for the active native verifier; no parallel game testing',file=sys.stderr,flush=True)
                announced=True
            time.sleep(min(.5,max(0,deadline-time.monotonic())))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('command', nargs='?', default='run', choices=['scan', 'run', 'watch', 'audit-demo'])
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--scope', choices=['changed', 'all', 'staged', 'range'])
    parser.add_argument('--base')
    parser.add_argument('--head', default='HEAD')
    parser.add_argument('--address', action='append', default=[], type=lambda v:f'0x{int(v,0):08X}')
    parser.add_argument('--level', type=int, choices=range(1,15), default=1)
    parser.add_argument('--state', type=Path)
    parser.add_argument('--timeout', type=float, default=600)
    parser.add_argument('--lock-timeout', type=float, default=600, help='Wait for the active sequential verifier rather than running concurrently')
    parser.add_argument('--interval', type=float, default=5)
    parser.add_argument('--until-passed', action='store_true', help='In watch mode, stop only after a nonempty accepted scope')
    parser.add_argument('--watch-timeout', type=float, default=0, help='Bound watch duration in seconds; zero means no deadline')
    parser.add_argument('--max-attempts', type=int, choices=range(1,6), default=3, help='Bounded autonomous repair attempts per native')
    parser.add_argument('--retry', action='store_true', help='Retry unchanged blocked jobs once')
    parser.add_argument('--demo-ref', default='game-engine-flow')
    parser.add_argument('--demo-candidate', type=Path, help='Exact preserved demo executable; never substitute the retail image')
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    args.scope = args.scope or ('all' if args.command == 'watch' else 'changed')
    if not math.isfinite(args.timeout) or not math.isfinite(args.interval) or not math.isfinite(args.lock_timeout) or args.timeout <= 0 or args.interval <= 0 or args.lock_timeout < 0 or not math.isfinite(args.watch_timeout) or args.watch_timeout < 0:
        parser.error('timeout and interval must be positive')
    root = args.root.resolve()
    if args.command == 'audit-demo':
        try:
            return audit_demo(root, args)
        except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
            print('demo reuse audit blocked:', error, file=sys.stderr)
            return 1
    lock = root / 'build/native-auto/worker.lock'
    lock.parent.mkdir(parents=True, exist_ok=True)
    started=time.monotonic()
    try:
        while True:
            with lock.open('a') as stream:
                acquire_worker_lock(stream,args.lock_timeout)
                code=cycle(root,args)
            if args.command!='watch':return code
            state_path=args.state or root/'build/native-auto/state.json'
            state=json.loads(state_path.read_text())
            if args.until_passed and state.get('verified') is True and state.get('selected_count',0)>0:
                return 0
            if args.watch_timeout and time.monotonic()-started>=args.watch_timeout:
                print('Watch deadline reached without native acceptance; reports retained',file=sys.stderr,flush=True)
                return 1
            args.retry=False
            time.sleep(args.interval)
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        print('native auto-verification blocked:', error, file=sys.stderr)
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
