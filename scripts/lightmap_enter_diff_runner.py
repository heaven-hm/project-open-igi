#!/usr/bin/env python3
"""Concrete x86 replay of the lightmap stack entry and its byte-state callee."""
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
CONTEXT = 0x70011000


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def observe(image, entry, globals_, fixture, mode_only):
    from native_unicorn_runner import new_emulator, read_u32, run_with_arguments
    from unicorn import x86_const as x86
    machine = new_emulator(image)
    count, stack, byte_bd, byte_be = globals_
    context = bytearray([0xA5] * 384)
    struct.pack_into('<i', context, 0x108, fixture['enabled'])
    struct.pack_into('<I', context, 0xD8, fixture['value'])
    machine.mem_write(CONTEXT, bytes(context))
    machine.mem_write(count, struct.pack('<I', fixture['count']))
    machine.mem_write(stack, b'\x5A' * 80)
    machine.mem_write(byte_bd, bytes([fixture['bd']]))
    machine.mem_write(byte_be, bytes([fixture['be']]))
    run_with_arguments(machine, entry, [] if mode_only else [CONTEXT], 0x70018000)
    result = machine.reg_read(x86.UC_X86_REG_EAX)
    return dict(return_value=dict(kind='context' if result == CONTEXT else 'integer',
                                  value=0 if result == CONTEXT else result),
                state_delta=dict(context_hex=bytes(machine.mem_read(CONTEXT, 384)).hex(),
                                 count=read_u32(machine, count)),
                side_effects=dict(stack_hex=bytes(machine.mem_read(stack, 80)).hex(),
                                  bd=machine.mem_read(byte_bd, 1)[0],
                                  be=machine.mem_read(byte_be, 1)[0]))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', default='0x004F82B0')
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--implementation', type=Path, required=True)
    parser.add_argument('--implementation-sha256', required=True)
    args = parser.parse_args()
    address = int(args.address, 16)
    if address not in (0x004F82B0, 0x004F1160):
        raise ValueError('Unsupported native entry')
    mode_only = address == 0x004F1160
    mode_source = args.root / 'engine/src/retail/script/script_mode.cpp'
    if mode_only and args.implementation.resolve() != mode_source.resolve():
        raise ValueError('Mode replay implementation must be the source actually compiled')
    dependencies = {str(mode_source): digest(mode_source)}
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('Input identities differ')
    try:
        import unicorn
        import pefile
    except ImportError:
        interpreter = os.environ.get('IGI_ANGR_PYTHON', str(args.root / '.venv-angr/bin/python'))
        if os.environ.get('IGI_LIGHTMAP_RUNNER_REEXEC'):
            raise RuntimeError('Isolated interpreter requires Unicorn and pefile')
        os.execve(interpreter, [interpreter, str(Path(__file__).resolve()), *sys.argv[1:]],
                  dict(os.environ, IGI_LIGHTMAP_RUNNER_REEXEC='1'))
    from native_unicorn_runner import PebImage
    fixtures = []
    flags = [(0, 0), (1, 0), (0, 1), (1, 1)]
    if mode_only:
        flags += [(128, 0), (0, 128), (255, 255), (255, 1)]
    for bd, be in flags:
        for enabled in ([0] if mode_only else [0, 1, -1]):
            for count in ([0] if mode_only else [0, 19]):
                fixtures.append(dict(bd=bd, be=be, enabled=enabled, count=count,
                                     value=0xFFFFFFFF if count else 7))
    with tempfile.TemporaryDirectory(prefix='igi-lightmap-diff-') as temporary:
        folder = Path(temporary)
        wrapper = folder / 'candidate.cpp'
        source = args.root / 'engine/src/retail/terrain/terrain_loaders.cpp' if mode_only else args.implementation
        wrapper.write_text('#include ' + json.dumps(str(mode_source.resolve())) + '\n#include ' +
                           json.dumps(str(source.resolve())) + '''
extern "C" __declspec(dllexport) void* LightmapEntry(void* context) {
    return igi::retail::terrain::TerrainLightMap_Init_2(context);
}
extern "C" __declspec(dllexport) unsigned ModeEntry() {
    return igi::retail::script::Script_ParseEnumReal32_5();
}
extern "C" __declspec(dllexport) void* LightmapGlobals[] = {
    &igi::retail::terrain::g_b97d24, igi::retail::terrain::g_data_b97b20.data(),
    &igi::retail::script::g_a758bd, &igi::retail::script::g_a758be,
};
''')
        binary = folder / 'candidate.dll'
        command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared',
                   '-ffunction-sections', '-fdata-sections', '-I'+str(args.root / 'engine/include'),
                   str(wrapper), '-Wl,--gc-sections,--exclude-all-symbols,--image-base,0x10000000',
                   '-static-libgcc', '-static-libstdc++', '-o', str(binary)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=180)
        if built.returncode:
            raise RuntimeError('Candidate compile failed: ' + built.stderr[-3000:])
        pe = pefile.PE(str(binary))
        base = pe.OPTIONAL_HEADER.ImageBase
        exports = {s.name.decode(): base+s.address for s in pe.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
        globals_ = struct.unpack('<4I', pe.get_data(exports['LightmapGlobals']-base, 16))
        retail, candidate = PebImage(args.binary), PebImage(binary)
        cases = []
        for index, fixture in enumerate(fixtures):
            expected = observe(retail, address, (0x00B97D24, 0x00B97B20, 0x00A758BD, 0x00A758BE), fixture, mode_only)
            actual = observe(candidate, exports['ModeEntry' if mode_only else 'LightmapEntry'], globals_, fixture, mode_only)
            cases.append(dict(id=f'case-{index}', categories=['nominal' if index < 3 else 'boundary'],
                              input=fixture, reference_output=expected, candidate_output=actual,
                              result='passed' if expected == actual else 'failed'))
        candidate_hash = digest(binary)
    if (digest(args.binary) != args.binary_sha256 or
            digest(args.implementation) != args.implementation_sha256 or
            any(digest(path) != value for path, value in dependencies.items())):
        raise ValueError('Inputs changed during comparison')
    failed = sum(case['result'] != 'passed' for case in cases)
    print(json.dumps(dict(schema_version=1, kind='behavioral-test-report', executed=True,
                          oracle='retail-binary-execution', result='failed' if failed else 'passed',
                          address=f'0x{address:08X}', binary_sha256=args.binary_sha256,
                          implementation_sha256=args.implementation_sha256,
                          dependencies=dependencies, observable_fields=['return_value', 'state_delta', 'side_effects'],
                          tests_total=len(cases), tests_passed=len(cases)-failed, tests_failed=failed, cases=cases,
                          runner=dict(path='scripts/lightmap_enter_diff_runner.py', sha256=digest(__file__),
                                      command=[sys.executable, *sys.argv]),
                          retail_execution=dict(tool='Unicorn', version=unicorn.__version__,
                                                candidate_sha256=candidate_hash, candidate_build_command=command),
                          limitations=['Valid nonaliasing mapped contexts; stack counters 0 and 19 only.',
                                       'The existing 20-element host storage does not establish retail overflow behavior.',
                                       'Controlled replay does not establish natural game reachability or whole-game parity.'])))
    return int(bool(failed))


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
