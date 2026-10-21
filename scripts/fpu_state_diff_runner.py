#!/usr/bin/env python3
"""Concrete PE32 replay of retail 0x00498460 and the actual x86 candidate."""
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
TARGETS = {
    0x00498460: ('Display_ClearSurfaces_16', 0, False),
    0x004AC66B: ('Camera_ApplyBandFilter_24', 1, True),
    0x004AC6FD: ('Camera_ApplyWideScreen_24', 1, True),
    0x004AC620: ('Camera_SetFieldOfView_24', 2, True),
    0x004AC655: ('Camera_ApplyLineFilter_24', 2, True),
    0x004A7157: ('Camera_ApplyBandFilter_10', 0, True),
}


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def observe(image, entry, global_address, control, arguments, has_return):
    from native_unicorn_runner import new_emulator, run_with_arguments
    from unicorn import x86_const as x86
    emulator = new_emulator(image)
    emulator.mem_write(global_address, b'\xa5' * 8)
    emulator.reg_write(x86.UC_X86_REG_FPCW, control)
    emulator.reg_write(x86.UC_X86_REG_EAX, 0x12345678)
    emulator.reg_write(x86.UC_X86_REG_EFLAGS, 0x202)
    run_with_arguments(emulator, entry, arguments, 0x70018000)
    registers = {name: emulator.reg_read(getattr(x86, 'UC_X86_REG_' + name))
                 for name in (('EBX', 'ESI', 'EDI', 'EBP', 'FPSW', 'FPCW') if has_return else
                              ('EAX', 'EBX', 'ECX', 'EDX', 'ESI', 'EDI', 'EBP', 'EFLAGS', 'FPSW', 'FPCW'))}
    return dict(return_value=emulator.reg_read(x86.UC_X86_REG_EAX) if has_return else None, state_delta=registers,
                side_effects=dict(saved_control_and_adjacent_bytes=bytes(emulator.mem_read(global_address, 8)).hex()))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--address', default='0x00498460')
    parser.add_argument('--root', type=Path, default=ROOT)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--binary-sha256', required=True)
    parser.add_argument('--implementation', type=Path, required=True)
    parser.add_argument('--implementation-sha256', required=True)
    args = parser.parse_args()
    address = int(args.address, 16)
    if address not in TARGETS:
        raise ValueError('Unsupported retail FPU entry')
    name, argument_count, has_return = TARGETS[address]
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('Input identities differ from the requested evidence')
    try:
        import unicorn
        import pefile
    except ImportError:
        interpreter = os.environ.get('IGI_ANGR_PYTHON', str(ROOT/'.venv-angr/bin/python'))
        if os.environ.get('IGI_FPU_RUNNER_REEXEC'):
            raise RuntimeError('Isolated interpreter requires Unicorn and pefile')
        os.execve(interpreter, [interpreter, str(Path(__file__).resolve()), *sys.argv[1:]],
                  dict(os.environ, IGI_FPU_RUNNER_REEXEC='1'))
    from native_unicorn_runner import PebImage
    with tempfile.TemporaryDirectory(prefix='igi-fpu-diff-') as temporary:
        folder = Path(temporary)
        wrapper = folder/'fpu.cpp'
        arguments = ['std::uint32_t first', 'std::uint32_t second'][:argument_count]
        invocation = f'igi::retail::core::{name}(' + ','.join(['first','second'][:argument_count]) + ');'
        wrapper.write_text('#include ' + json.dumps(str(args.implementation.resolve())) + '\n' +
                          'extern "C" __declspec(dllexport) ' + ('std::uint32_t' if has_return else 'void') +
                          ' FpuEntry(' + ','.join(arguments) + ') { ' + ('return ' if has_return else '') + invocation + '}\n' +
                          'extern "C" __declspec(dllexport) void* FpuGlobal = &igi::retail::core::g_6e5bc4;\n')
        dll = folder/'fpu.dll'
        command = ['i686-w64-mingw32-g++', '-std=c++20', '-O2', '-shared',
                   '-ffunction-sections', '-fdata-sections', '-I'+str(ROOT/'engine/include'),
                   str(wrapper), '-Wl,--gc-sections,--image-base,0x10000000', '-o', str(dll)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=120)
        if built.returncode:
            raise RuntimeError('Candidate compile failed: ' + built.stderr[-3000:])
        pe = pefile.PE(str(dll))
        base = pe.OPTIONAL_HEADER.ImageBase
        exports = {s.name.decode(): base+s.address for s in pe.DIRECTORY_ENTRY_EXPORT.symbols if s.name}
        global_address, = struct.unpack('<I', pe.get_data(exports['FpuGlobal']-base, 4))
        retail, candidate = PebImage(args.binary), PebImage(dll)
        cases = []
        controls = (0x037F, 0x027F, 0x007F, 0x077F, 0x0B7F, 0x0F7F, 0x067F, 0x0E7F)
        pure_inputs = None
        if address == 0x004AC66B:
            pure_inputs = (0, 1, 2, 4, 8, 16, 32, 0x100, 0x200, 0x300, 0x400, 0x800, 0xC00, 0x1000, 0x1F3F, 0xFFFF)
        elif address == 0x004AC6FD:
            pure_inputs = (0, 1, 2, 4, 8, 0x10, 0x80000, 0x100, 0x200, 0x300, 0x10000, 0x20000, 0x30000, 0x40000, 0xFFFFF, 0xFFFFFFFF)
        if pure_inputs is not None: controls = (0x037F,) * len(pure_inputs)
        for index, control in enumerate(controls):
            arguments = []
            if pure_inputs is not None: arguments = [pure_inputs[index]]
            elif argument_count == 2:
                arguments = [((index % 3) << 16) | ((index % 4) << 8),
                             0x30000 | 0x300 | (0x80000 if index % 2 else 0)]
            expected = observe(retail, address, 0x006E5BC4, control, arguments, has_return)
            actual = observe(candidate, exports['FpuEntry'], global_address, control, arguments, has_return)
            cases.append(dict(id=f'case-{index}-control-{control:04x}', categories=['nominal' if index<3 else 'boundary'],
                              input=dict(control_word=control, arguments=arguments, eax=0x12345678, flags=0x202),
                              reference_output=expected, candidate_output=actual,
                              result='passed' if expected == actual else 'failed'))
        candidate_hash = digest(dll)
    if digest(args.binary) != args.binary_sha256 or digest(args.implementation) != args.implementation_sha256:
        raise ValueError('Inputs changed during replay')
    failed = sum(case['result'] != 'passed' for case in cases)
    report = dict(schema_version=1, kind='behavioral-test-report', executed=True,
                  oracle='retail-binary-execution', result='failed' if failed else 'passed',
                  address=f'0x{address:08X}', binary_sha256=args.binary_sha256,
                  implementation_sha256=args.implementation_sha256,
                  observable_fields=['return_value', 'state_delta', 'side_effects'],
                  tests_total=len(cases), tests_passed=len(cases)-failed, tests_failed=failed, cases=cases,
                  runner=dict(path='scripts/fpu_state_diff_runner.py', sha256=digest(__file__),
                              command=[sys.executable, *sys.argv]),
                  retail_execution=dict(tool='Unicorn', version=unicorn.__version__,
                                        candidate_sha256=candidate_hash, candidate_build_command=command),
                  limitations=['Eight hardware-control fixtures, or sixteen scalar conversion fixtures; pending x87 exceptions and unmasked traps excluded.',
                               'No whole-game or all floating-point calculations equivalence claim.'])
    print(json.dumps(report))
    return int(bool(failed))


if __name__ == '__main__':
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError, subprocess.SubprocessError) as error:
        raise SystemExit(str(error))
