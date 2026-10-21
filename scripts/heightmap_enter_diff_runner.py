#!/usr/bin/env python3
"""Execute retail and the actual x86 HeightMap_Enter candidate on identical state.

Concrete controlled replay only: this does not establish game-loop reachability,
object placement, rendering, or whole-function equivalence for arbitrary aliases.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CONTEXT, TABLE, ROW = 0x70011000, 0x70012000, 0x70014000


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def build_candidate(source, folder):
    wrapper = folder / 'heightmap.cpp'
    wrapper.write_text('#include ' + json.dumps(str(source)) + '''
extern "C" __declspec(dllexport) void* HeightMapEntry(void* context) {
    return igi::retail::terrain::HeightMap_Enter(context);
}
extern "C" {
__declspec(dllexport) void* HeightMapGlobals[] = {
    &igi::retail::terrain::g_b97df4,
    &igi::retail::terrain::g_ba2058,
    igi::retail::terrain::g_b97d40.data(),
};
}
''')
    binary = folder / 'heightmap.dll'
    command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared',
               '-ffunction-sections', '-fdata-sections', f'-I{ROOT / "engine/include"}', str(wrapper),
               '-Wl,--gc-sections,--exclude-all-symbols,--image-base,0x10000000',
               '-static-libgcc', '-static-libstdc++', '-o', str(binary)]
    result = subprocess.run(command, capture_output=True, text=True, timeout=180)
    if result.returncode:
        raise RuntimeError('x86 candidate compile failed: ' + result.stderr[-3000:])
    return binary, command


def observe(image, entry, globals_, fixture):
    import unicorn
    from native_unicorn_runner import new_emulator, read_u32, run_with_arguments
    machine = new_emulator(image)
    context = bytearray([0xA5] * 512)
    struct.pack_into('<I', context, 0x68, TABLE if fixture['index'] >= 0 else 0)
    struct.pack_into('<i', context, 0xBC, fixture['index'])
    struct.pack_into('<I', context, 0xF8, fixture['value'])
    machine.mem_write(CONTEXT, bytes(context))
    machine.mem_write(TABLE + 0xDD0, struct.pack('<I', ROW))
    if fixture['index'] >= 0:
        machine.mem_write(ROW + fixture['index'] * 12, struct.pack('<I', fixture['enabled']))
    counter, active, stack = globals_
    machine.mem_write(stack, b'\x5A' * 64)
    machine.mem_write(counter, struct.pack('<I', fixture['count']))
    machine.mem_write(active, struct.pack('<I', 0xA5A5A5A5))
    run_with_arguments(machine, entry, [CONTEXT], 0x70018000)
    eax = machine.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    return {'return_value': {'kind': 'context' if eax == CONTEXT else 'integer',
                             'value': 0 if eax == CONTEXT else eax},
            'state_delta': {'context_hex': bytes(machine.mem_read(CONTEXT, 512)).hex(),
                            'count': read_u32(machine, counter), 'active': read_u32(machine, active)},
            'side_effects': {'stack_first_16': bytes(machine.mem_read(stack, 64)).hex()}}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', default='0x00506F00')
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--implementation', type=Path, required=True)
    parser.add_argument('--implementation-sha256', required=True)
    args = parser.parse_args()
    if int(args.address, 16) != 0x00506F00:
        raise ValueError('runner supports only the statically established 0x00506F00 entry')
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('input identity differs from requested evidence')
    try:
        import unicorn
        import pefile
    except ImportError:
        interpreter = os.environ.get('IGI_ANGR_PYTHON', str(ROOT / '.venv-angr/bin/python'))
        if os.environ.get('IGI_HEIGHTMAP_RUNNER_REEXEC'):
            raise RuntimeError('isolated interpreter must provide Unicorn and pefile')
        os.execve(interpreter, [interpreter, str(Path(__file__).resolve()), *sys.argv[1:]],
                  dict(os.environ, IGI_HEIGHTMAP_RUNNER_REEXEC='1'))
    from native_unicorn_runner import PebImage
    fixtures = [dict(id=name, index=index, enabled=enabled, value=value, count=count)
                for name, index, enabled, value, count in (
                    ('negative-index', -1, 0, 7, 0),
                    ('minimum-negative-index', -2147483648, 0, 0, 3),
                    ('empty-source-zero-index', 0, 0, 7, 0),
                    ('empty-source-offset-index', 2, 0, 9, 3),
                    ('enabled-zero-value', 0, 1, 0, 0),
                    ('enabled-value', 0, 1, 7, 0),
                    ('enabled-offset-index', 2, 1, 9, 3),
                    ('unsigned-index-wrap', 1, 1, 0xFFFFFFFF, 4))]
    with tempfile.TemporaryDirectory(prefix='igi-heightmap-diff-') as temp:
        candidate, command = build_candidate(args.implementation.resolve(), Path(temp))
        parsed = pefile.PE(str(candidate))
        base = parsed.OPTIONAL_HEADER.ImageBase
        exports = {s.name.decode(): base + s.address for s in parsed.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
        globals_va = exports['HeightMapGlobals']
        globals_ = struct.unpack('<III', parsed.get_data(globals_va - base, 12))
        reference, rebuilt = PebImage(args.binary), PebImage(candidate)
        cases = []
        for fixture in fixtures:
            expected = observe(reference, 0x00506F00, (0x00B97DF4, 0x00BA2058, 0x00B97D40), fixture)
            actual = observe(rebuilt, exports['HeightMapEntry'], globals_, fixture)
            categories = ['nominal'] if fixture['id'] in ('enabled-value', 'enabled-offset-index') else ['boundary']
            cases.append(dict(id=fixture['id'], categories=categories, input=fixture, reference_output=expected,
                              candidate_output=actual, result='passed' if actual == expected else 'failed'))
        candidate_hash = digest(candidate)
    failed = sum(c['result'] != 'passed' for c in cases)
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('inputs changed during comparison')
    report = dict(schema_version=1, kind='behavioral-test-report', result='failed' if failed else 'passed',
                  executed=True, oracle='retail-binary-execution', address='0x00506F00',
                  binary_sha256=args.binary_sha256, implementation_sha256=args.implementation_sha256,
                  observable_fields=['return_value', 'state_delta', 'side_effects'],
                  tests_total=len(cases), tests_passed=len(cases)-failed, tests_failed=failed, cases=cases,
                  runner={'path': 'scripts/heightmap_enter_diff_runner.py', 'sha256': digest(__file__),
                          'command': [sys.executable, *sys.argv]},
                  retail_execution={'tool': 'Unicorn', 'version': unicorn.__version__,
                                    'candidate_sha256': candidate_hash, 'candidate_build_command': command},
                  limitations=['Eight concrete x86 replay fixtures, not game-loop or rendering acceptance.',
                               'Valid stack counters 0..4; arbitrary aliases/overflow and unmapped contexts excluded.'])
    print(json.dumps(report))
    return int(bool(failed))


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
