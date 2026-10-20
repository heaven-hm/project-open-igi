#!/usr/bin/env python3
"""Bounded PE32 replay of TextureModifier_Push; no rendering acceptance claim."""
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
CONTEXT, TABLE, ROW, SENTINEL = 0x70011000, 0x70012000, 0x70014000, 0x70018000


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def observe(image, entry, globals_, error_entry, fixture):
    import unicorn
    from unicorn import x86_const as x86
    from native_unicorn_runner import new_emulator, read_u32, STACK_TOP
    machine = new_emulator(image)
    context = bytearray([0xA5] * 512)
    struct.pack_into('<I', context, 0x68, TABLE if fixture['index'] >= 0 else 0)
    struct.pack_into('<i', context, 0xBC, fixture['index'])
    struct.pack_into('<I', context, 0xE4, fixture['value'])
    machine.mem_write(CONTEXT, bytes(context))
    machine.mem_write(TABLE + 0xDCC, struct.pack('<I', ROW))
    if fixture['index'] >= 0:
        machine.mem_write(ROW + fixture['index'] * 12, struct.pack('<I', fixture['enabled']))
    count, stack, mask = globals_
    machine.mem_write(count, struct.pack('<I', fixture['count']))
    machine.mem_write(stack, b'\x5A' * 80)
    machine.mem_write(mask, struct.pack('<I', fixture['mask']))
    calls, repeated = [], dict(previous=None, halted=False)

    def code_hook(emu, address, size, unused):
        if address == error_entry:
            esp = emu.reg_read(x86.UC_X86_REG_ESP)
            message = read_u32(emu, esp + 4)
            text = bytes(emu.mem_read(message, 128)).split(b'\0', 1)[0].decode('ascii')
            calls.append(text)
            # Explicit callee boundary model: observe the error argument and
            # return zero on both sides, then require the caller's fatal loop.
            emu.reg_write(x86.UC_X86_REG_EAX, 0)
            emu.reg_write(x86.UC_X86_REG_EIP, read_u32(emu, esp))
            emu.reg_write(x86.UC_X86_REG_ESP, esp + 4)
            return
        if calls and address == repeated['previous'] and bytes(emu.mem_read(address, 2)) == b'\xeb\xfe':
            repeated['halted'] = True
            emu.emu_stop()
        repeated['previous'] = address

    machine.hook_add(unicorn.UC_HOOK_CODE, code_hook)
    machine.mem_write(STACK_TOP, struct.pack('<II', SENTINEL, CONTEXT))
    machine.reg_write(x86.UC_X86_REG_ESP, STACK_TOP)
    machine.emu_start(entry, SENTINEL, timeout=2000000, count=2000)
    if not repeated['halted'] and machine.reg_read(x86.UC_X86_REG_EIP) != SENTINEL:
        raise RuntimeError('Native neither returned nor reached its observed fatal loop')
    fatal_expected = fixture['index'] >= 0 and fixture['enabled'] != 0 and fixture['count'] >= 20
    if repeated['halted'] != fatal_expected:
        raise RuntimeError('Observed termination disagrees with reviewed fixture path')
    result = machine.reg_read(x86.UC_X86_REG_EAX)
    return dict(return_value=dict(kind='fatal-loop' if repeated['halted'] else
                                  'context' if result == CONTEXT else 'integer',
                                  value=0 if repeated['halted'] or result == CONTEXT else result),
                state_delta=dict(context_hex=bytes(machine.mem_read(CONTEXT, 512)).hex(),
                                 count=read_u32(machine, count), mask=read_u32(machine, mask)),
                side_effects=dict(stack_hex=bytes(machine.mem_read(stack, 80)).hex(), error_calls=calls))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--address', default='0x00505E40')
    p.add_argument('--root', type=Path, default=ROOT)
    for option in ('binary', 'implementation'):
        p.add_argument('--'+option, type=Path, required=True)
        p.add_argument('--'+option+'-sha256', required=True)
    args = p.parse_args()
    if int(args.address, 16) != 0x00505E40:
        raise ValueError('This runner supports only 0x00505E40')
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('Input hashes differ')
    try:
        import unicorn
        import pefile
    except ImportError:
        interpreter = os.environ.get('IGI_ANGR_PYTHON', str(args.root / '.venv-angr/bin/python'))
        if os.environ.get('IGI_MODIFIER_REEXEC'):
            raise RuntimeError('Isolated interpreter requires Unicorn and pefile')
        os.execve(interpreter, [interpreter, str(Path(__file__).resolve()), *sys.argv[1:]],
                  dict(os.environ, IGI_MODIFIER_REEXEC='1'))
    dependencies = {str(path):digest(path) for path in [
        args.root/'engine/src/retail/script/qvm_compiled_program.cpp',
        args.root/'engine/src/retail/script/qvm_object_calls.cpp',
        args.root/'engine/include/igi/retail/terrain/terrain_loaders.hpp',
        args.root/'scripts/native_unicorn_runner.py']}
    from native_unicorn_runner import PebImage
    fixtures = [dict(id=name, index=index, enabled=enabled, value=value, count=count, mask=mask)
                for name, index, enabled, value, count, mask in (
                    ('negative-index', -1, 0, 7, 0, 0x12345678),
                    ('minimum-index', -2147483648, 0, 0, 19, 0xFFFFFFFF),
                    ('disabled-row', 0, 0, 7, 0, 0x12345678),
                    ('disabled-offset-row', 2, 0, 9, 19, 0xFFFFFFFF),
                    ('push-zero', 0, 1, 0, 0, 0),
                    ('push-value', 0, 1, 7, 0, 0x12345678),
                    ('push-last-slot', 2, 1, 9, 19, 0xABCDEF12),
                    ('push-wrap', 1, 0xFFFFFFFF, 0xFFFFFFFF, 3, 0xFFFFFFFF),
                    ('push-wide-value', 0, 1, 0xFFFFFF, 4, 0x12345678),
                    ('full-stack-fatal', 0, 1, 7, 20, 0x12345678),
                    ('overfull-stack-fatal', 2, 1, 9, 21, 0xABCDEF12),
                    ('full-stack-disabled', 2, 0, 9, 20, 0xABCDEF12))]
    with tempfile.TemporaryDirectory(prefix='igi-modifier-diff-') as tmp:
        folder = Path(tmp)
        wrapper = folder/'candidate.cpp'
        wrapper.write_text('#include '+json.dumps(str(args.implementation.resolve()))+'''
extern "C" __declspec(dllexport) __attribute__((noinline)) int ErrorBoundary(const char* message) { asm volatile("" : : "r"(message) : "memory"); return 0; }
namespace igi::retail::error { int Error_Show(const char* message, ...) { return ErrorBoundary(message); } }
extern "C" __declspec(dllexport) void* ModifierEntry(void* context) {
    return igi::retail::terrain::TextureModifier_Push(context);
}
extern "C" __declspec(dllexport) void* ModifierGlobals[] = {
    &igi::retail::terrain::g_b97df8, igi::retail::terrain::g_b97da0.data(),
    &igi::retail::terrain::g_ba5060,
};
''')
        binary = folder/'candidate.dll'
        command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared',
                   '-ffunction-sections', '-fdata-sections', '-I'+str(args.root/'engine/include'), str(wrapper), str(args.root/'engine/src/retail/script/qvm_compiled_program.cpp'),
                   str(args.root/'engine/src/retail/script/qvm_object_calls.cpp'),
                   '-Wl,--gc-sections,--exclude-all-symbols,--image-base,0x10000000',
                   '-static-libgcc', '-static-libstdc++', '-o', str(binary)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=180)
        if built.returncode:
            raise RuntimeError('Candidate compilation failed: '+built.stderr[-3000:])
        pe = pefile.PE(str(binary)); base = pe.OPTIONAL_HEADER.ImageBase
        exports = {s.name.decode():base+s.address for s in pe.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
        globals_ = struct.unpack('<3I', pe.get_data(exports['ModifierGlobals']-base, 12))
        reference, candidate = PebImage(args.binary), PebImage(binary)
        cases = []
        for fixture in fixtures:
            expected = observe(reference, 0x00505E40, (0x00B97DF8, 0x00B97DA0, 0x00BA5060), 0x004AF7B0, fixture)
            actual = observe(candidate, exports['ModifierEntry'], globals_, exports['ErrorBoundary'], fixture)
            cases.append(dict(id=fixture['id'], categories=['nominal' if fixture['id']=='push-value' else 'boundary'],
                              input=fixture, reference_output=expected, candidate_output=actual,
                              result='passed' if expected == actual else 'failed'))
        candidate_hash = digest(binary)
    if (digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256
            or any(digest(path)!=value for path,value in dependencies.items())):
        raise ValueError('Inputs changed during comparison')
    failed = sum(c['result']!='passed' for c in cases)
    print(json.dumps(dict(schema_version=1, kind='behavioral-test-report', executed=True,
                          oracle='retail-binary-execution', result='failed' if failed else 'passed',
                          address='0x00505E40', binary_sha256=args.binary_sha256,
                          implementation_sha256=args.implementation_sha256, dependencies=dependencies,
                          observable_fields=['return_value','state_delta','side_effects'],
                          tests_total=len(cases), tests_passed=len(cases)-failed, tests_failed=failed, cases=cases,
                          runner=dict(path='scripts/texture_modifier_push_diff_runner.py', sha256=digest(__file__),
                                      command=[sys.executable,*sys.argv]),
                          retail_execution=dict(tool='Unicorn',version=unicorn.__version__,candidate_sha256=candidate_hash,
                                                candidate_build_command=command),
                          limitations=['Controlled nonaliasing PE32 contexts; null/unmapped pointers and negative stack counts excluded.',
                                       'Error_Show is modeled as an observed argument boundary returning zero; its body is not tested here.',
                                       'Fatal self-loop observed with an instruction budget; no natural game or rendering parity claim.'])))
    return int(bool(failed))

if __name__=='__main__':
    try:
        raise SystemExit(main())
    except (OSError,ValueError,RuntimeError,subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
