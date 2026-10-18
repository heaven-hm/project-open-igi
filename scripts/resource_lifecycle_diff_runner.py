#!/usr/bin/env python3
"""Compare lifecycle native bodies with identical modeled callee interfaces.

These fixtures verify branch/dispatch behavior, not VFS, registry, free, or
fatal-dialog implementations. Fatal paths must reach an actual self-loop.
"""
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
    for name in ("address", "binary", "binary-sha256", "implementation", "implementation-sha256", "root"):
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    root = pathlib.Path(args.root).resolve()
    try:
        import angr
    except ImportError:
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])
    if args.address not in {"0x004B6380", "0x004B5F00"}:
        raise ValueError("unsupported native")
    binary, source = pathlib.Path(args.binary), pathlib.Path(args.implementation)
    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise RuntimeError("binary or candidate changed")
    compiler = shutil.which("i686-w64-mingw32-g++")
    if compiler is None:
        raise RuntimeError("x86 compiler missing")
    wrapper_text = r'''
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/error/error_show.hpp"
#include "igi/retail/qfile/alias_resolve.hpp"
#include <cstdarg>
using namespace igi::retail::resource;
extern "C" {
__declspec(dllexport) int NativeLookup(const char*, ResourceHashNode**) { return 0; }
__declspec(dllexport) void* NativePack(const char*, int*) { return nullptr; }
__declspec(dllexport) void* NativeRegister(const char*, void*, int) { return nullptr; }
__declspec(dllexport) int NativeFree(ResourceHashNode*) { return 0; }
__declspec(dllexport) int NativeError(const char*, const char*) { return 0; }
}
namespace igi::retail::resource {
int Is_ResourceLoaded(const char* path, ResourceHashNode** out) { return NativeLookup(path,out); }
}
namespace igi::retail::qfile {
const char* QFile_GetResolvedAlias() { return reinterpret_cast<const char*>(0x00943A18); }
}
namespace igi::retail::error {
int Error_Show(const char* format, ...) {
    std::va_list args; va_start(args,format); const char* path=va_arg(args,const char*);
    va_end(args); return NativeError(format,path);
}
}
extern "C" __declspec(dllexport) void* NativeLoad(const char* path, int* size) {
    Host_SetResourcePackUnpack(NativePack); Host_SetSysResource(NativeRegister);
    return Resource_Load(path,size);
}
extern "C" __declspec(dllexport) int NativeUnload(const char* path) {
    Host_SetSysFreeMemory16(NativeFree); return Resource_Unload(path);
}
'''
    if args.address == "0x004B6380":
        fixtures = [{"id": f"kind-{kind}", "found": True, "kind": kind, "size": 31,
                     "packed": True, "null_size": False} for kind in (0, 1, 2, 3, 4, -1)]
        fixtures += [{"id": f"missing-{i}", "found": False, "kind": i, "size": i,
                      "packed": False, "null_size": False} for i in (0, 1)]
    else:
        fixtures = [{"id": f"found-{found}-packed-{packed}-null-{null_size}-size-{size}",
            "found": found, "packed": packed, "null_size": null_size, "size": size, "kind": 2}
            for found in (False, True) for packed in (False, True)
            for null_size in (False, True) for size in (0, 257)]
    retail = angr.Project(str(binary), auto_load_libs=False)
    observations = []
    with tempfile.TemporaryDirectory(prefix="igi-resource-lifecycle-") as temp:
        wrapper = pathlib.Path(temp) / "entry.cpp"
        candidate = pathlib.Path(temp) / "candidate.dll"
        wrapper.write_text(wrapper_text)
        command = [compiler, "-std=c++20", "-O0", "-shared", f"-I{root / 'engine/include'}",
                   str(wrapper), str(source), "-o", str(candidate)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=90)
        if built.returncode:
            raise RuntimeError(built.stderr)
        rebuilt = angr.Project(str(candidate), auto_load_libs=False)
        if retail.arch.bits != 32 or rebuilt.arch.bits != 32:
            raise RuntimeError("expected x86 images")
        entries = {"lookup": (0x004B5B90, "NativeLookup"), "pack": (0x004B18B0, "NativePack"),
                   "register": (0x004B5F80, "NativeRegister"), "free": (0x004B5C90, "NativeFree"),
                   "error": (0x004AF7B0, "NativeError")}

        def run(project, fixture, reference):
            base, node, payload, stop = 0x50000000, 0x50000100, 0x50000800, 0x13370000

            class Boundary(angr.SimProcedure):
                def __init__(self, operation, **kwargs):
                    self.operation = operation
                    super().__init__(**kwargs)

                def run(self, a, b=0, c=0):
                    op = self.operation
                    numbers = [self.state.solver.eval(value) if not isinstance(value,int) else value
                               for value in (a,b,c)]
                    events = tuple(self.state.globals.get("events", ()))
                    arity = {"lookup":1,"pack":1,"register":3,"free":1,"error":2}[op]
                    # Error format lives at different addresses in the two images.
                    if op == "error":
                        text = bytearray()
                        for offset in range(256):
                            value = self.state.solver.eval(self.state.memory.load(a+offset,1))
                            if value == 0: break
                            text.append(value)
                        event = [op, text.decode("latin1"), numbers[1]]
                    else:
                        event = [op, *numbers[:arity]]
                    self.state.globals["events"] = events + (event,)
                    if op == "lookup":
                        if fixture["found"]:
                            self.state.memory.store(b, node.to_bytes(4,"little"))
                        return int(fixture["found"])
                    if op == "pack":
                        self.state.memory.store(b, fixture["size"].to_bytes(4,"little"))
                        return payload if fixture["packed"] else 0
                    if op == "register": return payload + 0x40
                    if op == "free": return 0x12345678
                    return 0x789

            # Explicit prototypes prevent the model from guessing variadic arity.
            prototypes = {"lookup":"int f(char*,void**)", "pack":"void* f(char*,int*)",
                "register":"void* f(char*,void*,int)", "free":"int f(void*)", "error":"int f(char*,char*)"}
            for op, (retail_address, export) in entries.items():
                address = retail_address if reference else rebuilt.loader.find_symbol(export).rebased_addr
                project.hook(address, Boundary(op, prototype=prototypes[op],
                                              cc=angr.calling_conventions.SimCCCdecl(project.arch)), replace=True)
            load = args.address == "0x004B5F00"
            address = int(args.address,16) if reference else rebuilt.loader.find_symbol("NativeLoad" if load else "NativeUnload").rebased_addr
            arguments = [base]
            if load: arguments += [0 if fixture["null_size"] else base+0x80]
            state = project.factory.call_state(address,*arguments,ret_addr=stop,
                prototype="void* f(char*,int*)" if load else "int f(char*)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
            state.memory.store(base,b"\x00"*0x1000)
            state.memory.store(base,b"LOCAL:fixture\x00")
            state.memory.store(base+0x80,b"\xa5"*4)
            state.memory.store(node+0x24,(fixture["kind"] & 0xFFFFFFFF).to_bytes(4,"little"))
            state.memory.store(node+0x28,fixture["size"].to_bytes(4,"little"))
            state.memory.store(node+0x34,payload.to_bytes(4,"little"))
            state.memory.store(0x00943A18,b"LOCAL:resolved\x00")
            manager = project.factory.simgr(state)
            final, termination = None, None
            for _ in range(2000):
                if len(manager.active) != 1 or manager.errored:
                    raise RuntimeError(f"unexpected execution states: {[str(x.error) for x in manager.errored]}")
                active = manager.active[0]
                if active.addr == stop:
                    final,termination = active,"returned"
                    break
                if active.solver.eval(active.memory.load(active.addr,2),cast_to=bytes) == b"\xeb\xfe":
                    final,termination = active,"fatal-self-loop"
                    break
                manager.step()
            if final is None:
                raise RuntimeError("execution exceeded bound without return or an actual self-loop")
            return {"return_value": final.solver.eval(final.regs.eax) if termination=="returned" else None,
                    "state_delta":{"termination":termination,"arena":final.solver.eval(final.memory.load(base,0x1000),cast_to=bytes).hex()},
                    "side_effects":list(final.globals.get("events",()))}

        for fixture in fixtures:
            observations.append({"id":fixture["id"],"input":fixture,
                "categories":["nominal" if fixture["found"] else "boundary"],
                "reference_output":run(retail,fixture,True),"candidate_output":run(rebuilt,fixture,False)})
        candidate_hash = digest(candidate)
    failed = sum(case["reference_output"] != case["candidate_output"] for case in observations)
    report = {"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed",
        "executed":True,"oracle":"retail-binary-execution","address":args.address,
        "binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
        "method":"Retail and PE32 candidate native bodies executed with identical modeled resource callees; compared return/self-loop termination, caller arena and callee order/arguments.",
        "observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},
        "tests_total":len(observations),"tests_passed":len(observations)-failed,"tests_failed":failed,"cases":observations,
        "retail_execution":{"tool":"angr","engine":"VEX","version":angr.__version__,"function_entry":args.address,
            "candidate_sha256":candidate_hash,"candidate_build_command":command,"binary_sha256":args.binary_sha256},
        "limitations":["Lookup, unpack, register, free and fatal-error callees are models; their implementations are not verified by these fixtures.",
            "Candidate host callbacks are explicitly installed; unset callbacks, invalid nodes and the live game process are outside this scope."]}
    print(json.dumps(report))
    return int(bool(failed))


if __name__ == "__main__":
    raise SystemExit(main())
