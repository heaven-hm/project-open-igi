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


FREE_NODE_ADDRESS = "0x004B5C90"


def run_free_node(args, root, binary, source):
    """Differentially execute all Resource_FreeNode kind branches on matching x86 fixtures."""
    import angr

    if digest(binary) != args.binary_sha256 or digest(source) != args.implementation_sha256:
        raise RuntimeError("binary or candidate changed")
    compiler = shutil.which(os.environ.get("CXX", "c++"))
    if compiler is None:
        raise RuntimeError("C++ compiler missing")
    wrapper = r'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include "igi/retail/resource/sys_resource.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/sys/resource_lookup.hpp"
using namespace igi::retail::resource;
std::uint32_t observed[8] = {};
std::uint32_t freed_payload = 0;
static std::int32_t free_payload(void* p) {
    freed_payload = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p));
    return 0x1357;
}
int NativeFree(int kind, int active) {
    Resource_InitPoolForTest(4);
    Host_SetSysOpenFileStream5(&free_payload);
    std::int32_t meta[2] = {0, 0};
    auto* first = static_cast<ResourceHashNode*>(Resource_CreateNode(
        "LOCAL:one.bin", nullptr, reinterpret_cast<void*>(0x12345678), 17, kind, meta));
    ResourceHashNode* second = nullptr;
    if (active > 1) second = static_cast<ResourceHashNode*>(Resource_CreateNode(
        "LOCAL:two.bin", nullptr, reinterpret_cast<void*>(0x87654321), 19, 3, meta));
    static void* child_header[3] = {};
    if (kind == 1 || kind == 4) {
        first->link_08[0] = &child_header[0];
        first->link_08[1] = &child_header[0];
        child_header[1] = &first->link_08[0];
        child_header[2] = &first->link_08[0];
    }
    const int result = Resource_FreeNode(first);
    observed[0] = static_cast<std::uint32_t>(result);
    observed[1] = static_cast<std::uint32_t>(Resource_GetActiveCount());
    observed[2] = first->prev == nullptr;
    observed[3] = first->next == nullptr;
    observed[4] = first->link_08[0] == nullptr;
    observed[5] = first->link_08[1] == nullptr;
    observed[6] = second ? static_cast<std::uint32_t>(second->pool_active_idx) : 0xffffffffU;
    observed[7] = second ? (second->prev != nullptr && second->next != nullptr) : 0U;
    return result;
}
int main(int argc, char** argv) {
    if (argc != 3) return 2;
    const int kind = std::atoi(argv[1]);
    const int active = std::atoi(argv[2]);
    NativeFree(kind, active);
    std::printf("%u %u %u %u %u %u %u %u %u\n", observed[0], observed[1], observed[2],
        observed[3], observed[4], observed[5], observed[6], observed[7], freed_payload);
    return 0;
}
namespace igi::retail::resource {
static ResourceHashNode test_root;
ResourceHashNode& Resource_GetHashRoot() { return test_root; }
void Host_SetSysFreeMemory16(SysFreeMemory16Fn) {}
void Host_SetSysResource(SysResourceFn) {}
}
namespace igi::retail::sys {
std::int32_t Resource_CanonicalizePath(char* out, char* in) {
    if (!in) { *out = 0; return 0; }
    for (; *in; ++in, ++out) { char c = *in; *out = c >= 'a' && c <= 'z' ? c - 'a' + 'A' : c; }
    *out = 0; return 0;
}
std::uint32_t Resource_CalculatePathHash(const char* in) {
    std::uint32_t h = 1; for (; *in; ++in) h = (h + 1) * static_cast<unsigned char>(*in);
    return h % 0x3037;
}
}
namespace igi::retail::error { int Error_Show(const char*, ...) { return 0; } }
'''
    fixtures = [(kind, active) for kind in (0, 1, 2, 3, 4, 5, -1) for active in (1, 2)]
    retail = angr.Project(str(binary), auto_load_libs=False)
    observations = []
    with tempfile.TemporaryDirectory(prefix="igi-free-node-") as temp:
        tempdir = pathlib.Path(temp)
        wrapper_path = tempdir / "entry.cpp"
        candidate = tempdir / "candidate"
        wrapper_path.write_text(wrapper, encoding="utf-8")
        dead_sections = ["-Wl,-dead_strip"] if sys.platform == "darwin" else ["-Wl,--gc-sections"]
        command = [compiler, "-std=c++20", "-O0", "-ffunction-sections", "-fdata-sections",
                   f"-I{root / 'engine/include'}",
                   str(wrapper_path), str(source), str(root / "engine/src/retail/mem/heap_list.cpp"),
                   str(root / "engine/src/retail/sys/ilff_navigation.cpp"), *dead_sections, "-o", str(candidate)]
        built = subprocess.run(command, capture_output=True, text=True, timeout=90)
        if built.returncode:
            raise RuntimeError(f"candidate x86 build failed: {built.stderr[-4000:]}")
        stop = 0x13370000

        class ReleasePayload(angr.SimProcedure):
            def run(self, payload):
                value = self.state.solver.eval(payload)
                events = tuple(self.state.globals.get("events", ()))
                self.state.globals["events"] = events + (("release", value),)
                return 0x1357

        retail.hook(0x004B1A30, ReleasePayload(prototype="int f(void*)",
            cc=angr.calling_conventions.SimCCCdecl(retail.arch)), replace=True)

        def execute(project, kind, active, reference):
            if reference:
                node, pool, indexes, list_root = 0x60000100, 0x60000000, 0x60010000, 0x60020000
                state = project.factory.call_state(0x004B5C90, node, ret_addr=stop,
                    prototype="int f(void*)", cc=angr.calling_conventions.SimCCCdecl(project.arch),
                    add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                                 angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
                stride, active_index_offset = 0x248, 0x244
                state.memory.store(0x00943E20, pool, endness="Iend_LE")
                state.memory.store(0x00943E24, indexes, endness="Iend_LE")
                state.memory.store(0x00943E28, active, endness="Iend_LE")
                state.memory.store(0x00943E30, stride, endness="Iend_LE")
                state.memory.store(0x00943E34, active_index_offset, endness="Iend_LE")
                state.memory.store(indexes, b"\x00\x00\x00\x00\x01\x00\x00\x00")
                state.memory.store(node + 0x24, (kind & 0xffffffff).to_bytes(4, "little"))
                state.memory.store(node + 0x28, (17).to_bytes(4, "little"))
                state.memory.store(node + 0x34, (0x12345678).to_bytes(4, "little"))
                state.memory.store(node + active_index_offset, (0).to_bytes(4, "little"))
                second = pool + stride
                state.memory.store(second + 0x24, (3).to_bytes(4, "little"))
                state.memory.store(second + active_index_offset, (1).to_bytes(4, "little"))
                # Global resource list: one or two nodes rooted at 0x00943E40.
                state.memory.store(0x00943E44, node, endness="Iend_LE")
                state.memory.store(0x00943E48, node, endness="Iend_LE")
                state.memory.store(node, 0x00943E44, endness="Iend_LE")
                state.memory.store(node + 4, 0x00943E44, endness="Iend_LE")
                if active > 1:
                    state.memory.store(0x00943E44, second, endness="Iend_LE")
                    state.memory.store(0x00943E48, second, endness="Iend_LE")
                    state.memory.store(second, 0x00943E44, endness="Iend_LE")
                    state.memory.store(second + 4, node, endness="Iend_LE")
                # Empty child list and, for kinds 1/4, a valid singleton child-link list.
                state.memory.store(node + 0x14, node + 0x18, endness="Iend_LE")
                state.memory.store(node + 0x18, 0, endness="Iend_LE")
                if kind in (1, 4):
                    child_root = 0x60030000
                    state.memory.store(node + 8, child_root + 4, endness="Iend_LE")
                    state.memory.store(node + 12, child_root + 4, endness="Iend_LE")
                    state.memory.store(child_root + 4, node + 8, endness="Iend_LE")
                    state.memory.store(child_root + 8, node + 8, endness="Iend_LE")
            else:
                actual = subprocess.run([str(candidate), str(kind), str(active)], cwd=root,
                    text=True, capture_output=True, check=False, timeout=10)
                if actual.returncode != 0:
                    raise RuntimeError(f"candidate fixture {kind}/{active} failed: {actual.stderr.strip()}")
                values = [int(value, 0) for value in actual.stdout.split()]
                if len(values) != 9:
                    raise RuntimeError(f"candidate fixture {kind}/{active} returned malformed output: {actual.stdout!r}")
                _, count, detached_prev, detached_next, child_prev, child_next, second_idx, _, freed = values
                return {"return_value": None,
                    "state_delta": {"active_count": count, "first_detached": bool(detached_prev and detached_next),
                                    "child_detached": bool(child_prev and child_next) if kind in (1,4) else True,
                                    "second_active_index": second_idx if active>1 else None},
                    "side_effects": [["release",freed]] if freed else []}
            manager = project.factory.simgr(state)
            final = None
            for _ in range(12000):
                if len(manager.active) != 1 or manager.errored:
                    raise RuntimeError(f"fixture {kind}/{active}: execution error: "
                        f"{[str(e.error) for e in manager.errored]}")
                current = manager.active[0]
                if current.addr == stop:
                    final = current
                    break
                manager.step()
            if final is None:
                raise RuntimeError(f"fixture {kind}/{active}: exceeded execution bound")
            if reference:
                return {"return_value": None,
                        "state_delta": {"active_count": final.solver.eval(final.memory.load(0x00943E28, 4, endness="Iend_LE")),
                                        "first_detached": final.solver.eval(final.memory.load(node, 4, endness="Iend_LE")) == 0 and final.solver.eval(final.memory.load(node+4, 4, endness="Iend_LE")) == 0,
                                        "child_detached": (kind not in (1,4)) or (final.solver.eval(final.memory.load(node+8,4,endness="Iend_LE")) == 0 and final.solver.eval(final.memory.load(node+12,4,endness="Iend_LE")) == 0),
                                        "second_active_index": final.solver.eval(final.memory.load(second+active_index_offset,4,endness="Iend_LE")) if active>1 else None},
                        "side_effects": [list(event) for event in final.globals.get("events", ())] }
            raise AssertionError("unreachable")

        for kind, active in fixtures:
            reference = execute(retail, kind, active, True)
            candidate_output = execute(None, kind, active, False)
            observations.append({"id":f"kind-{kind}-active-{active}",
                "input":{"args":[kind,active],"initial_state":{"pool_capacity":4,"payload":"0x12345678"}},
                "categories":["edge" if kind in (5,-1) else ("boundary" if kind in (0,4) or active == 1 else "nominal")],
                "reference_output":reference,"candidate_output":candidate_output})
    failed = sum(c["reference_output"] != c["candidate_output"] for c in observations)
    report = {"schema_version":1,"kind":"behavioral-test-report","result":"failed" if failed else "passed",
        "executed":True,"oracle":"retail-binary-execution","address":args.address,
        "binary_sha256":args.binary_sha256,"implementation_sha256":args.implementation_sha256,
        "method":"Retail Resource_FreeNode at 0x004B5C90 was executed under angr/VEX; the host candidate was executed natively from equivalent resource fixtures. Compared caller-observable pool count/index updates, link detachment, and modeled payload-release callback arguments. The retail EAX value is omitted because analyzed callers discard it.",
        "observable_fields":["return_value","state_delta","side_effects"],"float_tolerance":{"absolute":0,"relative":0},
        "tests_total":len(observations),"tests_passed":len(observations)-failed,"tests_failed":failed,"cases":observations,
        "limitations":["Retail and candidate execute in different environments (angr/VEX versus host-native); layouts and observations are normalized, but this is not an identical-instruction-engine comparison.",
                       "Payload release is modeled at the Sys_OpenFileStream_5 boundary; the file subsystem itself is outside this native's comparison.",
                       "Container case uses an empty child list; recursive multi-child teardown remains outside this fixture.",
                       "Candidate fixture setup constructs equivalent pool/list state through test helpers; it does not compare Resource_CreateNode."]}
    print(json.dumps(report,separators=(",",":"),sort_keys=True))
    return int(bool(failed))


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
    if args.address == FREE_NODE_ADDRESS:
        return run_free_node(args, root, pathlib.Path(args.binary), pathlib.Path(args.implementation))
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
