#!/usr/bin/env python3
"""Prepare isolated native repair proposals; never apply or accept a child's claims."""
import argparse
import difflib
import hashlib
import json
import os
import re
from pathlib import Path
import shutil
import signal
import subprocess
import sys
import time

from native_verification_queue import atomic_json, digest, fingerprint, group_report, tool_identities, address

TREES = ('engine', 'scripts', 'docs', 'assets')
MANIFEST = 'docs/fidelity/native-verification.json'
TOP = ('CMakeLists.txt', 'build.sh', 'build_no_verify.sh', 'AGENTS.md', 'plan.md')


def select_jobs(report, requested, limit, manifest=None, root=None):
    grouped = group_report(report, requested)
    if grouped['unmatched_addresses']:
        raise ValueError('Addresses absent from report: ' + ', '.join(grouped['unmatched_addresses']))
    mapped = {}
    canonical = {item['address']: item.get('implementation', {}).get('path')
                 for item in (manifest or {}).get('natives', [])}
    for item in grouped['groups']:
        path = canonical.get(item['address'], item['path'])
        if not path or not item['address'] or not item['failures']:
            continue
        missing_record = item['address'] not in canonical and any(
            'no binary-bound verification record' in failure for failure in item['failures'])
        if not path.startswith('engine/') and not missing_record:
            continue
        if root is not None and not (root / path).is_file():
            continue
        target = mapped.setdefault((path, item['address']), dict(path=path, address=item['address'],
                                                               failures=[], original_paths=[],
                                                               kind='missing-record' if missing_record else 'repair'))
        target['failures'].extend(item['failures'])
        target['original_paths'].append(item['path'])
    jobs = list(mapped.values())
    def rank(item):
        text = ' '.join(item['failures']).lower()
        behavioral = any(word in text for word in ('behavioral', 'runner', 'mismatch', 'semantic'))
        return (not behavioral, item['path'], item['address'])
    return sorted(jobs, key=rank)[:limit], grouped


def missing_record_dossier(root, output, job, binary, report, analyze=False, timeout=600):
    """Report missing prerequisites without inventing an implementation or manifest record."""
    root = root.resolve()
    output.mkdir(parents=True, exist_ok=False)
    address_text = job['address']
    # Only expose matching paths as leads, never claim an address mention is an implementation.
    needles = (address_text.lower(), address_text[2:].lower())
    sources = []
    for source in (root / 'engine').rglob('*'):
        if source.is_file() and source.suffix.lower() in ('.cpp', '.cc', '.cxx', '.h', '.hpp'):
            try:
                if any(needle in source.read_text(errors='replace').lower() for needle in needles):
                    sources.append(str(source.relative_to(root)))
            except OSError:
                continue
    references = []
    for path in job['original_paths']:
        candidate = (root / safe_relative(path)).resolve()
        if candidate.is_relative_to(root) and candidate.is_file():
            references.append(dict(path=path, sha256=digest(candidate)))
    analysis = report.get('angr_by_address', {}).get(address_text, report.get('angr_analysis', {}))
    dossier_data = dict(address=address_text, binary_sha256=digest(binary),
                        references=references, engine_address_mentions=sorted(sources),
                        angr_analysis=analysis, failures=job['failures'],
                        missing=['binary-bound manifest record', 'confirmed engine implementation and ABI',
                                 'selected decompiler and radare2 CFG evidence',
                                 'semantic review and behavioral comparison'],
                        note='Address mentions and reference sources are leads, not verified implementations. '
                             'Runtime comparison and review are required before acceptance.')
    atomic_json(output / 'dossier.json', dossier_data)
    analysis_attempt = None
    if analyze:
        baseline, workspace = output / 'baseline', output / 'workspace'
        snapshot(root, baseline)
        shutil.copytree(baseline, workspace)
        before = source_inventory(workspace)
        prompt = ('Analyze exactly one unregistered retail native. Read AGENTS.md and the listed '
                  'GameNatives reference files and adjacent headers in this disposable workspace. '
                  'Use Binary Ninja MCP first for the exact address and verify its active image identity. '
                  'Only if Binary Ninja is unavailable, use Ghidra MCP for this target; do not use both. '
                  'Use Radare2 CFG and angr independently where available. The retail image is '
                  + str(binary) + ' with SHA-256 ' + digest(binary) + '.\n'
                  + json.dumps(dossier_data, indent=2) + '\n'
                  'Provide concise observations, paths and tool evidence IDs, ABI uncertainty, '
                  'and remaining static and runtime requirements. Clearly mark failures and mismatched '
                  'image identities. Treat GameNatives as unverified candidates. '
                  'Do not create or edit files, use git, promote statuses, launch games, or claim parity.')
        analysis_attempt = owned_process(child_command(workspace, output / 'analysis-summary.txt'),
                                         workspace, output / 'analysis.jsonl', timeout, prompt)
        analysis_attempt['source_mutations'] = source_mutations(before, source_inventory(workspace))
        analysis_attempt['scope'] = 'Untrusted agent observations; validate image, tool results and evidence independently.'
        atomic_json(output / 'analysis-attempt.json', analysis_attempt)
    (output / 'proposal.patch').write_text('')
    result = dict(address=address_text, source=job['path'], result='missing-record-evidence-required',
                  gate_passed=False, interrupted=False, dossier=str(output / 'dossier.json'),
                  scope='Discovery only; no manifest record, implementation, or status was invented',
                  analysis_attempt=analysis_attempt)
    atomic_json(output / 'result.json', result)
    return result


def discover_addresses(root, output, binary, addresses, timeout):
    """Run the existing strict gate per requested address and preserve its failures."""
    reports = []
    output.mkdir(parents=True, exist_ok=True)
    for native in addresses:
        report_path = output / (native + '-discovery.json')
        log = output / (native + '-discovery.log')
        report_path.unlink(missing_ok=True)
        command = [sys.executable, str(root / 'scripts/native_gate.py'), '--root', str(root),
                   '--binary', str(binary), '--address', native, '--threshold', 'VERIFIED',
                   '--cfg-threshold', '0.85', '--require-semantic-evidence',
                   '--require-behavioral-tests', '--report', str(report_path)]
        result = owned_process(command, root, log, timeout)
        if result['timed_out'] or result['interrupted'] or not report_path.is_file():
            raise ValueError('discovery gate did not complete for ' + native + ' (exit ' + str(result['returncode']) + '); see ' + str(log))
        report = json.loads(report_path.read_text())
        if report.get('binary_sha256') != digest(binary) or result['returncode'] not in (0, 1):
            raise ValueError('invalid discovery result for ' + native + '; see ' + str(log))
        reports.append(report)
    combined = dict(binary_sha256=digest(binary), result='discovery', checked_natives=[], failures=[],
                    discovery_reports=[str(output / (native + '-discovery.json')) for native in addresses],
                    angr_by_address={native: report.get('angr_analysis', {}) for native, report in zip(addresses, reports)})
    for report in reports:
        combined['checked_natives'].extend(report.get('checked_natives', []))
        combined['failures'].extend(report.get('failures', []))
    path = output / 'discovery-combined.json'
    atomic_json(path, combined)
    return path


def safe_relative(value):
    path = Path(value)
    if path.is_absolute() or '..' in path.parts:
        raise ValueError('Unsafe evidence/source path: ' + value)
    return path.as_posix()


def dossier(record):
    paths = set()
    def visit(value):
        if isinstance(value, dict):
            for key, item in value.items():
                if key in ('evidence_path', 'source_path') and isinstance(item, str):
                    paths.add(safe_relative(item))
                elif isinstance(item, (dict, list)):
                    visit(item)
        elif isinstance(value, list):
            for item in value:
                visit(item)
    visit(record.get('verification', {}))
    runner = record.get('verification', {}).get('behavioral_runner', {}).get('path')
    return dict(address=record['address'], implementation=record['implementation'],
                selected_decompiler=record.get('verification', {}).get('selected_decompiler'),
                provider_policy='Binary Ninja first, Ghidra fallback only; never both on the selected function. Preserve existing single-provider choice.',
                evidence_paths=sorted(paths), behavioral_runner=safe_relative(runner) if runner else None,
                abi_unknowns=record.get('verification', {}).get('limitations', []) +
                             [record['implementation'].get('signature', 'unknown')],
                re_agent_repository='/Users/haseeb-mir/Documents/Code/re-agent')


def snapshot(root, destination):
    destination.mkdir(parents=True, exist_ok=False)
    def ignore(directory, names):
        return [name for name in names if name in ('.git', '__pycache__', 'build', 'node_modules') or
                name.startswith('build-') or Path(name).suffix.lower() in ('.bndb', '.i64', '.idb', '.gz', '.zip') or
                (Path(directory) / name).is_symlink()]
    for folder in TREES:
        source = root / folder
        if source.is_dir():
            shutil.copytree(source, destination / folder, ignore=ignore)
    for name in TOP:
        if (root / name).is_file():
            shutil.copy2(root / name, destination / name)
    for relative, expected in inventory(destination).items():
        source = root / relative
        if not source.is_file() or digest(source) != expected:
            raise ValueError('Source changed during snapshot: ' + relative)


def inventory(root):
    result = {}
    for directory, folders, files in os.walk(root, followlinks=False):
        # Ignore generated products only; source trees themselves are always inspected.
        folders[:] = [name for name in folders if name != '__pycache__' and not (
            Path(directory) == root and (name == 'build' or name.startswith('build-')))]
        for name in files + [name for name in folders if (Path(directory) / name).is_symlink()]:
            path = Path(directory) / name
            result[path.relative_to(root).as_posix()] = ('symlink:' + os.readlink(path)
                                                       if path.is_symlink() else digest(path))
    return result


def allowed(path, info, existing_paths=None):
    if path == MANIFEST:
        return True  # Separate JSON policy below protects all unrelated records and statuses.
    if path == info['implementation']['path'] or path in info['evidence_paths']:
        return True
    if path == info['behavioral_runner']:
        return True
    return ((path.startswith('scripts/') and '/' not in path[len('scripts/'):] and path.endswith('_runner.py') and
             existing_paths is not None and path not in existing_paths) or
            path == f"docs/fidelity/evidence/native-repair-{info['address']}.md")


def selected_manifest_only(before, after, selected):
    try:
        old, new = json.loads(before.read_text()), json.loads(after.read_text())
        old_records, new_records = old.pop('natives'), new.pop('natives')
        if old != new or len(old_records) != len(new_records):
            return False
        pairs = list(zip(old_records, new_records))
        if any(a.get('address') != b.get('address') for a, b in pairs):
            return False
        for a, b in pairs:
            if a.get('address') != selected:
                if a != b:
                    return False
            else:
                if any(a.get(key) != b.get(key) for key in ('address', 'name', 'binary_sha256')):
                    return False
                if any(a.get('implementation', {}).get(key) != b.get('implementation', {}).get(key)
                       for key in ('path', 'signature')):
                    return False
                if any(a.get('verification', {}).get(key) != b.get('verification', {}).get(key)
                       for key in ('status', 'selected_decompiler')):
                    return False
        return True
    except (OSError, ValueError, KeyError, TypeError):
        return False


def changes(before, after, info, patch_path):
    old, new = inventory(before), inventory(after)
    changed = sorted(path for path in old.keys() | new.keys() if old.get(path) != new.get(path))
    denied = [path for path in changed if not allowed(path, info, old)]
    if MANIFEST in changed and not selected_manifest_only(before / MANIFEST, after / MANIFEST, info['address']):
        denied.append(MANIFEST)
    patch = []
    for path in changed:
        if (before / path).is_symlink() or (after / path).is_symlink():
            denied.append(path)
            continue
        a = (before / path).read_bytes() if path in old else b''
        b = (after / path).read_bytes() if path in new else b''
        try:
            patch.extend(difflib.unified_diff(a.decode().splitlines(True), b.decode().splitlines(True),
                                             fromfile='a/' + path if path in old else '/dev/null',
                                             tofile='b/' + path if path in new else '/dev/null'))
        except UnicodeDecodeError:
            denied.append(path)
    patch_path.write_text(''.join(patch))
    return changed, sorted(set(denied))


def owned_process(command, cwd, log, timeout, prompt=None, environment=None):
    with log.open('w') as stream:
        process = subprocess.Popen(command, cwd=cwd, stdin=subprocess.PIPE if prompt is not None else subprocess.DEVNULL,
                                   stdout=stream, stderr=subprocess.STDOUT, text=True, start_new_session=True, env=environment)
        atomic_json(log.with_suffix('.process.json'), dict(pid=process.pid, command=command, cwd=str(cwd)))
        try:
            process.communicate(prompt, timeout=timeout)
            return dict(returncode=process.returncode, timed_out=False, interrupted=False)
        except (subprocess.TimeoutExpired, KeyboardInterrupt) as error:
            os.killpg(process.pid, signal.SIGKILL)
            process.wait()
            return dict(returncode=process.returncode, timed_out=isinstance(error, subprocess.TimeoutExpired),
                        interrupted=isinstance(error, KeyboardInterrupt))


def gate_passed(code, report, expected_address=None, expected_path=None, expected_binary=None, expected_source=None):
    records = report.get('checked_natives', [])
    return (code == 0 and report.get('result') == 'passed' and report.get('failures') == [] and
            report.get('threshold') == 'VERIFIED' and report.get('cfg_threshold', 0) >= .85 and
            report.get('semantic_review_required') is True and report.get('behavioral_tests_required') is True and
            len(records) == 1 and report.get('checked_native_count') == 1 and
            (expected_binary is None or report.get('binary_sha256') == expected_binary) and
            all(not record.get('errors') and (expected_address is None or record.get('address') == expected_address)
                and (expected_path is None or record.get('path') == expected_path)
                and (expected_source is None or record.get('angr', {}).get('implementation_sha256') == expected_source)
                and (expected_binary is None or record.get('angr', {}).get('binary_sha256') == expected_binary)
                for record in records))


def sandbox_profile(workspace, temporary, report, log):
    # Allow-list writes narrowly; the baseline, dossier and live project remain immutable.
    def quoted(path):
        return json.dumps(str(path.resolve()))
    return ('(version 1)\n(allow default)\n(deny file-write*)\n'
            '(allow file-write* (subpath ' + quoted(workspace) + ') (subpath ' + quoted(temporary) +
            ') (literal ' + quoted(report) + ') (literal ' + quoted(log) + '))\n')


def source_inventory(root):
    return {path: value for path, value in inventory(root).items()
            if path.split('/', 1)[0] in TREES or path in TOP}


def source_mutations(before, after):
    return sorted(path for path in before.keys() | after.keys() if before.get(path) != after.get(path))


def job_prompt(info, failures):
    return ('Repair ONE native candidate in this disposable replica, not the live project. Read AGENTS.md.\n'
            + json.dumps(dict(dossier=info, failures=failures), indent=2) + '\n'
            'Be token-efficient: reuse exact hash-bound existing BN/r2/angr evidence and prepared raw reports. '
            'Do not repeat unchanged analysis, provider inventories, setup, or read unrelated skills/repositories. '
            'Read only the selected source/header and one relevant runner example; implement the focused missing check. '
            'Use the actual RE-Agent MCP orchestration and existing evidence. Analyze uncovered exact retail entry with the '
            'single selected decompiler (Binary Ninja first, Ghidra fallback only), Radare2 CFG and angr. '
            'Use MCP tools read-only; never mutate shared analysis databases or original tooling repositories. '
            'Record observed facts versus unknowns. No invented gameplay logic/constants, no fabricated evidence, '
            'no status promotion, no bypassing checks, no Git operations, no external messages, no game launches. '
            'Only edit selected implementation, listed evidence files, selected/new scripts/*_runner.py, or '
            f"docs/fidelity/evidence/native-repair-{info['address']}.md. Manifest edits may update only the selected native's evidence/hash fields, never its status or unrelated records/root metadata. Never change the gate. "
            'Make one bounded attempt; stop and explain unresolved evidence. Never modify paths outside this replica. '
            'The .venv-angr link is read-only tooling: do not modify it or install packages. '
            'Do not treat tool or fixture success as universal runtime equivalence. Summarize proposed changes and limits.')


def child_command(workspace, summary):
    # --approve-for-me already selects workspace-write; CLI forbids combining --sandbox.
    return ['codex', 'exec', '-C', str(workspace), '--approve-for-me', '--skip-git-repo-check',
            '--json', '--output-last-message', str(summary), '-']


def nullable_getter_profile(root, job, record, binary, gate_report):
    """A deterministic runner registration is possible only for an exact, already-analyzed leaf."""
    errors = job.get('failures', [])
    if not any('missing verification.behavioral_runner' in error for error in errors):
        return None
    if any('missing verification.behavioral_runner' not in error and
           not ('status ' in error and 'below required' in error) for error in errors):
        return None
    runner = record.get('verification', {}).get('behavioral_runner')
    if runner or not gate_report or gate_report.get('binary_sha256') != digest(binary):
        return None
    source = root / record['implementation']['path']
    if record.get('binary_sha256') != digest(binary) or digest(source) != record['implementation'].get('sha256'):
        return None
    checks = [item for item in gate_report.get('checked_natives', []) if item.get('address') == job['address']
              and item.get('path') == job['path']]
    if len(checks) != 1:
        return None
    check, image = checks[0], digest(binary)
    angr = check.get('angr', {})
    if (check.get('cfg_similarity', 0) < .85 or angr.get('result') != 'pass' or
            angr.get('function_entry') != job['address'] or angr.get('binary_sha256') != image or
            angr.get('implementation_sha256') != digest(source) or not angr.get('nodes') or
            angr.get('has_unresolved_jumps') is not False or
            angr.get('helper_sha256') != digest(root / 'scripts/angr_analyze.py')):
        return None
    runner_path = root / 'scripts/nullable_getter_diff_runner.py'
    if not runner_path.is_file():
        return None
    try:
        from nullable_getter_diff_runner import recognize_nullable_member_getter
        profile = recognize_nullable_member_getter(binary, job['address'])
    except (ImportError, ValueError, OSError):
        return None
    if not profile:
        return None
    text = source.read_text()
    namespaces = re.findall(r'\bnamespace\s+([A-Za-z_]\w*(?:::[A-Za-z_]\w*)*)\s*\{', text)
    name = record.get('name', '')
    if len(namespaces) != 1 or re.fullmatch(r'[A-Za-z_]\w*', name) is None:
        return None
    signature = r'\bstd::int32_t\s+' + re.escape(name) + r'\s*\(\s*const\s+[A-Za-z_:0-9]+\s*\*\s*[A-Za-z_]\w*\s*\)'
    if len(re.findall(signature + r'\s*\{', text)) != 1:
        return None
    includes = re.findall(r'#include\s+"([^"]+)"', text)
    headers = [root / 'engine/include' / path for path in includes]
    if sum(len(re.findall(signature + r'\s*;', header.read_text())) for header in headers if header.is_file()) != 1:
        return None
    return dict(profile=profile, symbol=namespaces[0] + '::' + name,
                runner='scripts/nullable_getter_diff_runner.py', runner_sha256=digest(runner_path))


def register_nullable_runner(workspace, record, profile):
    manifest_path = workspace / MANIFEST
    manifest = json.loads(manifest_path.read_text())
    selected = [item for item in manifest['natives'] if item.get('address') == record['address']]
    if len(selected) != 1:
        raise ValueError('Deterministic profile requires a unique selected manifest record')
    selected[0]['verification']['behavioral_runner'] = dict(path=profile['runner'], timeout_seconds=120, args=[
        '--address', '{address}', '--binary', '{binary}', '--binary-sha256', '{binary_sha256}',
        '--implementation', '{implementation}', '--implementation-sha256', '{implementation_sha256}',
        '--root', '{root}', '--symbol', profile['symbol']])
    atomic_json(manifest_path, manifest)


def run_job(root, output, job, record, binary, timeout, gate_report=None):
    output.mkdir(parents=True, exist_ok=False)
    info = dossier(record)
    baseline, workspace = output / 'baseline', output / 'workspace'
    snapshot(root, baseline)
    shutil.copytree(baseline, workspace)
    # Resolved tooling lies outside the child workspace sandbox; it must remain read-only.
    venv = root / '.venv-angr'
    if venv.is_dir():
        (workspace / '.venv-angr').symlink_to(venv, target_is_directory=True)
        (baseline / '.venv-angr').symlink_to(venv, target_is_directory=True)
    info['original_report_paths'] = job.get('original_paths', [job['path']])
    atomic_json(output / 'dossier.json', info)
    fastpath = nullable_getter_profile(root, job, record, binary, gate_report)
    if fastpath:
        register_nullable_runner(workspace, record, fastpath)
        atomic_json(output / 'deterministic-profile.json', fastpath)
        child = dict(returncode=0, timed_out=False, interrupted=False, mode='deterministic-nullable-member-getter')
    else:
        command = child_command(workspace, output / 'child-summary.txt')
        child = owned_process(command, workspace, output / 'child.jsonl', timeout, job_prompt(info, job['failures']))
    changed, denied = changes(baseline, workspace, info, output / 'proposal.patch')
    source = workspace / info['implementation']['path']
    result = dict(address=job['address'], source=job['path'], child=child, changed_files=changed, denied_changes=denied,
                  retail_sha256=digest(binary),
                  source_sha256_before=digest(baseline / info['implementation']['path']),
                  source_sha256_after=digest(source) if source.is_file() else None,
                  result='candidate-rejected' if denied else 'candidate-review-required',
                  scope='Isolated proposal only; no live changes and no runtime acceptance',
                  requires_hunk_level_critic=True, interrupted=child.get('interrupted', False))
    if child['returncode'] != 0 or child.get('timed_out') or child.get('interrupted'):
        result['result'] = 'candidate-incomplete'
    if not denied and child['returncode'] == 0 and not child.get('timed_out') and not child.get('interrupted'):
        gate_report = output / 'native-gate.json'
        gate_command = [sys.executable, str(workspace / 'scripts/native_gate.py'), '--root', str(workspace),
                        '--file', job['path'], '--address', job['address'], '--binary', str(binary),
                        '--manifest', str(workspace / 'docs/fidelity/native-verification.json'),
                        '--threshold', 'VERIFIED', '--cfg-threshold', '0.85', '--require-semantic-evidence',
                        '--require-behavioral-tests', '--report', str(gate_report)]
        temporary = output / 'tmp'
        temporary.mkdir()
        profile = output / 'gate.sb'
        gate_log = output / 'gate.log'
        profile.write_text(sandbox_profile(workspace, temporary, gate_report, gate_log))
        before_gate, baseline_before = source_inventory(workspace), inventory(baseline)
        if sys.platform != 'darwin' or not Path('/usr/bin/sandbox-exec').is_file():
            result.update(gate_passed=False, gate_unavailable='Filesystem sandbox unavailable; generated runner was not executed')
        else:
            environment = dict(os.environ, TMPDIR=str(temporary), PYTHONPYCACHEPREFIX=str(temporary / 'pycache'),
                               XDG_CACHE_HOME=str(temporary / 'cache'))
            gate = owned_process(['/usr/bin/sandbox-exec', '-f', str(profile)] + gate_command,
                                 workspace, gate_log, timeout, environment=environment)
            try:
                report = json.loads(gate_report.read_text()) if gate_report.exists() else {}
            except ValueError as error:
                report = {}
                result['gate_report_error'] = str(error)
            mutated = source_mutations(before_gate, source_inventory(workspace))
            baseline_mutated = source_mutations(baseline_before, inventory(baseline))
            changed, denied = changes(baseline, workspace, info, output / 'proposal.patch')
            result.update(gate=gate, changed_files=changed, denied_changes=denied,
                          gate_source_mutations=mutated, baseline_mutations=baseline_mutated,
                          interrupted=result['interrupted'] or gate.get('interrupted', False),
                          gate_passed=not mutated and not baseline_mutated and not denied and
                          gate_passed(gate['returncode'], report, job['address'], job['path'],
                                      digest(binary), result['source_sha256_after']))
            if mutated or baseline_mutated or denied:
                result['result'] = 'candidate-rejected'
            if gate.get('interrupted'):
                result['result'] = 'candidate-incomplete'
    result['handoff'] = f"Review {output / 'proposal.patch'} and {output / 'result.json'}; independent review is required before applying selected hunks."
    result['review_command'] = ['git', '-C', str(root), 'apply', '--check', str(output / 'proposal.patch')]
    result['apply_after_independent_review'] = ['git', '-C', str(root), 'apply', str(output / 'proposal.patch')]
    atomic_json(output / 'result.json', result)
    return result


def repair_fingerprint(root, binary, manifest, report):
    command = shutil.which('codex')
    identity = digest(Path(command).resolve()) if command else 'codex-unavailable'
    return hashlib.sha256((fingerprint(root, binary, manifest, report) + identity).encode()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path('.'))
    parser.add_argument('--gate-report', type=Path, default=Path('build/native-verification.json'))
    parser.add_argument('--address', action='append', type=address)
    parser.add_argument('--discover', action='store_true', help='Run the strict gate for each requested address before planning')
    parser.add_argument('--max-jobs', type=int, default=1)
    parser.add_argument('--timeout', type=float, default=600)
    parser.add_argument('--output', type=Path, default=Path('build/native-repair-loop'))
    parser.add_argument('--run', action='store_true')
    parser.add_argument('--analyze-missing', dest='analyze_missing', action='store_true', default=True, help='Use bounded Codex/MCP analysis for missing records (default)')
    parser.add_argument('--no-missing-analysis', dest='analyze_missing', action='store_false', help='Create missing-record dossiers without an MCP analysis attempt')
    parser.add_argument('--resume', action='store_true')
    parser.add_argument('--binary', type=Path, default=Path('/Users/haseeb-mir/Downloads/IGI/igi.exe'))
    args = parser.parse_args()
    if args.max_jobs < 1 or not 0 < args.timeout <= 3600:
        parser.error('max-jobs must be positive and timeout must be 1..3600 seconds')
    root = args.root.resolve()
    gate_path, output = (root / args.gate_report).resolve(), (root / args.output).resolve()
    binary = args.binary.resolve()
    if args.discover:
        if not args.address:
            parser.error('--discover requires at least one --address')
        gate_path = discover_addresses(root, output, binary, list(dict.fromkeys(args.address)), args.timeout)
    report = json.loads(gate_path.read_text())
    manifest_path = root / 'docs/fidelity/native-verification.json'
    manifest = json.loads(manifest_path.read_text())
    jobs, grouped = select_jobs(report, set(args.address or []), args.max_jobs, manifest, root)
    if any(output.is_relative_to(root / folder) for folder in TREES):
        raise ValueError('Output must lie outside snapshot source trees')
    binary = args.binary.resolve()
    if digest(binary) != report.get('binary_sha256') or digest(binary) != manifest.get('binary_sha256'):
        raise ValueError('Retail image identity disagrees with report/manifest')
    current = repair_fingerprint(root, binary, manifest_path, gate_path)
    output.mkdir(parents=True, exist_ok=True)
    plan = dict(fingerprint=current, selected_jobs=jobs, all_failures=grouped, mode='run' if args.run else 'plan',
                scope='Bounded isolated repair proposals; no deployment or acceptance')
    atomic_json(output / 'plan.json', plan)
    if args.run:
        if not jobs:
            raise ValueError('Empty repair queue; no candidate can be verified')
        help_text = subprocess.run(['codex', 'exec', '--help'], capture_output=True, text=True, check=True, timeout=10).stdout
        if not all(flag in help_text for flag in ('--approve-for-me', '--skip-git-repo-check', '--output-last-message', '--json')):
            raise ValueError('Installed Codex CLI does not expose required safe execution flags')
        incomplete = False
        for job in jobs:
            key = hashlib.sha256((job['path'] + job['address']).encode()).hexdigest()[:12]
            folder = output / (job['address'] + '-' + key + '-' + str(time.time_ns()))
            record = next((item for item in manifest['natives'] if item.get('address') == job['address'] and
                           item.get('implementation', {}).get('path') == job['path']), None)
            if record is None and job.get('kind') != 'missing-record':
                raise ValueError('No matching binary-bound record for ' + job['address'])
            if repair_fingerprint(root, binary, manifest_path, gate_path) != current:
                raise ValueError('Live inputs changed; regenerate the plan before another repair job')
            reusable = args.resume and all(item.get('sha256') for item in tool_identities(root).values())
            if reusable:
                matches = sorted(output.glob(job['address'] + '-' + key + '-*/input-fingerprint.json'))
                reused = False
                for previous in reversed(matches):
                    saved = json.loads(previous.read_text())
                    if saved.get('fingerprint') != current:
                        continue
                    result_path, patch = previous.parent / 'result.json', previous.parent / 'proposal.patch'
                    if (result_path.is_file() and patch.is_file() and saved.get('result_sha256') == digest(result_path)
                            and saved.get('patch_sha256') == digest(patch)):
                        previous_result = json.loads(result_path.read_text())
                        print('reused ' + previous_result['result'], previous.parent, flush=True)
                        incomplete |= not previous_result.get('gate_passed', False)
                        reused = True
                        break
                if reused:
                    continue
            result = (missing_record_dossier(root, folder, job, binary, report, args.analyze_missing, args.timeout) if record is None else
                      run_job(root, folder, job, record, binary, args.timeout, report))
            atomic_json(folder / 'input-fingerprint.json', dict(fingerprint=current,
                        result_sha256=digest(folder / 'result.json'), patch_sha256=digest(folder / 'proposal.patch')))
            print(result['result'], folder, flush=True)
            incomplete |= not result.get('gate_passed', False)
            if result.get('interrupted'):
                return 130
        if incomplete:
            return 1
    else:
        print(json.dumps(dict(jobs=len(jobs), plan=str(output / 'plan.json')), indent=2))
    return 0


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        print('native_repair_loop: ' + str(error), file=sys.stderr)
        raise SystemExit(1)
