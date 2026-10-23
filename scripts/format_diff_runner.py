#!/usr/bin/env python3
"""Compare retail CRT-format wrappers with freshly built Windows x86 candidates.

Retail formatting instructions execute directly. The candidate's imported
vsprintf is modeled with the host C runtime on a 512-byte
fixture. This models the CRT boundary and does not establish full CRT parity.
"""
from __future__ import annotations

import argparse
import ctypes
import hashlib
import json
import os
import pathlib
import re
import shutil
import subprocess
import sys
import tempfile


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    for key in ("address", "binary", "binary-sha256", "implementation", "implementation-sha256", "root"):
        parser.add_argument("--" + key, required=True)
    args = parser.parse_args()
    root = pathlib.Path(args.root).resolve()
    try:
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("configured interpreter cannot import angr")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])
    if args.address not in {"0x004A5702", "0x004A53B3"}:
        raise ValueError("unsupported format wrapper")
    binary, source = pathlib.Path(args.binary), pathlib.Path(args.implementation)
    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise RuntimeError("binary or implementation changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if not compiler:
        raise RuntimeError("Windows x86 compiler is missing")
    retail = angr.Project(str(binary), auto_load_libs=False)
    fixtures = [("empty", "", 0, 0), ("literal", "hello", 0, 0),
                ("integer", "value=%d", 42, 0), ("negative", "%d", -17, 0),
                ("unsigned", "%u", 0xFFFFFFFF, 0), ("hex-width", "%08x", 0x1234AB, 0),
                ("string", "name=%s", 0x50001300, 0),
                ("mixed", "%d:%s", 7, 0x50001300), ("escape", "%%=%d", 9, 0),
                ("padding", "%+06d", 12, 0)]
    observations = []
    with tempfile.TemporaryDirectory(prefix="igi-format-diff-") as temp:
        wrapper = pathlib.Path(temp) / "entry.cpp"
        candidate = pathlib.Path(temp) / "candidate.dll"
        if args.address == "0x004A5702":
            signature = "char* out, const char* format, char* arguments"
            call = "String_VFormat(out, format, arguments)"
        elif args.address == "0x004A53B3":
            # The port's sprintf is variadic and forwards to String_VFormat, so the
            # harness marshals the fixed cases into matching integer argument pairs.
            signature = "char* out, const char* format, unsigned a, unsigned b"
            call = "sprintf(out, format, a, b)"
        else:
            signature = "char* out, const char* format, unsigned a, unsigned b"
            call = "sprintf(out, format, a, b)"
        wrapper.write_text('#include "igi/retail/error/error_show.hpp"\n'
            f'extern "C" __declspec(dllexport) int NativeEntry({signature}) {{\n'
            f'return igi::retail::error::{call}; }}\n')
        command = [compiler, "-std=c++20", "-O0", "-D__USE_MINGW_ANSI_STDIO=0", "-shared", f"-I{root / 'engine/include'}",
                   str(wrapper), str(source), "-o", str(candidate)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=90)
        if built.returncode:
            raise RuntimeError(built.stderr)
        rebuilt = angr.Project(str(candidate), auto_load_libs=False)

        def model_format(state, out, fmt, ap):
            cursor = ap
            def argument():
                nonlocal cursor
                value = state.solver.eval(state.memory.load(cursor, 4, endness="Iend_LE"))
                cursor += 4
                return value
            def string_at(pointer):
                result = bytearray()
                for offset in range(256):
                    value = state.solver.eval(state.memory.load(pointer + offset, 1))
                    if value == 0:
                        return bytes(result)
                    result.append(value)
                raise RuntimeError("unterminated modeled string")
            format_bytes = string_at(fmt)
            values = []
            offset = 0
            while offset < len(format_bytes):
                if format_bytes[offset:offset + 1] != b"%":
                    offset += 1
                    continue
                match = re.match(rb"%(?:%|[-+ 0#]*[0-9]*[diuxs])", format_bytes[offset:])
                if match is None:
                    raise RuntimeError("unsupported modeled format conversion")
                conversion = match.group()[-1:]
                offset += len(match.group())
                if conversion == b"%":
                    continue
                value = argument()
                values.append(ctypes.c_char_p(string_at(value)) if conversion == b"s" else
                              ctypes.c_int32(value) if conversion in (b"d", b"i") else ctypes.c_uint32(value))
            runtime = ctypes.CDLL(None)
            runtime.snprintf.argtypes = [ctypes.c_void_p, ctypes.c_size_t, ctypes.c_char_p]
            runtime.snprintf.restype = ctypes.c_int
            output = ctypes.create_string_buffer(512)
            length = runtime.snprintf(output, 512, format_bytes, *values)
            if length < 0 or length >= 512:
                raise RuntimeError("format output exceeds bounded fixture")
            state.memory.store(out, output.raw[:length + 1])
            return length

        class CandidateVsprintf(angr.SimProcedure):
            def run(self, out, fmt, ap):
                return model_format(self.state, out, fmt, ap)

        class CandidateSprintf(angr.SimProcedure):
            # __mingw_sprintf is cdecl variadic: at entry [esp]=return address,
            # [esp+4]=out, [esp+8]=fmt, [esp+12]=first substitution.
            def run(self, out, fmt):
                ap = self.state.regs.esp + 12
                return model_format(self.state, out, fmt, ap)

        # The candidate CRT boundary differs per address: String_VFormat reaches
        # vsprintf, while the sprintf entry is mingw's __mingw_sprintf (its
        # __mingw_pformat engine is not the vsprintf boundary).
        if args.address == "0x004A53B3":
            symbol = next((s for name in ("__mingw_sprintf", "___mingw_sprintf", "sprintf")
                           for s in rebuilt.loader.symbols if s.name == name), None)
            procedure = CandidateSprintf
        else:
            # MinGW may emit an inline C++ shim instead of an imported CRT symbol.
            symbol = next((s for name in ("__Z8vsprintfPcPKcS_", "vsprintf", "___mingw_vsprintf")
                           for s in rebuilt.loader.symbols if s.name == name), None)
            procedure = CandidateVsprintf
        entry = rebuilt.loader.find_symbol("NativeEntry")
        if symbol is None or entry is None:
            raise RuntimeError(f"candidate CRT import or entry is missing: "
                               f"{[s.name for s in rebuilt.loader.symbols if 'printf' in s.name or 'NativeEntry' in s.name]}")
        rebuilt.hook(symbol.rebased_addr, procedure())
        if retail.arch.bits != 32 or rebuilt.arch.bits != 32:
            raise RuntimeError("expected two Windows x86 images")

        def execute(project, entry_address, fmt, a, b):
            values = [0x50000000, 0x50001000]
            proto = "int format(char*, char*, unsigned, unsigned)"
            if args.address == "0x004A5702":
                values += [0x50001200]
                proto = "int format(char*, char*, char*)"
            else:
                values += [a & 0xFFFFFFFF, b & 0xFFFFFFFF]
            state = project.factory.call_state(entry_address, *values, ret_addr=0x13370000,
                prototype=proto, cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                             angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(0x50000000, b"\xa5" * 512)
            state.memory.store(0x50001000, fmt.encode() + b"\x00")
            state.memory.store(0x50001200, (a & 0xFFFFFFFF).to_bytes(4, "little") +
                               (b & 0xFFFFFFFF).to_bytes(4, "little"))
            state.memory.store(0x50001300, b"fixture\x00")
            manager = project.factory.simgr(state)
            manager.explore(find=0x13370000, n=20000)
            if len(manager.found) != 1 or manager.errored:
                raise RuntimeError(f"execution did not return: {[str(x.error) for x in manager.errored]}")
            final = manager.found[0]
            return {"return_value": final.solver.eval(final.regs.eax),
                    "state_delta": {"destination": final.solver.eval(final.memory.load(0x50000000, 512), cast_to=bytes).hex()},
                    "side_effects": []}

        for case_id, fmt, a, b in fixtures:
            observations.append({"id": case_id, "input": {"args": [fmt, a, b]},
                "categories": ["boundary" if case_id in {"empty", "negative", "unsigned"} else "nominal"],
                "reference_output": execute(retail, int(args.address, 16), fmt, a, b),
                "candidate_output": execute(rebuilt, entry.rebased_addr, fmt, a, b)})
        candidate_hash = digest(candidate)
    failed = sum(case["reference_output"] != case["candidate_output"] for case in observations)
    report = {"schema_version": 1, "kind": "behavioral-test-report", "result": "failed" if failed else "passed",
        "executed": True, "oracle": "retail-binary-execution", "address": args.address,
        "binary_sha256": args.binary_sha256, "implementation_sha256": args.implementation_sha256,
        "method": "Retail formatter instructions executed in VEX; candidate PE32 CRT boundary modeled by host libc snprintf with explicit x86 va_list dword reads for bounded integer/string formats. Exact return and 512 destination bytes compared.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations), "tests_passed": len(observations)-failed,
        "tests_failed": failed, "cases": observations,
        "retail_execution": {"tool": "angr", "engine": "VEX", "version": angr.__version__,
            "function_entry": args.address, "binary_sha256": args.binary_sha256,
            "modeled_candidate_crt_boundary": symbol.name,
            "candidate_sha256": candidate_hash, "candidate_build_command": command},
        "limitations": ["Candidate CRT is a SimProcedure model, not actual Windows CRT execution.",
            "Only bounded integer/string C-locale fixtures; no floating formats, overflow, invalid pointers or CRT failures.",
            "No original-game runtime capture or universal equivalence proof."]}
    print(json.dumps(report))
    return int(bool(failed))


if __name__ == "__main__":
    raise SystemExit(main())
