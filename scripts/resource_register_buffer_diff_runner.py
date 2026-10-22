#!/usr/bin/env python3
"""Compare raw-buffer Resource_RegisterBuffer paths against retail x86 code."""
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


ADDRESS = "0x004B5F80"
BINARY_SHA256 = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"

FIXTURES = [
    {"id": "empty-buffer", "length": 0, "data": "" , "category": "boundary"},
    {"id": "short-seven", "length": 7, "data": "1234567", "category": "boundary"},
    {"id": "short-negative", "length": -1, "data": "12345678", "category": "edge"},
    {"id": "prefix-mismatch-first", "length": 8, "data": "XESOURCE", "category": "boundary"},
    {"id": "prefix-mismatch-middle", "length": 8, "data": "RESXURCE", "category": "nominal"},
    {"id": "prefix-mismatch-last", "length": 8, "data": "RESOURCX", "category": "nominal"},
    {"id": "nonarchive-eight", "length": 8, "data": "NOTARCH!", "category": "nominal"},
    {"id": "nonarchive-long", "length": 19, "data": "QVM:script-data-01", "category": "nominal"},
]


def sha256(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def signed32(value: int) -> int:
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


def run(args: argparse.Namespace) -> int:
    import angr

    root = pathlib.Path(args.root).resolve()
    binary = pathlib.Path(args.binary).resolve()
    source = pathlib.Path(args.implementation).resolve()
    if sha256(binary) != args.binary_sha256 or sha256(source) != args.implementation_sha256:
        raise RuntimeError("retail binary or implementation changed")
    if args.binary_sha256 != BINARY_SHA256:
        raise RuntimeError("runner fixtures are bound to the recorded retail IGI image")

    compiler = shutil.which(os.environ.get("CXX", "c++"))
    if compiler is None:
        raise RuntimeError("host C++ compiler missing")

    wrapper = r'''#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "igi/retail/resource/sys_resource.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/resource/resource_lifecycle.hpp"
#include "igi/retail/sys/resource_lookup.hpp"
using namespace igi::retail::resource;
static std::uint32_t alloc_calls;
static char alloc_path[128];
static std::int32_t allocate_meta(std::int32_t* out, const char* path) {
    ++alloc_calls;
    std::snprintf(alloc_path, sizeof(alloc_path), "%s", path ? path : "");
    out[0] = 7; out[1] = 11;
    return 0;
}
namespace igi::retail::resource {
static ResourceHashNode test_root;
ResourceHashNode& Resource_GetHashRoot() { return test_root; }
void Sys_SetFreeMemory16(SysFreeMemory16Fn) {}
void Sys_SetResource(SysResourceFn) {}
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
int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const int fixture = std::atoi(argv[1]);
    static const char* data[] = {"", "1234567", "12345678", "XESOURCE", "RESXURCE",
                                 "RESOURCX", "NOTARCH!", "QVM:script-data-01"};
    static const int lengths[] = {0, 7, -1, 8, 8, 8, 8, 19};
    if (fixture < 0 || fixture >= 8) return 3;
    Resource_InitPoolForTest(4);
    Sys_SetAllocateMemory5(&allocate_meta);
    auto* buffer = const_cast<char*>(data[fixture]);
    void* result = Resource_RegisterBuffer("LOCAL:runtime-probe.qvm", buffer, lengths[fixture]);
    auto* node = static_cast<ResourceHashNode*>(Resource_GetHashRoot().next);
    if (!node) return 4;
    std::printf("%u %u %u %d %d %d %d %d\n", result == buffer, alloc_calls,
        std::strcmp(alloc_path, "LOCAL:runtime-probe.qvm") == 0, node->kind, node->size,
        node->loaded_flag, Resource_GetActiveCount(), node->meta_8f);
    return 0;
}
'''

    observations = []
    retail = angr.Project(str(binary), auto_load_libs=False)
    stop = 0x13370000
    buffer = 0x60002000
    name = 0x60001000
    node = 0x60003000

    class Boundary(angr.SimProcedure):
        def __init__(self, operation: str, fixture: dict, **kwargs):
            self.operation, self.fixture = operation, fixture
            super().__init__(**kwargs)

        def read_cstring(self, address):
            result = bytearray()
            for offset in range(256):
                value = self.state.solver.eval(self.state.memory.load(address + offset, 1))
                if value == 0:
                    break
                result.append(value)
            return result.decode("latin1")

        def run(self, *argv):
            op = self.operation
            values = [self.state.solver.eval(value) if not isinstance(value, int) else value
                      for value in argv]
            if op == "allocate":
                self.state.memory.store(values[0], (7).to_bytes(4, "little") + (11).to_bytes(4, "little"))
                self.state.globals["allocate_args"] = [1, self.read_cstring(values[1]) == "LOCAL:runtime-probe.qvm"]
                return 0
            if op == "tag":
                return self.state.solver.eval(self.state.memory.load(values[0], 4, endness="Iend_LE"))
            if op == "form":
                return self.state.solver.eval(self.state.memory.load(values[0] + 0x10, 4, endness="Iend_LE"))
            if op == "prefix":
                left, right, length = values
                a, b = self.read_cstring(left), self.read_cstring(right)
                for index in range(min(length, len(a) + 1)):
                    x = ord(a[index]) if index < len(a) else 0
                    y = ord(b[index]) if index < len(b) else 0
                    if x != y:
                        return -1 if y > x else 1
                return 0
            if op == "create":
                path, _unused, payload, size, kind, metadata = values
                fields = [size, kind,
                          self.state.solver.eval(self.state.memory.load(metadata, 4, endness="Iend_LE")),
                          self.state.solver.eval(self.state.memory.load(metadata + 4, 4, endness="Iend_LE"))]
                self.state.memory.store(node + 0x24, kind.to_bytes(4, "little"))
                self.state.memory.store(node + 0x28, size.to_bytes(4, "little"))
                self.state.memory.store(node + 0x2C, (0).to_bytes(4, "little"))
                self.state.memory.store(node + 0x30, (1).to_bytes(4, "little"))
                self.state.memory.store(node + 0x34, payload.to_bytes(4, "little"))
                self.state.memory.store(node + 0x238, fields[2].to_bytes(4, "little"))
                self.state.memory.store(node + 0x23C, fields[3].to_bytes(4, "little"))
                self.state.globals["create_args"] = [self.read_cstring(path), payload == buffer,
                                                       size - 0x100000000 if size & 0x80000000 else size,
                                                       kind, fields[2], fields[3]]
                return node
            raise RuntimeError(f"unmodeled boundary {op}")

    def retail_observation(fixture: dict) -> dict:
        state = retail.factory.call_state(
            int(ADDRESS, 16), name, buffer, fixture["length"], ret_addr=stop,
            prototype="void* f(const char*,void*,int)",
            cc=angr.calling_conventions.SimCCCdecl(retail.arch),
            add_options={angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                         angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS})
        state.memory.store(name, b"LOCAL:runtime-probe.qvm\0")
        state.memory.store(buffer, fixture["data"].encode("ascii") + b"\0")
        state.memory.store(buffer + 0x10, (0).to_bytes(4, "little"))
        hooks = [(0x004B1820, "allocate", "void f(int*,char*)"),
                 (0x004B7580, "tag", "int f(void*)"),
                 (0x004B7570, "form", "int f(void*)"),
                 (0x004AAD10, "prefix", "int f(void*,void*,int)"),
                 (0x004B59F0, "create", "void* f(char*,void*,void*,int,int,int*)")]
        for address, operation, prototype in hooks:
            retail.hook(address, Boundary(operation, fixture, prototype=prototype,
                        cc=angr.calling_conventions.SimCCCdecl(retail.arch)), replace=True)
        manager = retail.factory.simgr(state)
        final = None
        for _ in range(2000):
            if len(manager.active) != 1 or manager.errored:
                raise RuntimeError(f"retail {fixture['id']}: {[str(e.error) for e in manager.errored]}")
            current = manager.active[0]
            if current.addr == stop:
                final = current
                break
            manager.step()
        if final is None:
            raise RuntimeError(f"retail {fixture['id']}: execution bound exceeded")
        args_seen = final.globals.get("create_args")
        alloc_args = final.globals.get("allocate_args")
        if args_seen is None or alloc_args is None:
            raise RuntimeError(f"retail {fixture['id']}: no node registration observed")
        return {
            "return_value": final.solver.eval(final.regs.eax) == buffer,
            "state_delta": {"active_count": 1, "kind": final.solver.eval(final.memory.load(node + 0x24, 4, endness="Iend_LE")),
                            "size": signed32(final.solver.eval(final.memory.load(node + 0x28, 4, endness="Iend_LE"))),
                            "loaded": final.solver.eval(final.memory.load(node + 0x2C, 4, endness="Iend_LE")),
                            "metadata_first": final.solver.eval(final.memory.load(node + 0x238, 4, endness="Iend_LE"))},
            "side_effects": [["allocate", *alloc_args], ["create", *args_seen]],
        }

    wrapper_path = None
    with tempfile.TemporaryDirectory(prefix="igi-register-buffer-") as temp:
        tmp = pathlib.Path(temp)
        wrapper_path = tmp / "candidate.cpp"
        candidate = tmp / "candidate"
        wrapper_path.write_text(wrapper, encoding="utf-8")
        gc = "-Wl,-dead_strip" if sys.platform == "darwin" else "-Wl,--gc-sections"
        command = [compiler, "-std=c++20", "-O0", "-ffunction-sections", "-fdata-sections",
                   f"-I{root / 'engine/include'}", str(wrapper_path), str(source),
                   str(root / "engine/src/retail/mem/heap_list.cpp"),
                   str(root / "engine/src/retail/sys/ilff_navigation.cpp"), gc, "-o", str(candidate)]
        built = subprocess.run(command, cwd=root, text=True, capture_output=True, timeout=90)
        if built.returncode:
            raise RuntimeError(f"candidate compile failed: {built.stderr[-4000:]}")
        for index, fixture in enumerate(FIXTURES):
            actual = subprocess.run([str(candidate), str(index)], cwd=root, text=True,
                                    capture_output=True, timeout=10)
            if actual.returncode:
                raise RuntimeError(f"candidate {fixture['id']} failed: {actual.stderr.strip()}")
            fields = [int(x, 0) for x in actual.stdout.split()]
            if len(fields) != 8:
                raise RuntimeError(f"candidate {fixture['id']} malformed: {actual.stdout!r}")
            returned, alloc_called, alloc_path_matches, kind, size, loaded, count, metadata = fields
            reference = retail_observation(fixture)
            candidate_output = {
                "return_value": bool(returned),
                "state_delta": {"active_count": count, "kind": kind, "size": size,
                                "loaded": loaded, "metadata_first": metadata},
                "side_effects": [["allocate", alloc_called, bool(alloc_path_matches)],
                                 ["create", "LOCAL:runtime-probe.qvm", True, size, kind, metadata, 11]],
            }
            # The allocation-name check and second metadata word are fixture invariants too.
            observations.append({"id": fixture["id"],
                "input": {"args": [fixture["length"]], "initial_state": {"buffer_prefix": fixture["data"]}},
                "categories": [fixture["category"]], "reference_output": reference,
                "candidate_output": candidate_output})

    failed = sum(case["reference_output"] != case["candidate_output"] for case in observations)
    report = {"schema_version": 1, "kind": "behavioral-test-report",
        "result": "failed" if failed else "passed", "executed": True,
        "oracle": "retail-binary-execution", "address": ADDRESS,
        "binary_sha256": args.binary_sha256, "implementation_sha256": args.implementation_sha256,
        "method": "Retail Resource_RegisterBuffer at 0x004B5F80 was executed in angr/VEX for short and non-RESOURCE raw buffers. The candidate was compiled from current C++ and executed natively. Hooks model only allocator metadata, ILFF field reads, prefix comparison, and Resource_CreateNode; compared return-to-input-buffer, kind/size/loaded state, active count, metadata, path, and registration arguments.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(observations), "tests_passed": len(observations)-failed,
        "tests_failed": failed, "cases": observations,
        "limitations": ["Only raw-buffer paths are differentially executed; ILFF/IRES and RESOURCE archive parsing remain outside this runner.",
            "Retail and candidate use different execution engines (angr/VEX versus host-native), with caller-observable fields normalized.",
            "Allocator and Resource_CreateNode are modeled on the retail side; their own implementations are not compared here."]}
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return int(bool(failed))


def main() -> int:
    parser = argparse.ArgumentParser()
    for name in ("address", "binary", "binary-sha256", "implementation", "implementation-sha256", "root"):
        parser.add_argument("--" + name, required=True)
    args = parser.parse_args()
    if args.address != ADDRESS:
        raise ValueError(f"unsupported native: {args.address}")
    try:
        return run(args)
    except ImportError:
        root = pathlib.Path(args.root).resolve()
        interpreter = os.environ.get("IGI_ANGR_PYTHON", str(root / ".venv-angr/bin/python"))
        if os.path.abspath(interpreter) == os.path.abspath(sys.executable):
            raise RuntimeError("angr unavailable")
        return subprocess.call([interpreter, __file__, *sys.argv[1:]])


if __name__ == "__main__":
    raise SystemExit(main())
