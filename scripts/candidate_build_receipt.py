#!/usr/bin/env python3
"""Bind a diagnostic x86 build to compiler inputs; never a release attestation."""
import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import subprocess
import shutil
import shlex

from build_attestation import require_x86_pe


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def snapshot(root, build):
    suffixes = {'.cpp', '.c', '.cc', '.cxx', '.h', '.hpp', '.hh', '.hxx', '.inl', '.cmake'}
    paths = {root / 'CMakeLists.txt'}
    for folder in (root / 'engine', root / 'cmake'):
        if folder.exists():
            paths.update(p for p in folder.rglob('*') if p.is_file() and
                         (p.name == 'CMakeLists.txt' or p.suffix in suffixes))
    paths.add(build / 'CMakeCache.txt')
    paths.update(build.rglob('flags.make'))
    paths.update(build.rglob('link.txt'))
    paths.update(build.glob('CMakeFiles/*/CMakeCXXCompiler.cmake'))
    cache = (build / 'CMakeCache.txt').read_text()
    compiler = next((line.split('=', 1)[1].strip(chr(34)) for line in cache.splitlines()
                     if line.startswith('CMAKE_CXX_COMPILER:')), None)
    compiler = Path(shutil.which(compiler) or compiler) if compiler else None
    if compiler is None or not compiler.is_file():
        raise ValueError('configured compiler is missing')
    return {'sources': {str(p.relative_to(root)): digest(p) for p in sorted(paths)},
            'compiler': {'path': str(compiler.resolve()), 'sha256': digest(compiler)}}


def stable_source_inputs(snapshot_value, root, build):
    """Return repository inputs, excluding files regenerated inside the build tree."""
    build = build.resolve()
    return {
        name: value
        for name, value in snapshot_value['sources'].items()
        if not (root / name).resolve().is_relative_to(build)
    }


def target_output(build):
    # The configured CMake linker rule is authoritative, not a caller's PE path.
    links = list(build.glob('**/CMakeFiles/igi.dir/link.txt'))
    if len(links) != 1:
        raise ValueError('exactly one configured igi target linker rule is required')
    link = links[0]
    directory = link.parent.parent.parent
    commands = [shlex.split(line) for line in link.read_text().splitlines()]
    command = next(command for command in commands if '-o' in command)
    result = (directory / command[command.index('-o') + 1]).resolve()
    if not result.is_relative_to(build.resolve()):
        raise ValueError('configured target output escapes its build directory')
    return result, command, directory


def compiled_inputs(root, build):
    output, command, directory = target_output(build)
    object_dirs = {directory / 'CMakeFiles/igi.dir'}
    link_inputs = list(command)
    # CMake generators can keep static-library arguments in an @response file.
    # Include those archives in the receipt so engine sources are bound too.
    for token in command:
        if token.startswith('@'):
            response = (directory / token[1:]).resolve()
            if response.is_file():
                link_inputs.extend(shlex.split(response.read_text()))
    for token in link_inputs:
        if token.startswith('-') or not token.endswith('.a'):
            continue
        archive = (directory / token).resolve()
        if archive.is_relative_to(build.resolve()) and archive.name.startswith('lib'):
            object_dirs.add(archive.parent / 'CMakeFiles' / (archive.name[3:-2] + '.dir'))
    sources, dependencies, objects = set(), {}, {}
    for object_dir in sorted(object_dirs):
        for dependency_file in sorted(object_dir.rglob('*.obj.d')):
            object_file = dependency_file.with_suffix('')
            # CMake can leave a dependency file behind when a source is removed
            # from a target. It is not a compiled input unless its object still
            # exists in the clean build.
            if not object_file.is_file():
                continue
            entries = shlex.split(dependency_file.read_text().split(':', 1)[1].replace(chr(92) + chr(10), ' '))
            if not entries:
                raise ValueError('empty compiler dependency file')
            source = Path(entries[0]).resolve()
            if source.is_relative_to(root):
                sources.add(str(source.relative_to(root)))
            for entry in entries:
                path = Path(entry).resolve()
                dependencies[str(path)] = digest(path)
            objects[str(object_file.relative_to(build))] = digest(object_file)
    if not sources or not objects:
        raise ValueError('configured target has no recorded compiler inputs')
    return dict(target=str(output), compiled_sources=sorted(sources),
                dependencies=dependencies, objects=objects)


def validate_receipt(path, root, candidate, required_sources=()):
    root = Path(root).resolve()
    report = json.loads(Path(path).read_text())
    build = Path(report['build_directory']).resolve()
    if (report.get('kind') != 'diagnostic-candidate-build' or report.get('result') != 'built'
            or report.get('schema_version') != 1 or Path(report['root']).resolve() != root
            or report.get('candidate_sha256') != digest(candidate)):
        raise ValueError('candidate has no matching diagnostic build receipt')
    if report.get('compilation') != compiled_inputs(root, build):
        raise ValueError('candidate target compilation inputs differ from its receipt')
    if report.get('inputs') != snapshot(root, build):
        raise ValueError('candidate compiler inputs differ from its build receipt; rebuild')
    for source in required_sources:
        name = str(Path(source).resolve().relative_to(root.resolve()))
        if name not in report['compilation']['compiled_sources']:
            raise ValueError('required source is absent from the build receipt: ' + name)
    require_x86_pe(Path(candidate))
    return report


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument('--build-dir', type=Path)
    parser.add_argument('--candidate', type=Path)
    parser.add_argument('--receipt', type=Path)
    args = parser.parse_args()
    root = args.root.resolve()
    build = (args.build_dir or root / 'build-win32-no-verify').resolve()
    configured, _, _ = target_output(build)
    candidate = (args.candidate or configured).resolve()
    if candidate != configured:
        raise ValueError('--candidate must be the configured igi target output')
    receipt = args.receipt or root / 'build/diagnostic-candidate-build.json'
    before = snapshot(root, build)
    command = ['cmake', '--build', str(build), '--target', 'igi', '--clean-first', '-j4']
    subprocess.run(command, cwd=root, check=True)
    after = snapshot(root, build)
    if (before['compiler'] != after['compiler'] or
            stable_source_inputs(before, root, build) != stable_source_inputs(after, root, build)):
        raise ValueError('source or compiler inputs changed during the clean build; no receipt issued')
    require_x86_pe(candidate)
    report = dict(schema_version=1, kind='diagnostic-candidate-build', result='built',
                  scope='Diagnostic compiler/source binding only; no native acceptance or deployment',
                  root=str(root), build_directory=str(build), command=command,
                  candidate=str(candidate), candidate_sha256=digest(candidate), inputs=after,
                  compilation=compiled_inputs(root, build),
                  created_at=datetime.now(timezone.utc).isoformat())
    receipt.parent.mkdir(parents=True, exist_ok=True)
    receipt.write_text(json.dumps(report, indent=2) + '\n')
    print('Diagnostic build receipt:', receipt)


if __name__ == '__main__':
    main()
