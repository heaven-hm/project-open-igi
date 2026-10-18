#!/usr/bin/env python3
"""Compare retail and compiled Windows x86 hash-list and FORM accessors."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import pathlib
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
    ilff_names = {"0x004B7470": "Ilff_GetNextChunk", "0x004B7490": "Ilff_GetFirstChunk",
                  "0x004B74D0": "Ilff_GetChunkPayload", "0x004B7590": "Ilff_FindChunk"}
    path_names = {"0x004B5AF0": "Resource_CanonicalizePath", "0x004B5B30": "Resource_CalculatePathHash"}
    if args.address not in {"0x004CF300", "0x004CEAA0", *ilff_names, *path_names}:
        raise ValueError("unsupported native address")
    binary, source = pathlib.Path(args.binary), pathlib.Path(args.implementation)
    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise ValueError("binary or implementation changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if not compiler:
        raise RuntimeError("Windows x86 compiler is unavailable")
    retail = angr.Project(str(binary), auto_load_libs=False)
    observations = []
    with tempfile.TemporaryDirectory(prefix="igi-hash-lookup-") as temp:
        wrapper = pathlib.Path(temp) / "entry.cpp"
        candidate = pathlib.Path(temp) / "candidate.dll"
        lookup_wrapper = ('#include "igi/retail/dynobj/hash_lookup.hpp"\n'
                           'extern "C" __declspec(dllexport) void* NativeEntry(void* p, int i) {\n'
                           'return igi::retail::dynobj::DynObj_LookupHashItem('
                           'static_cast<igi::retail::dynobj::HashItem**>(p),i); }\n')
        form_wrapper = ('#include "igi/retail/dynobj/hash_lookup.hpp"\n'
                        'extern "C" __declspec(dllexport) void* NativeEntry(void* p, int tag, int* size) {\n'
                        'return igi::retail::dynobj::Ilff_FindFormChunk(p, tag, size); }\n')
        if args.address in path_names:
            params = "char* out, char* source" if args.address == "0x004B5AF0" else "const char* source"
            call = "out, source" if args.address == "0x004B5AF0" else "source"
            wrapper.write_text('#include "igi/retail/sys/resource_lookup.hpp"\n'
                f'extern "C" __declspec(dllexport) unsigned NativeEntry({params}) {{\n'
                f'return igi::retail::sys::{path_names[args.address]}({call}); }}\n')
        elif args.address in ilff_names:
            name = ilff_names[args.address]
            extra = ", void* parent" if args.address == "0x004B74D0" else ", int tag" if args.address == "0x004B7590" else ""
            call = ("p, parent" if args.address == "0x004B74D0" else
                    "static_cast<int*>(p), tag" if args.address == "0x004B7590" else "p")
            wrapper.write_text('#include "igi/retail/sys/ilff_navigation.hpp"\n'
                'extern "C" { __declspec(dllexport) int NativeError = 0; }\n'
                f'extern "C" __declspec(dllexport) void* NativeEntry(void* p{extra}) {{\n'
                f'auto* result = igi::retail::sys::{name}({call});\n'
                'NativeError = igi::retail::sys::Ilff_GetErrorFlag(); return result; }\n')
        else:
            wrapper.write_text(lookup_wrapper if args.address == "0x004CF300" else form_wrapper)
        command = [compiler, "-std=c++20", "-O0", "-shared", f"-I{root / 'engine/include'}",
                   str(wrapper), str(source), "-o", str(candidate)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=90)
        if built.returncode:
            raise RuntimeError(built.stderr)
        rebuilt = angr.Project(str(candidate), auto_load_libs=False)
        entry = rebuilt.loader.find_symbol("NativeEntry")
        if entry is None or retail.arch.bits != 32 or rebuilt.arch.bits != 32:
            raise RuntimeError("expected PE32 candidate export and retail image")

        def execute(project, address, raw, arguments, prototype):
            base, stop = 0x50000000, 0x13370000
            state = project.factory.call_state(address, *arguments, ret_addr=stop,
                prototype=prototype,
                cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                             angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(base, bytes(raw))
            error_address = None
            if args.address in ilff_names:
                error_address = (0x00A43E5C if project is retail else
                                 rebuilt.loader.find_symbol("NativeError").rebased_addr)
                state.memory.store(error_address, b"\x00" * 4)
            manager = project.factory.simgr(state)
            manager.explore(find=stop, n=10000)
            if len(manager.found) != 1 or manager.errored:
                raise RuntimeError(f"execution did not return cleanly at {address:#x}: "
                                   f"{[str(item.error) for item in manager.errored]}")
            final = manager.found[0]
            memory = {"arena": final.solver.eval(final.memory.load(base, len(raw)), cast_to=bytes).hex()}
            if error_address is not None:
                memory["error_flag"] = final.solver.eval(final.memory.load(error_address, 4, endness="Iend_LE"))
            return {"return_value": final.solver.eval(final.regs.eax),
                    "state_delta": memory,
                    "side_effects": []}

        fixtures = []
        base = 0x50000000
        if args.address == "0x004CF300":
            for count in (1, 2, 4, 8):
                raw = bytearray(0x400)
                raw[:4] = (base + 0x100).to_bytes(4, "little")
                for node in range(count):
                    nxt = base + 0x100 + (node + 1) * 16 if node + 1 < count else 0
                    raw[0x100 + node * 16:0x104 + node * 16] = nxt.to_bytes(4, "little")
                for index in sorted({-2147483648, -1, 0, 1, count - 2, count - 1, count, 2147483647}):
                    fixtures.append((f"nodes-{count}-index-{index}", raw,
                        [base, index & 0xFFFFFFFF], "void* lookup(void** context, int index)",
                        "boundary" if index < 0 or index >= count - 1 else "nominal"))
        elif args.address == "0x004CEAA0":
            for size in (0, 1, 2, 3, 4, 7, 16):
                raw = bytearray(0x400)
                chunks = bytearray()
                for tag, length in ((b"AAAA", size), (b"BBBB", 3), (b"CCCC", 4)):
                    chunks += tag + length.to_bytes(4, "big") + b"x" * length
                    chunks += b"\x00" * ((-length) % 4)
                form = b"FORM" + (4 + len(chunks)).to_bytes(4, "big") + b"TEST" + chunks
                raw[:len(form)] = form
                raw[0x300:0x304] = b"\xa5" * 4
                for tag in (b"AAAA", b"BBBB", b"CCCC", b"NONE"):
                    for null_size in (False, True):
                        fixtures.append((f"size-{size}-tag-{tag.decode()}-null-{null_size}", raw,
                            [base, int.from_bytes(tag, "big"), 0 if null_size else base + 0x300],
                            "void* find(void* context, int tag, int* size)",
                            "boundary" if size == 0 or tag == b"NONE" or null_size else "nominal"))
        elif args.address in path_names:
            for index, value in enumerate((b"", b"a", b"Z", b"abcXYZ", b"a/b\\c.mef", b"..\\file.qvm",
                                           b"0_!", b"\x7f\x80\xff", b"z" * 200, b"a\x00ignored")):
                for inplace in ((False, True) if args.address == "0x004B5AF0" else (False,)):
                    raw = bytearray(b"\xa5" * 0x400)
                    raw[:len(value) + 1] = value + b"\x00"
                    arguments = ([base if inplace else base + 0x200, base]
                                 if args.address == "0x004B5AF0" else [base])
                    fixtures.append((f"string-{index}-inplace-{inplace}", raw, arguments,
                        "unsigned canonical(char*, char*)" if args.address == "0x004B5AF0" else "unsigned hash(const char*)",
                        "boundary" if index in (0, 7, 9) else "nominal"))
        else:
            def put(raw, offset, value):
                raw[offset:offset + 4] = (value & 0xFFFFFFFF).to_bytes(4, "little")

            if args.address == "0x004B7470":
                for offset in (0, 1, 4, 16, 32, 64, 127, 256):
                    raw = bytearray(0x400)
                    put(raw, 12, offset)
                    fixtures.append((f"next-offset-{offset}", raw, [base], "void* next(void*)",
                                     "boundary" if offset == 0 else "nominal"))
            elif args.address == "0x004B7490":
                for alignment in (0, 1, 2, 3, 4, 8, 16, 32):
                    for size in (0, 16, 20, 21, 64, 256, 0x80000000, 0xFFFFFFFF):
                        raw = bytearray(0x400)
                        put(raw, 4, size)
                        put(raw, 8, alignment)
                        fixtures.append((f"first-align-{alignment}-size-{size}", raw, [base],
                                         "void* first(void*)", "boundary" if size <= 21 else "nominal"))
            elif args.address == "0x004B74D0":
                for parent_alignment, child_alignment in ((4, 4), (8, 4), (4, 8), (3, 4), (4, 3), (0, 0)):
                    for offset, size, parent_size in ((32, 4, 256), (64, 0, 256), (256, 4, 256),
                                                      (32, 4, 0x80000000), (32, 4, 0xFFFFFFFF)):
                        raw = bytearray(0x400)
                        put(raw, 4, parent_size)
                        put(raw, 8, parent_alignment)
                        put(raw, offset + 4, size)
                        put(raw, offset + 8, child_alignment)
                        fixtures.append((f"payload-pa-{parent_alignment}-ca-{child_alignment}-offset-{offset}-size-{size}-parent-size-{parent_size}",
                            raw, [base + offset, base], "void* payload(void*, void*)", "boundary" if size == 0 else "nominal"))
            else:
                for count in (1, 2, 4):
                    for tag in (0, 1, 2, 3, 99):
                        raw = bytearray(0x400)
                        for node in range(count):
                            put(raw, node * 32, node)
                            put(raw, node * 32 + 12, 32 if node + 1 < count else 0)
                        fixtures.append((f"find-count-{count}-tag-{tag}", raw, [base, tag],
                                         "void* find(void*, int)", "boundary" if tag >= count else "nominal"))
            raw = bytearray(0x400)
            arity = 2 if args.address in {"0x004B74D0", "0x004B7590"} else 1
            fixtures.append(("null-context", raw, [0] * arity,
                             "void* null_case(void*, void*)" if arity == 2 else "void* null_case(void*)", "boundary"))
        for case_id, raw, arguments, prototype, category in fixtures:
            observations.append({"id": case_id, "input": {"args": arguments, "arena": raw.hex()},
                "categories": [category],
                "reference_output": execute(retail, int(args.address, 16), raw, arguments, prototype),
                "candidate_output": execute(rebuilt, entry.rebased_addr, raw, arguments, prototype)})
        candidate_hash = digest(candidate)
    failed = sum(case["reference_output"] != case["candidate_output"] for case in observations)
    report = {"schema_version": 1, "kind": "behavioral-test-report",
        "result": "failed" if failed else "passed", "executed": True,
        "oracle": "retail-binary-execution", "address": args.address,
        "binary_sha256": args.binary_sha256, "implementation_sha256": args.implementation_sha256,
        "method": "Concrete angr/VEX execution of retail bytes and freshly compiled Windows x86 candidate; exact return and entire fixture arena compared.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations), "tests_passed": len(observations) - failed,
        "tests_failed": failed, "cases": observations,
        "retail_execution": {"tool": "angr", "engine": "VEX", "version": angr.__version__,
            "function_entry": args.address, "binary_sha256": args.binary_sha256,
            "candidate_sha256": candidate_hash, "candidate_build_command": command},
        "limitations": ["Valid finite non-empty lists or bounded FORM chunks only; invalid pointers, cycles, malformed sizes and concurrent writes excluded.",
                         "Concrete emulation is not an original-game process or universal equivalence proof."]}
    print(json.dumps(report))
    return int(bool(failed))


if __name__ == "__main__":
    raise SystemExit(main())
