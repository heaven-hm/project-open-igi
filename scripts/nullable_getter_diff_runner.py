#!/usr/bin/env python3
"""Bounded concrete PE32 replay of a strictly recognized nullable s32 getter.

Compiles the selected actual C++ source, never a synthesized getter algorithm.
This diagnostic does not establish game-loop reachability or runtime acceptance.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import struct
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def validate_symbol(symbol):
    if not re.fullmatch(r'[A-Za-z_]\w*(?:::[A-Za-z_]\w*)*', symbol, flags=re.ASCII):
        raise ValueError('symbol must be a qualified C++ identifier')
    return symbol


def recognize_getter(raw):
    """Return (signed member displacement, exact length); reject other bodies."""
    if len(raw) < 14 or raw[:7] != bytes.fromhex('8b44240485c074'):
        raise ValueError('not the strict nullable s32 getter prefix')
    if raw[8:10] == bytes.fromhex('8b00'):
        offset, end = 0, 10
    elif raw[8:10] == bytes.fromhex('8b40') and len(raw) >= 11:
        offset, end = struct.unpack('b', raw[10:11])[0], 11
    else:
        raise ValueError('unsupported getter load encoding')
    if raw[end:end + 4] != bytes.fromhex('c333c0c3'):
        raise ValueError('getter must return loaded value or xor-zero then return')
    if raw[7] != end + 1 - 8:
        raise ValueError('null branch does not target xor-zero')
    if not -32 <= offset <= 32:
        raise ValueError('member displacement exceeds fixture scope')
    return offset, end + 4


def recognize_nullable_member_getter(binary: Path, address: str) -> dict | None:
    """Stdlib-only PE32 profile detection; never infers a native identity."""
    raw = Path(binary).read_bytes()
    try:
        if raw[:2] != b'MZ':
            return None
        pe = struct.unpack_from('<I', raw, 0x3c)[0]
        if raw[pe:pe+4] != b'PE\0\0':
            return None
        machine, count = struct.unpack_from('<HH', raw, pe+4)
        optional_size = struct.unpack_from('<H', raw, pe+20)[0]
        optional = pe+24
        if machine != 0x14c or struct.unpack_from('<H', raw, optional)[0] != 0x10b:
            return None
        base = struct.unpack_from('<I', raw, optional+28)[0]
        rva = int(address, 0) - base
        for index in range(count):
            section = optional + optional_size + 40*index
            virtual_size, virtual_address, raw_size, raw_pointer = struct.unpack_from('<IIII', raw, section+8)
            delta = rva-virtual_address
            if 0 <= delta and delta+15 <= raw_size:
                code = raw[raw_pointer+delta:raw_pointer+delta+15]
                offset, length = recognize_getter(code)
                return {'offset': offset, 'pattern': 'strict-nullable-s32-member-getter',
                        'codehex': code[:length].hex(), 'length': length}
    except (ValueError, struct.error, IndexError):
        return None
    return None


class BoundedMachine:
    def __init__(self, machine):
        self.machine = machine

    def __getattr__(self, name):
        return getattr(self.machine, name)

    def emu_start(self, begin, until):
        self.machine.emu_start(begin, until, timeout=1000000, count=10000)
        import unicorn
        if self.machine.reg_read(unicorn.x86_const.UC_X86_REG_EIP) != until:
            raise RuntimeError('getter execution did not return within bounded replay')


def observe(image, entry, offset, value, shift, null=False):
    import unicorn
    from native_unicorn_runner import new_emulator, run_with_arguments
    machine = BoundedMachine(new_emulator(image))
    base, sentinel = 0x70011000, 0x70018000
    before = bytearray((index * 37 + 19) & 255 for index in range(128))
    struct.pack_into('<I', before, 48 + shift + offset, value)
    machine.mem_write(base, bytes(before))
    writes = []
    def written(_uc, _access, address, size, written_value, _data):
        if not 0x70000000 <= address < 0x70010000:
            writes.append({'offset': address - base, 'size': size, 'value': written_value})
    machine.hook_add(unicorn.UC_HOOK_MEM_WRITE, written)
    run_with_arguments(machine, entry, [0 if null else base + 48 + shift], sentinel)
    result = machine.reg_read(unicorn.x86_const.UC_X86_REG_EAX)
    after = bytes(machine.mem_read(base, 128))
    return {'return_value': result if result < 0x80000000 else result - 0x100000000,
            'state_delta': {'context_hex': after.hex(), 'unchanged': after == bytes(before)},
            'side_effects': {'memory_writes': writes}}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--address', required=True)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--implementation', type=Path, required=True)
    parser.add_argument('--implementation-sha256', required=True)
    parser.add_argument('--symbol', required=True)
    args = parser.parse_args()
    validate_symbol(args.symbol)
    root, source = args.root.resolve(), args.implementation.resolve()
    def identities():
        if digest(args.binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
            raise ValueError('input identity differs from requested evidence')
    identities()
    try:
        import unicorn
        import pefile
    except ImportError:
        if os.environ.get('IGI_NULLABLE_RUNNER_REEXEC'):
            raise RuntimeError('isolated interpreter must provide Unicorn and pefile')
        interpreter = os.environ.get('IGI_ANGR_PYTHON', str(root / '.venv-angr/bin/python'))
        os.execve(interpreter, [interpreter, str(Path(__file__).resolve()), *sys.argv[1:]],
                  dict(os.environ, IGI_NULLABLE_RUNNER_REEXEC='1'))
    from native_unicorn_runner import PebImage
    retail_pe = pefile.PE(str(args.binary))
    if retail_pe.FILE_HEADER.Machine != 0x14c or retail_pe.OPTIONAL_HEADER.Magic != 0x10b:
        raise ValueError('retail image must be PE32 x86')
    address = int(args.address, 0)
    raw = retail_pe.get_data(address - retail_pe.OPTIONAL_HEADER.ImageBase, 15)
    offset, length = recognize_getter(raw)
    with tempfile.TemporaryDirectory(prefix='igi-nullable-diff-') as temp:
        folder = Path(temp)
        wrapper = folder / 'probe.cpp'
        wrapper.write_text('#include ' + json.dumps(str(source)) + '\n'
                           '#include <type_traits>\n#include <cstdint>\n'
                           'template<class T> struct Signature;\n'
                           'template<class R, class A> struct Signature<R(*)(A)> { using Arg=A; using Ret=R; };\n'
                           'using Fn=Signature<decltype(&' + args.symbol + ')>;\n'
                           'static_assert(std::is_pointer_v<Fn::Arg> && sizeof(Fn::Ret)==4);\n'
                           'extern "C" __declspec(dllexport) std::int32_t ProbeEntry(void* context) {\n'
                           'return static_cast<std::int32_t>(' + args.symbol + '(static_cast<Fn::Arg>(context)));\n}\n')
        candidate = folder / 'probe.dll'
        dependencies = folder / 'probe.d'
        command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared',
                   '-ffunction-sections', '-fdata-sections', '-MMD', '-MF', str(dependencies),
                   '-I' + str(root / 'engine/include'), str(wrapper),
                   '-Wl,--gc-sections,--exclude-all-symbols,--image-base,0x10000000',
                   '-static-libgcc', '-static-libstdc++', '-o', str(candidate)]
        result = subprocess.run(command, capture_output=True, text=True, timeout=180)
        if result.returncode:
            raise RuntimeError('candidate harness failed to build: ' + result.stderr[-4000:])
        parsed = pefile.PE(str(candidate))
        if parsed.FILE_HEADER.Machine != 0x14c:
            raise ValueError('candidate must be x86')
        exports = {s.name.decode(): parsed.OPTIONAL_HEADER.ImageBase + s.address
                   for s in parsed.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
        reference, rebuilt = PebImage(args.binary), PebImage(candidate)
        cases = []
        fixtures = [('null', 0, 0, True)] + [
            (f'value-{value:08x}-shift-{shift}', value, shift, False)
            for shift in (0, 4) for value in
            (0, 1, 0xffffffff, 0x7fffffff, 0x80000000, 0x4646494c, 0x4853454d, 0xa5a5a5a5)]
        for name, value, shift, null in fixtures:
            expected = observe(reference, address, offset, value, shift, null)
            actual = observe(rebuilt, exports['ProbeEntry'], offset, value, shift, null)
            passed = actual == expected and expected['state_delta']['unchanged'] and not expected['side_effects']['memory_writes']
            cases.append(dict(id=name, categories=['boundary' if null or value in (0, 0xffffffff, 0x7fffffff, 0x80000000) else 'nominal'],
                              input=dict(member_bits=value, pointer_shift=shift, null=null),
                              reference_output=expected, candidate_output=actual,
                              result='passed' if passed else 'failed'))
        # Compiler dependency output is recorded verbatim, with hashes for existing
        # repository sources/headers; temporary wrapper is retained by content hash.
        dependency_text = dependencies.read_text()
        dependency_paths = shlex.split(dependency_text.replace('\\\n', ' ').split(':', 1)[1])
        dependency_hashes = {str(Path(p).resolve()): digest(p) for p in dependency_paths
                             if Path(p).is_file() and Path(p).resolve() != wrapper.resolve()}
        candidate_hash, wrapper_hash = digest(candidate), digest(wrapper)
    identities()
    failed = sum(c['result'] != 'passed' for c in cases)
    print(json.dumps(dict(schema_version=1, kind='behavioral-test-report', executed=True,
        result='failed' if failed else 'passed', oracle='retail-binary-execution',
        address=f'0x{address:08X}', binary_sha256=args.binary_sha256,
        implementation_sha256=args.implementation_sha256,
        observable_fields=['return_value', 'state_delta', 'side_effects'],
        tests_total=len(cases), tests_passed=len(cases)-failed, tests_failed=failed, cases=cases,
        profile={'kind': 'strict-nullable-s32-member-getter', 'offset': offset,
                 'retail_bytes': raw[:length].hex()},
        runner={'path': str(Path(__file__).resolve()), 'sha256': digest(__file__),
                'helper_sha256': digest(root / 'scripts/native_unicorn_runner.py'),
                'command': [sys.executable, *sys.argv]},
        retail_execution={'tool': 'Unicorn', 'version': unicorn.__version__,
                          'candidate_sha256': candidate_hash, 'wrapper_sha256': wrapper_hash,
                          'candidate_build_command': command, 'dependency_output': dependency_text,
                          'source_header_sha256': dependency_hashes},
        limitations=['Concrete mapped nonaliasing buffers and null only; no game-loop reachability evidence.',
                     'No live paired-runtime acceptance; candidate algorithm comes from the selected actual source.'])))
    return int(bool(failed))


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
