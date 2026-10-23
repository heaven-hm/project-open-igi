#!/usr/bin/env python3
"""Differential harness for the resource pool natives 0x004B5C90 (Resource_FreeNode)
and 0x004B5F80 (Resource_RegisterBuffer).

Retail IGI.EXE and a freshly compiled PE32 candidate are executed under angr with
identical modeled boundaries:

- 0x004B5F80 cases hook the native's callees (Sys_AllocateMemory_5 0x004B1820,
  Ilff_GetChunkTag 0x004B7580, Ilff_GetFormType 0x004B7570, Resource_CreateNode
  0x004B59F0, Resource_RegisterIlffContainer 0x004B60F0, Resource_MatchPrefix
  0x004AAD10, HeapList_Insert 0x004AF8F0) on both images, so only the native's own
  dispatch logic, argument construction, name-buffer handling, entry strides and
  loaded-flag writes are compared.
- 0x004B5C90 cases build real nodes through each image's own Resource_CreateNode and
  retail pool init (0x004B5E70) with a deterministic hooked HeapAlloc/malloc, then
  compare the free path: return value (count vs relocated node), the pool count,
  freedata boundary calls (Sys_OpenFileStream_5 0x004B1A30) and surviving node fields.

Node link words (+0x00..+0x07) are excluded from the byte comparison because the port
mirrors the retail list behaviour through its own 64-bit-safe representation; the
anchor region +0x10..+0x1C, all semantic fields, name strings, the freedata order and
the return-value identity are compared exactly.

The private worker imports angr only in the isolated analysis interpreter.
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

SUPPORTED = {"0x004B5C90", "0x004B5F80", "0x004AAD10", "0x004B59F0", "0x004B60F0", "0x004CF330"}

STOP = 0x13370000
NODE_BASE = 0x05B00000
NODE_STRIDE = 0x300
NAME_BASE = 0x05C00000
DATA_BASE = 0x05C00200
META_BASE = 0x05C01000
SPEC_BASE = 0x05C02000
BUF_BASE = 0x05C40000

RETAIL = {
    "init": 0x004B5E70,
    "create_node": 0x004B59F0,
    "free_node": 0x004B5C90,
    "register_buffer": 0x004B5F80,
    "alloc5": 0x004B1820,
    "chunk_tag": 0x004B7580,
    "form_type": 0x004B7570,
    "container": 0x004B60F0,
    "match_prefix": 0x004AAD10,
    "insert": 0x004AF8F0,
    "package_meshes": 0x004CF330,
    "find_form": 0x004CEAA0,
    "load_mesh": 0x004CF390,
    "free_data": 0x004B1A30,
    "error_show": 0x004AF7B0,
}

CANDIDATE_HOOKS = ("Ilff_GetChunkTag", "Ilff_GetFormType", "Resource_CreateNode",
                   "Resource_RegisterIlffContainer", "Resource_MatchPrefix", "HeapList_Insert")

COUNT_GLOBAL = 0x00943E28
PACKAGE_PAYLOAD = 0x05C80000
PACKAGE_CONTEXT = 0x05C81000
PACKAGE_NODE_BASE = 0x05B40000
PACKAGE_NODE_STRIDE = 0x100


def digest(path: pathlib.Path) -> str:
    value = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def name_stride_retail(name_len: int) -> int:
    """Retail `add eax, 0x2B; and al, 0xE0`: low byte masked, upper bits preserved."""
    total = name_len + 0x2B
    return (total & 0xFFFFFF00) | ((total & 0xFF) & 0xE0)


def payload_stride(data_size: int) -> int:
    return (data_size + 0x1F) & 0xFFFFFFE0


def build_archive(entries: list[tuple[str, bytes]]) -> bytes:
    body = bytearray(b"\x00" * 0x20)
    body[0:8] = b"RESOURCE"
    body[8:12] = len(entries).to_bytes(4, "little")
    for name, payload in entries:
        encoded = name.encode("latin1") + b"\x00"
        entry = bytearray(b"\x00" * (name_stride_retail(len(name)) + payload_stride(len(payload))))
        entry[4:8] = len(payload).to_bytes(4, "little")
        entry[8:12] = len(name).to_bytes(4, "little")
        entry[12:12 + len(encoded)] = encoded
        entry[name_stride_retail(len(name)):name_stride_retail(len(name)) + len(payload)] = payload
        body.extend(entry)
    return bytes(body)


WRAPPER = r'''
#include "igi/retail/resource/sys_resource.hpp"
#include "igi/retail/resource/is_resource_loaded.hpp"
#include "igi/retail/mem/heap_list.hpp"
#include <cstdint>

using namespace igi::retail::resource;

namespace igi::retail::resource {
std::int32_t Resource_GetActiveCount() noexcept;
}

// Deterministic bump arena replacing operator new in the candidate image so the
// emulated pool allocation never leaves the DLL for libstdc++/winpthreads machinery.
static std::uint8_t g_harness_arena[0x400000];
static std::size_t g_harness_used = 0;
void* operator new(std::size_t size)
{
    void* pointer = g_harness_arena + g_harness_used;
    g_harness_used += (size + 15u) & ~static_cast<std::size_t>(15u);
    return pointer;
}
void* operator new[](std::size_t size) { return operator new(size); }
void operator delete(void*) noexcept {}
void operator delete[](void*) noexcept {}
void operator delete(void*, std::size_t) noexcept {}
void operator delete[](void*, std::size_t) noexcept {}

extern "C" {
__declspec(dllexport) std::uint32_t NativeEvents[64][4];
__declspec(dllexport) std::uint32_t NativeEventCount = 0;
__declspec(dllexport) std::uint32_t NativeNodes[8];
__declspec(dllexport) std::uint32_t NativeNodeCount = 0;
__declspec(dllexport) std::int32_t NativeKinds[8];
__declspec(dllexport) std::int32_t NativeRet = 0;
__declspec(dllexport) std::int32_t NativeCountAfter = 0;

__declspec(dllexport) std::int32_t NativeAllocate(std::int32_t* meta, const char* path) {
    (void)path;
    meta[0] = 0x77;
    meta[1] = 0x33;
    NativeEvents[NativeEventCount][0] = 1;
    NativeEvents[NativeEventCount][1] = 0x77;
    NativeEvents[NativeEventCount][2] = 0x33;
    NativeEventCount++;
    return 1;
}

__declspec(dllexport) std::int32_t NativeFreeData(void* data) {
    NativeEvents[NativeEventCount][0] = 8;
    NativeEvents[NativeEventCount][1] = reinterpret_cast<std::uint32_t>(data);
    NativeEventCount++;
    return 1;
}

#if IGI_HARNESS_NATIVE == 0x004B5F80
__declspec(dllexport) void* NativeRegisterBuffer(const char* name, void* buffer, std::int32_t size) {
    Sys_SetAllocateMemory5(&NativeAllocate);
    Sys_SetOpenFileStream5(&NativeFreeData);
    return Resource_RegisterBuffer(name, buffer, size);
}

#endif

#if IGI_HARNESS_NATIVE == 0x004AAD10
__declspec(dllexport) std::int32_t NativeMatchCase(std::int32_t str1, std::int32_t str2, std::int32_t max_len) {
    return Resource_MatchPrefix(reinterpret_cast<const void*>(str1), reinterpret_cast<const void*>(str2), max_len);
}

#endif

#if IGI_HARNESS_NATIVE == 0x004B5F80
__declspec(dllexport) std::int32_t NativeListWalk(std::int32_t out_base) {
    // Walks the port's global hash list (Resource_GetHashRoot -> next chain) and stores the
    // visited node pointers; returns the count. The zeroed tail sentinel (next == nullptr)
    // terminates the walk and is not recorded.
    std::int32_t count = 0;
    for (const ResourceHashNode* node = &Resource_GetHashRoot(); node != nullptr; node = node->next) {
        if (node->next == nullptr) {
            break;
        }
        *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(out_base)
            + 4u * static_cast<std::uint32_t>(count)) = reinterpret_cast<std::uint32_t>(node);
        if (++count >= 16) {
            break;
        }
    }
    return count;
}

#endif

#if IGI_HARNESS_NATIVE == 0x004B59F0
__declspec(dllexport) std::int32_t NativeCreateCase(std::int32_t spec_count, std::int32_t specs_base) {
    Resource_InitPoolForTest(64);
    NativeNodeCount = 0;
    for (std::int32_t i = 0; i < spec_count && i < 8; ++i) {
        const std::uint32_t spec = static_cast<std::uint32_t>(specs_base) + 24u * static_cast<std::uint32_t>(i);
        const std::uint32_t name = *reinterpret_cast<const std::uint32_t*>(spec);
        const std::uint32_t arg2 = *reinterpret_cast<const std::uint32_t*>(spec + 4);
        const std::uint32_t data = *reinterpret_cast<const std::uint32_t*>(spec + 8);
        const std::int32_t size = *reinterpret_cast<const std::int32_t*>(spec + 12);
        const std::int32_t kind = *reinterpret_cast<const std::int32_t*>(spec + 16);
        const std::int32_t* meta = reinterpret_cast<const std::int32_t*>(
            *reinterpret_cast<const std::uint32_t*>(spec + 20));
        void* node = Resource_CreateNode(reinterpret_cast<const char*>(name), reinterpret_cast<void*>(arg2),
            reinterpret_cast<void*>(data), size, kind, meta);
        NativeNodes[NativeNodeCount++] = reinterpret_cast<std::uint32_t>(node);
    }
    NativeCountAfter = Resource_GetActiveCount();
    return NativeNodeCount;
}

#endif

#if IGI_HARNESS_NATIVE == 0x004B60F0
__declspec(dllexport) std::int32_t NativeContainerCase(std::int32_t name, std::int32_t buffer, std::int32_t meta) {
    Resource_InitPoolForTest(64);
    Sys_SetAllocateMemory5(&NativeAllocate);
    Sys_SetOpenFileStream5(&NativeFreeData);
    void* parent = Resource_CreateNode(reinterpret_cast<const char*>(name), nullptr,
        reinterpret_cast<void*>(buffer), 0, 0, reinterpret_cast<const std::int32_t*>(meta));
    NativeNodes[0] = reinterpret_cast<std::uint32_t>(parent);
    NativeNodeCount = 1;
    return Resource_RegisterIlffContainer(reinterpret_cast<const char*>(name), parent,
        reinterpret_cast<void*>(buffer), reinterpret_cast<const std::int32_t*>(meta));
}

#endif

#if IGI_HARNESS_NATIVE == 0x004B5C90
__declspec(dllexport) std::int32_t NativeFreeCase(std::int32_t node_count, std::int32_t free_index,
                                                  std::int32_t link_children) {
    Resource_InitPoolForTest(64);
    Sys_SetOpenFileStream5(&NativeFreeData);
    static const char* const kNames[8] = {
        "LOCAL:HARNESS/FREENODE.0", "LOCAL:HARNESS/FREENODE.1", "LOCAL:HARNESS/FREENODE.2",
        "LOCAL:HARNESS/FREENODE.3", "LOCAL:HARNESS/FREENODE.4", "LOCAL:HARNESS/FREENODE.5",
        "LOCAL:HARNESS/FREENODE.6", "LOCAL:HARNESS/FREENODE.7"};
    NativeNodeCount = 0;
    for (std::int32_t i = 0; i < node_count && i < 8; ++i) {
        void* node = Resource_CreateNode(kNames[i], nullptr,
            reinterpret_cast<void*>(0x05C00200u + 0x10u * static_cast<std::uint32_t>(i)),
            8 + i, NativeKinds[i], nullptr);
        NativeNodes[NativeNodeCount++] = reinterpret_cast<std::uint32_t>(node);
    }
    if (link_children && node_count >= 2) {
        std::uint32_t parent = NativeNodes[0];
        std::uint32_t prev_item = parent + 0x14;
        for (std::int32_t i = 1; i < node_count; ++i) {
            std::uint32_t item = NativeNodes[i] + 8;
            *reinterpret_cast<std::uint32_t*>(prev_item) = item;
            *reinterpret_cast<std::uint32_t*>(item + 4) = prev_item;
            prev_item = item;
        }
        *reinterpret_cast<std::uint32_t*>(prev_item) = parent + 0x18;
        *reinterpret_cast<std::uint32_t*>(parent + 0x1C) = prev_item;
    }
    NativeRet = Resource_FreeNode(reinterpret_cast<void*>(NativeNodes[free_index]));
    NativeCountAfter = Resource_GetActiveCount();
    return NativeRet;
}

#endif

#if IGI_HARNESS_NATIVE == 0x004CF330
} // extern "C"
#endif

#if IGI_HARNESS_NATIVE == 0x004CF330
// Package-mesh loop callees. The loop calls the mesh loader and the form walker; both are
// modeled here with the same answers the retail-side hooks give, so both sides execute the
// same loop against identical boundaries.
static std::uint32_t g_package_mesh_pool[8][0x40];
static std::uint32_t g_package_payload = 0;
static std::uint32_t g_package_mesh_index = 0;

__declspec(dllexport) std::uint32_t NativePackageEvents[64][4];
__declspec(dllexport) std::uint32_t NativePackageEventCount = 0;
__declspec(dllexport) std::int32_t NativePackageRet = 0;
__declspec(dllexport) std::uint32_t NativePackageInsertCount = 0;
__declspec(dllexport) std::uint32_t NativePackageChain[8];

namespace igi::retail::dynobj {
void* Ilff_FindFormChunk(void* context, std::int32_t arg2, std::int32_t* arg3);
void* DynObj_LoadModelMesh(const char* model_name);
void* DynObj_LoadPackageMeshes(void* context);

void* Ilff_FindFormChunk(void* context, std::int32_t arg2, std::int32_t* arg3) {
    (void)context; (void)arg3;
    NativePackageEvents[NativePackageEventCount][0] = 2;  // form lookup
    NativePackageEvents[NativePackageEventCount][1] = static_cast<std::uint32_t>(arg2);
    NativePackageEventCount++;
    return reinterpret_cast<void*>(g_package_payload);
}

void* DynObj_LoadModelMesh(const char* model_name) {
    std::uint32_t hash = 2166136261u;
    for (const char* cursor = model_name; *cursor != '\0'; ++cursor) {
        hash ^= static_cast<std::uint8_t>(*cursor);
        hash *= 16777619u;
    }
    NativePackageEvents[NativePackageEventCount][0] = 3;  // mesh load
    NativePackageEvents[NativePackageEventCount][1] = hash;
    NativePackageEventCount++;
    return g_package_mesh_pool[g_package_mesh_index++ & 7u];
}
} // namespace igi::retail::dynobj

extern "C" {
__declspec(dllexport) std::int32_t NativePackageCase(std::int32_t payload) {
    g_package_payload = static_cast<std::uint32_t>(payload);
    g_package_mesh_index = 0;
    NativePackageEventCount = 0;
    NativePackageInsertCount = 0;
    static std::uint8_t context[0x40];
    for (std::uint32_t index = 0; index < sizeof(context); ++index) {
        context[index] = 0;
    }
    *reinterpret_cast<std::uint32_t*>(context + 0x08) = 0x12345678u;
    const std::uint32_t base = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(context));
    *reinterpret_cast<std::uint32_t*>(context + 0x14) = base + 0x18u;
    *reinterpret_cast<std::uint32_t*>(context + 0x18) = base + 0x14u;
    NativePackageRet = static_cast<std::int32_t>(reinterpret_cast<std::intptr_t>(
        igi::retail::dynobj::DynObj_LoadPackageMeshes(context)));
    std::uint32_t item = *reinterpret_cast<std::uint32_t*>(base + 0x14u);
    std::uint32_t guard = 0;
    while (item != base + 0x14u && item != 0 && guard < 8) {
        NativePackageChain[NativePackageInsertCount++] = item;
        item = *reinterpret_cast<std::uint32_t*>(item);
        guard++;
    }
    return 0;
}
#endif
}
'''


def read_cstring(state, pointer: int, cap: int = 0x120) -> str:
    raw = bytearray()
    for offset in range(cap):
        value = state.solver.eval(state.memory.load(pointer + offset, 1))
        if value == 0:
            break
        raw.append(value)
    return raw.decode("latin1")


def read_block(state, pointer: int, length: int) -> str:
    return state.solver.eval(state.memory.load(pointer, length), cast_to=bytes).hex()


def fnv1a(text: str) -> int:
    value = 0x811C9DC5
    for byte in text.encode("latin1"):
        value = ((value ^ byte) * 0x01000193) & 0xFFFFFFFF
    return value


def package_class(value: int) -> str:
    value &= 0xFFFFFFFF
    if value == 0:
        return "null"
    if value < 0x10000:
        return f"small:{value}"
    return "ptr"


def package_payload(fixture: dict) -> bytes:
    count = fixture.get("count", len(fixture["names"]))
    blob = bytearray((count & 0xFFFFFFFF).to_bytes(4, "little"))
    for name in fixture["names"]:
        blob += name.encode("latin1") + b"\x00"
    return bytes(blob)


def node_snapshot(state, node: int) -> dict:
    anchor = []
    for offset in (0x10, 0x14, 0x18, 0x1C):
        value = state.solver.eval(state.memory.load(node + offset, 4, endness="Iend_LE"))
        delta = (value - node) & 0xFFFFFFFF
        anchor.append("0" if value == 0 else f"0x{delta:06x}")
    data_value = state.solver.eval(state.memory.load(node + 0x34, 4, endness="Iend_LE"))
    return {
        "anchor_offsets": anchor,
        "hash": state.solver.eval(state.memory.load(node + 0x20, 4, endness="Iend_LE")),
        "kind": state.solver.eval(state.memory.load(node + 0x24, 4, endness="Iend_LE")),
        "size": state.solver.eval(state.memory.load(node + 0x28, 4, endness="Iend_LE")),
        "loaded": state.solver.eval(state.memory.load(node + 0x2C, 4, endness="Iend_LE")),
        "flag_30": state.solver.eval(state.memory.load(node + 0x30, 4, endness="Iend_LE")),
        "data": "self" if data_value == node else f"0x{data_value:08X}",
        "name": read_cstring(state, node + 0x38, 0x40),
        "meta": read_block(state, node + 0x238, 0xC),
        "active_idx": state.solver.eval(state.memory.load(node + 0x244, 4, endness="Iend_LE")),
    }


def normalize_return(ret: int, count_after: int, node_addresses: list[int]) -> str:
    if ret == count_after:
        return f"count:{count_after}"
    for index, address in enumerate(node_addresses):
        if ret == address:
            return f"node:{index}"
    return f"raw:0x{ret & 0xFFFFFFFF:08X}"


def build_candidate(root: pathlib.Path, source: pathlib.Path, temp: pathlib.Path,
                    address: int = 0) -> tuple[pathlib.Path, list[str]]:
    compiler = shutil.which("i686-w64-mingw32-g++")
    if compiler is None:
        raise RuntimeError("i686-w64-mingw32-g++ is required")
    wrapper = temp / "resource_sys_entry.cpp"
    wrapper.write_text(WRAPPER, encoding="utf-8")
    sources = [
        source,
        root / "engine/src/retail/resource/resource_lifecycle.cpp",
        root / "engine/src/retail/resource/is_resource_loaded.cpp",
        root / "engine/src/retail/sys/resource_lookup.cpp",
        root / "engine/src/retail/sys/ilff_navigation.cpp",
        root / "engine/src/retail/qfile/alias_resolve.cpp",
        root / "engine/src/retail/mem/heap_list.cpp",
        root / "engine/src/retail/error/error_show.cpp",
    ]
    candidate = temp / "resource_sys_candidate.dll"
    command = [compiler, "-std=c++20", "-O0", "-shared", "-fno-exceptions", "-fno-rtti",
               "-ffunction-sections", "-fdata-sections", "-Wl,--gc-sections",
               f"-DIGI_HARNESS_NATIVE=0x{address:08X}",
               "-static-libgcc", f"-I{root / 'engine/include'}", str(wrapper),
               *[str(p) for p in sources], "-o", str(candidate)]
    built = subprocess.run(command, cwd=root, text=True, capture_output=True, timeout=180)
    if built.returncode:
        raise RuntimeError(f"candidate x86 build failed: {built.stderr[-4000:]}")
    return candidate, command


def worker(binary: pathlib.Path, candidate: pathlib.Path, address: str) -> dict:
    import angr

    sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
    from angr_abi_execute import _hook_allocator

    retail = angr.Project(str(binary), auto_load_libs=False)
    rebuilt = angr.Project(str(candidate), auto_load_libs=False)
    for project in (retail, rebuilt):
        if project.arch.name != "X86" or project.arch.bits != 32:
            raise RuntimeError("both inputs must be x86 PE32")

    def symbol(name: str) -> int:
        found = rebuilt.loader.find_symbol(name)
        if found is not None:
            return found.rebased_addr
        # C++ exports are mangled; fall back to a unique substring match over all symbols
        # (e.g. _Z19Resource_CreateNodePKcS0_PviimPPv contains Resource_CreateNode).
        candidates = [s for s in rebuilt.loader.main_object.symbols if name in s.name]
        if len(candidates) != 1:
            raise RuntimeError(f"candidate symbol missing or ambiguous: {name} ({len(candidates)} matches)")
        return candidates[0].rebased_addr

    for name in ("memcpy", "memmove", "memset", "strlen"):
        target = rebuilt.loader.find_symbol(name)
        if target is not None:
            rebuilt.hook(target.rebased_addr, angr.SIM_PROCEDURES["libc"][name](), replace=True)

    def cc_args(project, count):
        return angr.calling_conventions.SimCCCdecl(project.arch)

    class Alloc5(angr.SimProcedure):
        def run(self, meta, path):
            self.state.memory.store(meta, (0x77).to_bytes(4, "little"))
            self.state.memory.store(meta + 4, (0x33).to_bytes(4, "little"))
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("alloc5", 0x77, 0x33),)
            return 1

    class Tag(angr.SimProcedure):
        def run(self, buffer):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("tag", CASE["tag"]),)
            return CASE["tag"]

    class Form(angr.SimProcedure):
        def run(self, buffer):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("form", CASE["form"]),)
            return CASE["form"]

    class CreateNodeModel(angr.SimProcedure):
        def run(self, name, arg2, data, size, kind, meta):
            state = self.state
            index = state.globals.get("node_i", 0)
            state.globals["node_i"] = index + 1
            node = NODE_BASE + index * NODE_STRIDE
            state.memory.store(node, b"\x00" * NODE_STRIDE)
            # Retail CreateNode leaves a freshly allocated node with the self-anchored empty
            # child list (verified at runtime: +0x10 = node, +0x14 = node+0x18, +0x18 = 0,
            # +0x1C = node+0x14). The container splice and the modeled boundaries below rely
            # on that shape, so the model reproduces it exactly.
            state.memory.store(node + 0x10, node.to_bytes(4, "little"))
            state.memory.store(node + 0x14, (node + 0x18).to_bytes(4, "little"))
            state.memory.store(node + 0x1C, (node + 0x14).to_bytes(4, "little"))
            if meta != 0:
                m0 = state.solver.eval(state.memory.load(meta, 4, endness="Iend_LE"))
                m1 = state.solver.eval(state.memory.load(meta + 4, 4, endness="Iend_LE"))
                state.memory.store(node + 0x23C, m0.to_bytes(4, "little"))
                state.memory.store(node + 0x240, m1.to_bytes(4, "little"))
            events = tuple(state.globals.get("events", ()))
            state.globals["events"] = events + (
                ("create", read_cstring(state, name), state.solver.eval(arg2),
                 state.solver.eval(data), state.solver.eval(size), state.solver.eval(kind), node),)
            return node

    class Container(angr.SimProcedure):
        def run(self, name, node, buffer):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (
                ("container", read_cstring(self.state, name),
                 self.state.solver.eval(node), self.state.solver.eval(buffer)),)
            return 1

    class MatchPrefix(angr.SimProcedure):
        def run(self, buffer, prefix, max_len):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("matchprefix", CASE["prefix"]),)
            return CASE["prefix"]

    class Insert(angr.SimProcedure):
        def run(self, anchor, item):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (
                ("insert", self.state.solver.eval(anchor), self.state.solver.eval(item)),)
            return anchor

    class FindForm(angr.SimProcedure):
        def run(self, context, tag, out):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("findform", self.state.solver.eval(tag)),)
            return PACKAGE_PAYLOAD

    class LoadMesh(angr.SimProcedure):
        def run(self, name):
            text = read_cstring(self.state, self.state.solver.eval(name), 0x40)
            index = self.state.globals.get("mesh_index", 0)
            self.state.globals["mesh_index"] = index + 1
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("loadmesh", fnv1a(text)),)
            return PACKAGE_NODE_BASE + index * PACKAGE_NODE_STRIDE

    class FreeData(angr.SimProcedure):
        def run(self, data):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (("freedata", self.state.solver.eval(data)),)
            return 1

    class ErrorModel(angr.SimProcedure):
        def run(self, fmt, arg):
            events = tuple(self.state.globals.get("events", ()))
            self.state.globals["events"] = events + (
                ("error", read_cstring(self.state, fmt, 0x40),
                 read_cstring(self.state, self.state.solver.eval(arg), 0x40)),)
            return 0

    prototypes = {
        "alloc5": "int f(int*, char*)", "tag": "int f(void*)", "form": "int f(void*)",
        "create": "void* f(char*, void*, void*, int, int, int*)",
        "container": "int f(char*, void*, void*, int*)",
        "matchprefix": "int f(void*, void*, int)", "insert": "void* f(void*, void**)",
        "freedata": "int f(void*)", "error": "int f(char*, char*)",
        "findform": "void* f(void*, int, int*)", "loadmesh": "void* f(char*)",
    }

    def install_hooks(project, reference: bool) -> None:
        mapping = {
            "alloc5": RETAIL["alloc5"] if reference else None,
            "tag": RETAIL["chunk_tag"] if reference else None,
            "form": RETAIL["form_type"] if reference else None,
            "create": RETAIL["create_node"] if reference else None,
            "container": RETAIL["container"] if reference else None,
            "matchprefix": RETAIL["match_prefix"] if reference else None,
            "insert": RETAIL["insert"] if reference else None,
            "freedata": RETAIL["free_data"] if reference else None,
            "error": RETAIL["error_show"] if reference else None,
            "findform": RETAIL["find_form"] if reference else None,
            "loadmesh": RETAIL["load_mesh"] if reference else None,
        }
        procedures = {"alloc5": Alloc5, "tag": Tag, "form": Form, "create": CreateNodeModel,
                      "container": Container, "matchprefix": MatchPrefix, "insert": Insert,
                      "freedata": FreeData, "error": ErrorModel, "findform": FindForm,
                      "loadmesh": LoadMesh}
        active_ops = {
            "0x004B5F80": {"alloc5", "tag", "form", "create", "container", "matchprefix", "insert"},
            "0x004B5C90": {"freedata"},
            # 0x004B60F0: hook only CreateNode (simulated fresh-shaped nodes) and Error_Show.
            # HeapList_Insert must stay NATIVE on both sides: retail calls it while the port
            # splices the same pointer writes inline (LinkChildIntoContainer), and the real
            # writes on identical simulated node addresses are exactly what gets compared.
            "0x004B60F0": {"create", "error"},
            "0x004AAD10": set(),
            "0x004B59F0": set(),
            # 0x004CF330: the loop's two callees are modeled on both sides; HeapList_Insert stays
            # native so retail bytes and the port's own splice both run for real.
            "0x004CF330": {"findform", "loadmesh"},
        }[address]
        for op, proc in procedures.items():
            if op not in active_ops:
                continue
            target = mapping[op]
            if target is None:
                if op in ("freedata", "findform", "loadmesh"):
                    # The candidate reaches the free-data boundary through the port's own
                    # Sys_SetOpenFileStream5 hook; no image symbol hook is needed.
                    continue
                target = symbol({
                    "tag": "Ilff_GetChunkTag", "form": "Ilff_GetFormType",
                    "create": "Resource_CreateNode", "container": "Resource_RegisterIlffContainer",
                    "matchprefix": "Resource_MatchPrefix", "insert": "HeapList_Insert",
                    "error": "Error_Show"}[op])
            project.hook(target, proc(prototype=prototypes[op], cc=cc_args(project, 0)), replace=True)

    def terminal_kind(project, state):
        ip = state.solver.eval(state.regs.ip)
        if ip == STOP:
            return "returned"
        # Retail 0x004B637D is exactly EB FE after Error_Show. Require both the
        # observed error boundary and the actual unconditional self-jump bytes.
        if any(event[0] == "error" for event in state.globals.get("events", ())):
            if project.factory.block(ip, size=2).bytes == b"\xeb\xfe":
                return "fatal_halt"
        return None

    def call_chain(project, state, entry: int, args: list[int], allow_fatal=False):
        work = state.copy()
        stack = work.solver.eval(work.regs.esp) - 0x400
        work.memory.store(stack, STOP.to_bytes(4, "little"))
        for index, value in enumerate(args):
            work.memory.store(stack + 4 + index * 4, value.to_bytes(4, "little"))
        work.regs.esp = stack
        work.regs.ip = entry
        manager = project.factory.simgr(work)
        manager.explore(find=(lambda s: terminal_kind(project, s) is not None) if allow_fatal else STOP,
                        num_find=1, n=200000)
        if not manager.found:
            raise RuntimeError(
                f"call to 0x{entry:08X} did not return: {[str(x.error) for x in manager.errored][:2]}")
        return manager.found[0]

    def run_register(project, fixture, reference: bool, base_state=None) -> dict:
        global CASE
        CASE = fixture
        install_hooks(project, reference)
        entry = RETAIL["register_buffer"] if reference else symbol("NativeRegisterBuffer")
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        state = project.factory.call_state(entry, NAME_BASE, BUF_BASE, fixture["size"],
                                           ret_addr=STOP, prototype="void* f(char*, void*, int)",
                                           cc=angr.calling_conventions.SimCCCdecl(project.arch),
                                           add_options=options)
        encoded = fixture["name"].encode("latin1") + b"\x00"
        state.memory.store(NAME_BASE, encoded)
        state.memory.store(BUF_BASE, fixture["buffer_bytes"])
        manager = project.factory.simgr(state)
        manager.explore(find=STOP, num_find=1, n=200000)
        if not manager.found:
            raise RuntimeError(f"register case did not return: {[str(x.error) for x in manager.errored][:2]}")
        final = manager.found[0]
        events = []
        for event in final.globals.get("events", ()):
            events.append([event[0], *[list(item) if isinstance(item, tuple) else item for item in event[1:]]])
        nodes = [NODE_BASE + i * NODE_STRIDE for i in range(final.globals.get("node_i", 0))]
        return {
            "return_value": final.solver.eval(final.regs.eax),
            "state_delta": {
                "nodes": [node_snapshot(final, node) for node in nodes],
                "count_global": final.solver.eval(final.memory.load(COUNT_GLOBAL, 4, endness="Iend_LE")),
            },
            "side_effects": events,
        }

    def run_free(project, fixture, reference: bool, base_state=None) -> dict:
        install_hooks(project, reference)
        # Retail pool init allocates through HeapAlloc; the candidate's Resource_InitPoolForTest
        # vector goes through malloc. Both boundaries resolve to one deterministic address.
        _hook_allocator(project, "HeapAlloc" if reference else "malloc", True)
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        if reference:
            state = base_state.copy() if base_state is not None else call_chain(
                project, project.factory.call_state(RETAIL["init"], ret_addr=STOP, add_options=options),
                RETAIL["init"], [])
            for index in range(fixture["node_count"]):
                encoded = f"LOCAL:HARNESS/FREENODE.{index}\x00".encode()
                state.memory.store(NAME_BASE + index * 0x40, encoded)
            node_addresses = []
            for index in range(fixture["node_count"]):
                name = NAME_BASE + index * 0x40
                print(f"[worker]   create[{index}]", file=sys.stderr, flush=True)
                state = call_chain(project, state, RETAIL["create_node"],
                                   [name, 0, DATA_BASE + index * 0x10, 8 + index,
                                    fixture["kinds"][index], 0])
                node = state.solver.eval(state.regs.eax)
                node_addresses.append(node)
            if fixture["link_children"]:
                parent = node_addresses[0]
                prev_item = parent + 0x14
                writes = []
                for index in range(1, len(node_addresses)):
                    item = node_addresses[index] + 8
                    writes.append((prev_item, item))
                    writes.append((item + 4, prev_item))
                    prev_item = item
                writes.append((prev_item, parent + 0x18))
                writes.append((parent + 0x1C, prev_item))
                for target, value in writes:
                    state.memory.store(target, value.to_bytes(4, "little"))
            state = call_chain(project, state, RETAIL["free_node"], [node_addresses[fixture["free_index"]]])
            print("[worker]   free done", file=sys.stderr, flush=True)
            ret = state.solver.eval(state.regs.eax)
            count_after = state.solver.eval(state.memory.load(COUNT_GLOBAL, 4, endness="Iend_LE"))
            events = [list(event) for event in state.globals.get("events", ())]
            snapshots = [node_snapshot(state, node) for node in node_addresses]
        else:
            kinds_symbol = symbol("NativeKinds")
            state = project.factory.call_state(
                symbol("NativeFreeCase"), fixture["node_count"], fixture["free_index"],
                1 if fixture["link_children"] else 0, ret_addr=STOP,
                prototype="int f(int, int, int)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch), add_options=options)
            for index, kind in enumerate(fixture["kinds"]):
                state.memory.store(kinds_symbol + index * 4, kind.to_bytes(4, "little"))
            manager = project.factory.simgr(state)
            manager.explore(find=STOP, num_find=1, n=200000)
            if not manager.found:
                raise RuntimeError(f"free case did not return: {[str(x.error) for x in manager.errored][:2]}")
            final = manager.found[0]
            events_symbol = symbol("NativeEvents")
            count_symbol = symbol("NativeEventCount")
            event_count = final.solver.eval(final.memory.load(count_symbol, 4, endness="Iend_LE"))
            events = []
            for index in range(event_count):
                base = events_symbol + index * 16
                values = [final.solver.eval(final.memory.load(base + off * 4, 4, endness="Iend_LE"))
                          for off in range(4)]
                if values[0] == 8:
                    events.append(["freedata", values[1]])
                elif values[0] == 1:
                    events.append(["alloc5", values[1], values[2]])
            nodes_symbol = symbol("NativeNodes")
            node_count = final.solver.eval(final.memory.load(symbol("NativeNodeCount"), 4, endness="Iend_LE"))
            node_addresses = [final.solver.eval(final.memory.load(nodes_symbol + i * 4, 4, endness="Iend_LE"))
                              for i in range(node_count)]
            ret = final.solver.eval(final.memory.load(symbol("NativeRet"), 4, endness="Iend_LE"))
            count_after = final.solver.eval(final.memory.load(symbol("NativeCountAfter"), 4, endness="Iend_LE"))
            snapshots = [node_snapshot(final, node) for node in node_addresses]
        return {
            "return_value": normalize_return(ret, count_after, node_addresses),
            "state_delta": {"count_after": count_after, "nodes": snapshots},
            "side_effects": events,
        }

    def run_match(project, fixture, reference: bool, base_state=None) -> dict:
        # 0x004AAD10 is a pure bounded prefix compare: execute the retail body and the
        # candidate export directly with identical string fixtures; no hooks, no pool.
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        entry = RETAIL["match_prefix"] if reference else symbol("NativeMatchCase")
        state = project.factory.call_state(entry, fixture["str1_addr"], fixture["str2_addr"],
                                           fixture["max_len"], ret_addr=STOP,
                                           prototype="int f(char*, char*, int)",
                                           cc=angr.calling_conventions.SimCCCdecl(project.arch),
                                           add_options=options)
        state.memory.store(fixture["str1_addr"], fixture["str1_bytes"])
        state.memory.store(fixture["str2_addr"], fixture["str2_bytes"])
        manager = project.factory.simgr(state)
        manager.explore(find=STOP, num_find=1, n=100000)
        if not manager.found:
            raise RuntimeError(f"match case did not return: {[str(x.error) for x in manager.errored][:2]}")
        final = manager.found[0]
        return {
            "return_value": final.solver.eval(final.regs.eax),
            "state_delta": {
                "str1_after": read_block(final, fixture["str1_addr"], 0x40),
                "str2_after": read_block(final, fixture["str2_addr"], 0x40),
            },
            "side_effects": [],
        }

    def run_package(project, fixture, reference: bool, base_state=None) -> dict:
        # 0x004CF330 is a loop over NUL-terminated model names: it reads the MODS payload through
        # Ilff_FindFormChunk, loads each name through the mesh loader and splices the returned
        # record into the intrusive list at context + 0x10. Both callees are modeled with the same
        # answers on each side; HeapList_Insert stays native so retail bytes and the port's own
        # splice both execute for real.
        install_hooks(project, reference)
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        payload = package_payload(fixture)
        anchor = PACKAGE_CONTEXT + 0x14
        if reference:
            state = project.factory.call_state(
                RETAIL["package_meshes"], PACKAGE_CONTEXT, ret_addr=STOP,
                prototype="void* f(void*)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch), add_options=options)
            state.memory.store(PACKAGE_CONTEXT + 0x08, b"\x78\x56\x34\x12")
            state.memory.store(anchor, (PACKAGE_CONTEXT + 0x18).to_bytes(4, "little"))
            state.memory.store(PACKAGE_CONTEXT + 0x18, anchor.to_bytes(4, "little"))
            state.memory.store(PACKAGE_PAYLOAD, payload)
            manager = project.factory.simgr(state)
            manager.explore(find=STOP, num_find=1, n=200000)
            if not manager.found:
                raise RuntimeError(
                    f"package case did not return: {[str(x.error) for x in manager.errored][:2]}")
            final = manager.found[0]
            ret = final.solver.eval(final.regs.eax)
            events = [["findform", f"0x{event[1]:08X}"] if event[0] == "findform"
                      else ["loadmesh", event[1]]
                      for event in final.globals.get("events", ())]
            chain = []
            item = final.solver.eval(final.memory.load(anchor, 4, endness="Iend_LE"))
            guard = 0
            while item != anchor and item != 0 and guard < 8:
                chain.append(package_class(item))
                item = final.solver.eval(final.memory.load(item, 4, endness="Iend_LE"))
                guard += 1
        else:
            state = project.factory.call_state(
                symbol("NativePackageCase"), PACKAGE_PAYLOAD, ret_addr=STOP,
                prototype="int f(int)", cc=angr.calling_conventions.SimCCCdecl(project.arch),
                add_options=options)
            state.memory.store(PACKAGE_PAYLOAD, payload)
            manager = project.factory.simgr(state)
            manager.explore(find=STOP, num_find=1, n=200000)
            if not manager.found:
                raise RuntimeError(
                    f"package candidate case did not return: {[str(x.error) for x in manager.errored][:2]}")
            final = manager.found[0]
            ret = final.solver.eval(final.memory.load(symbol("NativePackageRet"), 4, endness="Iend_LE"))
            event_count = final.solver.eval(
                final.memory.load(symbol("NativePackageEventCount"), 4, endness="Iend_LE"))
            events = []
            for index in range(event_count):
                base = symbol("NativePackageEvents") + index * 16
                kind = final.solver.eval(final.memory.load(base, 4, endness="Iend_LE"))
                value = final.solver.eval(final.memory.load(base + 4, 4, endness="Iend_LE"))
                events.append(["findform", f"0x{value:08X}"] if kind == 2 else ["loadmesh", value])
            chain_count = final.solver.eval(
                final.memory.load(symbol("NativePackageInsertCount"), 4, endness="Iend_LE"))
            chain = []
            for index in range(chain_count):
                value = final.solver.eval(final.memory.load(
                    symbol("NativePackageChain") + index * 4, 4, endness="Iend_LE"))
                chain.append(package_class(value))
        return {
            "return_value": package_class(ret),
            "state_delta": {"inserted": len(chain), "chain": chain},
            "side_effects": events,
        }

    def write_spec_memory(state, fixture) -> None:
        for index, spec in enumerate(fixture["specs"]):
            name_bytes = (spec["name"] + "\x00").encode("latin1")
            state.memory.store(NAME_BASE + index * 0x40, name_bytes)
            meta_ptr = 0
            if spec["meta"]:
                state.memory.store(META_BASE + index * 8, (0x77).to_bytes(4, "little"))
                state.memory.store(META_BASE + index * 8 + 4, (0x33).to_bytes(4, "little"))
                meta_ptr = META_BASE + index * 8
            base = SPEC_BASE + index * 24
            state.memory.store(base, (NAME_BASE + index * 0x40).to_bytes(4, "little"))
            state.memory.store(base + 4, (0).to_bytes(4, "little"))
            state.memory.store(base + 8, (spec["data"]).to_bytes(4, "little"))
            state.memory.store(base + 12, (spec["size"]).to_bytes(4, "little"))
            state.memory.store(base + 16, (spec["kind"]).to_bytes(4, "little"))
            state.memory.store(base + 20, (meta_ptr).to_bytes(4, "little"))

    def run_create(project, fixture, reference: bool, base_state=None) -> dict:
        _hook_allocator(project, "HeapAlloc" if reference else "malloc", True)
        install_hooks(project, reference)  # no hooks for this address; keeps parity explicit
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        node_addresses = []
        if reference:
            state = base_state.copy() if base_state is not None else call_chain(
                project, project.factory.call_state(RETAIL["init"], ret_addr=STOP, add_options=options),
                RETAIL["init"], [])
            write_spec_memory(state, fixture)
            for index in range(len(fixture["specs"])):
                spec = SPEC_BASE + index * 24
                values = [state.solver.eval(state.memory.load(spec + off * 4, 4, endness="Iend_LE"))
                          for off in range(6)]
                state = call_chain(project, state, RETAIL["create_node"], values)
                node_addresses.append(state.solver.eval(state.regs.eax))
            count_after = state.solver.eval(state.memory.load(COUNT_GLOBAL, 4, endness="Iend_LE"))
            snapshots = [node_snapshot(state, node) for node in node_addresses]
            # Retail global-list walk: the created nodes must all be reachable from the head.
            end_marker = 0x00943E3C
            current = state.solver.eval(state.memory.load(0x00943E38, 4, endness="Iend_LE"))
            visited = []
            guard = 0
            while current not in (0, end_marker) and guard < 16:
                visited.append(current)
                current = state.solver.eval(state.memory.load(current, 4, endness="Iend_LE"))
                guard += 1
            index_of = {address: index for index, address in enumerate(node_addresses)}
            list_set = sorted(index_of[address] for address in visited if address in index_of)
        else:
            state = project.factory.call_state(symbol("NativeCreateCase"), len(fixture["specs"]),
                SPEC_BASE, ret_addr=STOP, prototype="int f(int, int)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch), add_options=options)
            write_spec_memory(state, fixture)
            manager = project.factory.simgr(state)
            manager.explore(find=STOP, num_find=1, n=300000)
            if not manager.found:
                raise RuntimeError(f"create case did not return: {[str(x.error) for x in manager.errored][:2]}")
            final = manager.found[0]
            nodes_symbol = symbol("NativeNodes")
            node_count = final.solver.eval(final.memory.load(symbol("NativeNodeCount"), 4, endness="Iend_LE"))
            node_addresses = [final.solver.eval(final.memory.load(nodes_symbol + i * 4, 4, endness="Iend_LE"))
                              for i in range(node_count)]
            count_after = final.solver.eval(final.memory.load(symbol("NativeCountAfter"), 4, endness="Iend_LE"))
            snapshots = [node_snapshot(final, node) for node in node_addresses]
            # Port global-list walk through the same chain the port's Is_ResourceLoaded uses.
            walk_symbol = symbol("NativeListWalk")
            out_base = SPEC_BASE + 0x200
            walked = call_chain(project, final, walk_symbol, [out_base])
            visited = [walked.solver.eval(walked.memory.load(out_base + i * 4, 4, endness="Iend_LE"))
                       for i in range(walked.solver.eval(walked.regs.eax))]
            index_of = {address: index for index, address in enumerate(node_addresses)}
            list_set = sorted(index_of[address] for address in visited if address in index_of)
        return {
            "return_value": count_after,
            "state_delta": {"count_after": count_after, "nodes": snapshots,
                            "list_reachable_sorted": list_set},
            "side_effects": [],
        }

    def write_container_memory(state, fixture) -> None:
        state.memory.store(NAME_BASE, (fixture["name"] + "\x00").encode("latin1"))
        state.memory.store(META_BASE, (0x77).to_bytes(4, "little"))
        state.memory.store(META_BASE + 4, (0x33).to_bytes(4, "little"))
        state.memory.store(BUF_BASE, fixture["buffer_bytes"])

    def run_container(project, fixture, reference: bool, base_state=None) -> dict:
        install_hooks(project, reference)  # create/insert/error boundary models on both images
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        if reference:
            state = project.factory.call_state(RETAIL["create_node"], NAME_BASE, 0, BUF_BASE, 0, 0,
                META_BASE, ret_addr=STOP, prototype="void* f(char*, void*, void*, int, int, int*)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch), add_options=options)
            write_container_memory(state, fixture)
            state = call_chain(project, state, RETAIL["create_node"],
                               [NAME_BASE, 0, BUF_BASE, 0, 0, META_BASE])
            parent = state.solver.eval(state.regs.eax)
            state = call_chain(project, state, RETAIL["container"],
                               [NAME_BASE, parent, BUF_BASE, META_BASE], allow_fatal=True)
            terminal = terminal_kind(project, state)
            if terminal != fixture["expected_terminal"]:
                raise RuntimeError(f"retail fixture {fixture['id']} reached {terminal}, expected {fixture['expected_terminal']}")
            ret = state.solver.eval(state.regs.eax) if terminal == "returned" else None
            snapshots = [node_snapshot(state, parent)]
        else:
            state = project.factory.call_state(symbol("NativeContainerCase"), NAME_BASE, BUF_BASE,
                META_BASE, ret_addr=STOP, prototype="int f(char*, void*, int*)",
                cc=angr.calling_conventions.SimCCCdecl(project.arch), add_options=options)
            write_container_memory(state, fixture)
            manager = project.factory.simgr(state)
            manager.explore(find=lambda s: terminal_kind(project, s) is not None, num_find=1, n=400000)
            if not manager.found:
                raise RuntimeError(f"container case did not return: {[str(x.error) for x in manager.errored][:2]}")
            final = manager.found[0]
            parent = final.solver.eval(final.memory.load(symbol("NativeNodes"), 4, endness="Iend_LE"))
            terminal = terminal_kind(project, final)
            ret = final.solver.eval(final.regs.eax) if terminal == "returned" else None
            snapshots = [node_snapshot(final, parent)]
            state = final  # events recorded by the model hooks live on the final state
        events = []
        for event in state.globals.get("events", ()):
            events.append([event[0], *[item for item in event[1:]]])
        return {
            "return_value": ret,
            "state_delta": {"parent": snapshots, "terminal": terminal},
            "side_effects": events,
        }

    cases = build_cases(address)
    # The retail pool initializer walks a 10000-entry table; compute it once and fork per case.
    retail_base = None
    if address in ("0x004B5C90", "0x004B59F0"):
        print("[worker] retail pool init (once)", file=sys.stderr, flush=True)
        _hook_allocator(retail, "HeapAlloc", True)
        options = {angr.options.ZERO_FILL_UNCONSTRAINED_MEMORY,
                   angr.options.ZERO_FILL_UNCONSTRAINED_REGISTERS}
        state = retail.factory.call_state(RETAIL["init"], ret_addr=STOP, add_options=options)
        retail_base = call_chain(retail, state, RETAIL["init"], [])
    runners = {
        "0x004B5F80": run_register,
        "0x004B5C90": run_free,
        "0x004AAD10": run_match,
        "0x004B59F0": run_create,
        "0x004B60F0": run_container,
        "0x004CF330": run_package,
    }
    run_one = runners[address]
    observations = []
    for fixture in cases:
        print(f"[worker] case {fixture['id']} retail", file=sys.stderr, flush=True)
        reference = run_one(retail, fixture, True, base_state=retail_base)
        print(f"[worker] case {fixture['id']} candidate", file=sys.stderr, flush=True)
        observed = run_one(rebuilt, fixture, False, base_state=None)
        print(f"[worker] case {fixture['id']} done", file=sys.stderr, flush=True)
        observations.append({
            "id": fixture["id"],
            "input": fixture["input"],
            "categories": fixture["categories"],
            "reference_output": reference,
            "candidate_output": observed,
        })
    return {"cases": observations, "angr_version": angr.__version__}


def build_ilff(chunks: list[tuple[str, bytes]], *, form_type: int = 0x53455249,
               alignment: int = 4, total_override=None) -> bytes:
    """Builds an ILFF container from (tag, payload) child specs.

    Layout evidence (engine/tests/resource/sys_resource_test.cpp + the ported chunk walkers):
    container header tag/size/alignment/next at +0x00..+0x0F, form type at +0x10, first child
    at (alignment + 0x13) & ~(alignment - 1); each child carries tag/size/alignment/next with
    its payload at child + 0x10 (aligned) and the RELATIVE next-chunk offset at +0x0C.
    A NAME chunk's payload is the node name string. An 'ILFF'-tagged child whose +0x10 form
    type is 'DBUG' or 'IRES' embeds that form word where its payload would start, exactly as
    the retail walk reads them (GetFormType reads +0x10 of any chunk base).
    """
    first = (alignment + 0x13) & ~(alignment - 1)
    placed = []
    cursor = first
    for tag_text, payload in chunks:
        tag = int.from_bytes(tag_text.encode("latin1"), "little")
        blob = bytearray(b"\x00" * (0x10 + len(payload)))
        blob[0:4] = tag.to_bytes(4, "little")
        blob[4:8] = len(payload).to_bytes(4, "little")
        blob[8:12] = alignment.to_bytes(4, "little")
        blob[0x10:0x10 + len(payload)] = payload
        placed.append((cursor, blob))
        # Chunks sit at alignment-aligned offsets so the walker's payload formula
        # (child_alignment + relative + 0xF) & ~(child_alignment - 1) lands at chunk + 0x10.
        cursor = (cursor + 0x10 + len(payload) + alignment - 1) & ~(alignment - 1)
    total = total_override if total_override is not None else cursor + 0x10
    buffer = bytearray(b"\x00" * total)
    buffer[0:4] = (0x46464C49).to_bytes(4, "little")  # 'ILFF'
    buffer[4:8] = total.to_bytes(4, "little")
    buffer[8:12] = alignment.to_bytes(4, "little")
    buffer[0x10:0x14] = form_type.to_bytes(4, "little")
    for position, (offset, blob) in enumerate(placed):
        buffer[offset:offset + len(blob)] = blob
        next_offset = placed[position + 1][0] - offset if position + 1 < len(placed) else 0
        buffer[offset + 0x0C:offset + 0x10] = next_offset.to_bytes(4, "little")
    return bytes(buffer)


def build_nested_chunk(tag_text: str, form_type: int, inner_chunks: list[tuple[str, bytes]],
                       alignment: int = 4) -> tuple[str, bytes]:
    """Builds an ILFF-tagged sub-container chunk (DBUG sibling or nested IRES resource).

    The returned payload starts at chunk + 0x10: the form word followed by the inner
    children laid out exactly like a container body.
    """
    inner_first = (alignment + 0x13) & ~(alignment - 1)
    placed = []
    cursor = inner_first
    for tag_text_inner, payload in inner_chunks:
        tag = int.from_bytes(tag_text_inner.encode("latin1"), "little")
        blob = bytearray(b"\x00" * (0x10 + len(payload)))
        blob[0:4] = tag.to_bytes(4, "little")
        blob[4:8] = len(payload).to_bytes(4, "little")
        blob[8:12] = alignment.to_bytes(4, "little")
        blob[0x10:0x10 + len(payload)] = payload
        placed.append((cursor, blob))
        cursor = (cursor + 0x10 + len(payload) + alignment - 1) & ~(alignment - 1)
    body = bytearray(b"\x00" * (cursor + 0x10 - 0x10))
    body[0:4] = form_type.to_bytes(4, "little")
    for position, (offset, blob) in enumerate(placed):
        inner_offset = offset - 0x10
        if inner_offset < 0:
            inner_offset = offset
        body[inner_offset:inner_offset + len(blob)] = blob
        next_offset = (placed[position + 1][0] - placed[position][0]) if position + 1 < len(placed) else 0
        body[inner_offset + 0x0C:inner_offset + 0x10] = next_offset.to_bytes(4, "little")
    return tag_text, bytes(body)


def build_package_cases() -> list[dict]:
    def case(case_id: str, names: list[str], categories: tuple, count: int | None = None) -> dict:
        fixture = {"id": case_id, "names": list(names), "categories": list(categories),
                   "input": {"count": len(names) if count is None else count, "names": list(names)}}
        if count is not None:
            fixture["count"] = count
        return fixture

    return [
        case("pkg-one-model", ["guard"], ("nominal",)),
        case("pkg-two-models", ["guard", "soldier"], ("nominal",)),
        case("pkg-three-models", ["guard", "soldier", "crate"], ("nominal",)),
        case("pkg-eight-models", [f"model{i}" for i in range(8)], ("boundary",)),
        case("pkg-no-models", [], ("boundary",)),
        case("pkg-single-char-name", ["x"], ("boundary",)),
        case("pkg-long-name", ["m" * 63], ("boundary",)),
        case("pkg-backslash-name", ["LEVEL5\\GUARD"], ("boundary",)),
        case("pkg-empty-name", [""], ("boundary",)),
        case("pkg-empty-second-name", ["guard", ""], ("boundary",)),
        case("pkg-negative-count", [], ("boundary",), count=-1),
    ]


def build_cases(address: str) -> list[dict]:
    cases: list[dict] = []
    if address == "0x004AAD10":
        return build_match_cases()
    if address == "0x004B59F0":
        return build_create_cases()
    if address == "0x004B60F0":
        return build_container_cases()
    if address == "0x004CF330":
        return build_package_cases()
    if address == "0x004B5F80":
        def add(case_id, *, tag, form, prefix, entries, name="LOCAL:HARNESS/REGISTER.NODE",
                categories=("nominal",), raw_size=None):
            buffer = build_archive(entries)
            size = len(buffer) if raw_size is None else raw_size
            cases.append({
                "id": case_id,
                "tag": tag, "form": form, "prefix": prefix,
                "name": name, "buffer_bytes": buffer, "size": size,
                "categories": list(categories),
                "input": {"tag": f"0x{tag:08X}", "form": f"0x{form:08X}", "prefix": prefix,
                          "size": size, "count": len(entries),
                          "entries": [n for n, _ in entries], "name": name},
            })

        add("reg-ilff-ires", tag=0x46464C49, form=0x53455249, prefix=1, entries=[])
        add("reg-ilff-not-ires", tag=0x46464C49, form=0x524F4E46, prefix=0,
            entries=[("CHILD.A", bytes([1, 2, 3, 4, 5]))])
        add("reg-raw-small", tag=0x41544144, form=0, prefix=1, entries=[], raw_size=7,
            categories=("boundary",))
        add("reg-raw-prefix-hit", tag=0x41544144, form=0, prefix=1, entries=[], raw_size=8,
            categories=("boundary",))
        add("reg-archive-empty", tag=0x41544144, form=0, prefix=0, entries=[],
            categories=("boundary",))
        add("reg-archive-one", tag=0x41544144, form=0, prefix=0,
            entries=[("CHILD.A", bytes([1, 2, 3, 4, 5]))])
        add("reg-archive-two", tag=0x41544144, form=0, prefix=0,
            entries=[("CHILD.A", bytes([1, 2, 3])), ("LONGER.CHILD.NAME.X", bytes(range(64)))])
        add("reg-archive-strides", tag=0x41544144, form=0, prefix=0,
            entries=[("A", b""), ("EIGHTCHR", b"\x01"), ("THIRTYTWO.CHARS.EXACTLY.PAD!!", bytes(range(31)))])
        add("reg-archive-longname", tag=0x41544144, form=0, prefix=0,
            entries=[("L" * 250, bytes(range(8)))], categories=("boundary",))
        add("reg-raw-zero", tag=0x41544144, form=0, prefix=0, entries=[], raw_size=0,
            categories=("boundary",))
        return cases

    def add(case_id, *, kinds, free_index, link_children, categories):
        cases.append({
            "id": case_id, "kinds": list(kinds), "free_index": free_index,
            "link_children": link_children, "categories": list(categories),
            "node_count": len(kinds),
            "input": {"kinds": list(kinds), "free_index": free_index,
                      "link_children": link_children, "node_count": len(kinds)},
        })

    add("free-middle-kind2", kinds=(2, 2, 2), free_index=1, link_children=0, categories=("nominal",))
    add("free-last-kind2", kinds=(2, 2, 2), free_index=2, link_children=0, categories=("nominal",))
    add("free-middle-kind3", kinds=(3, 3, 3), free_index=1, link_children=0, categories=("nominal",))
    add("free-first-kind3", kinds=(3, 3, 3), free_index=0, link_children=0, categories=("nominal",))
    add("free-single-kind2", kinds=(2,), free_index=0, link_children=0, categories=("boundary",))
    add("free-container-children", kinds=(0, 2, 2), free_index=0, link_children=1,
        categories=("nominal", "boundary"))
    add("free-container-empty", kinds=(0,), free_index=0, link_children=0, categories=("boundary",))
    add("free-middle-kind4", kinds=(4, 4, 4), free_index=1, link_children=0, categories=("nominal",))
    add("free-unknown-kind9", kinds=(9, 2, 2), free_index=0, link_children=0, categories=("boundary",))
    add("free-subresource-kind1", kinds=(0, 1), free_index=1, link_children=1, categories=("nominal",))
    return cases


def _match_case(case_id, str1, str2, max_len, categories):
    encoded1 = str1.encode("latin1") + b"\x00"
    encoded2 = str2.encode("latin1") + b"\x00"
    return {
        "id": case_id,
        "str1": str1, "str2": str2, "max_len": max_len,
        "str1_addr": NAME_BASE, "str2_addr": NAME_BASE + 0x40,
        "str1_bytes": encoded1, "str2_bytes": encoded2,
        "categories": list(categories),
        "input": {"str1": str1, "str2": str2, "max_len": max_len},
    }


if True:  # keeps the fixture blocks below at a stable indentation
    pass


def build_match_cases() -> list[dict]:
    return [
        _match_case("match-equal", "RESOURCE", "RESOURCE", 8, ("nominal",)),
        _match_case("match-prefix-equal", "RESOURCE", "RESOURCED", 8, ("nominal",)),
        _match_case("match-str2-smaller", "RESOURCX", "RESOURCE", 8, ("nominal",)),
        _match_case("match-str2-greater", "RESOURCE", "RESOURCY", 8, ("nominal",)),
        _match_case("match-str1-shorter", "RES", "RESOURCE", 8, ("boundary",)),
        _match_case("match-str2-shorter", "RESOURCEDATA", "RES", 8, ("boundary",)),
        _match_case("match-zero-len", "RESOURCE", "RESOURCE", 0, ("boundary",)),
        _match_case("match-one-byte", "A", "B", 1, ("boundary",)),
        _match_case("match-slash-difference", "LOCAL:path\\one", "LOCAL:path/one", 14, ("nominal",)),
        _match_case("match-empty-str1", "", "x", 8, ("boundary",)),
        _match_case("match-empty-str2", "x", "", 8, ("boundary",)),
        _match_case("match-long-bound", "LOCAL:HARNESS/MATCH.VERY.LONG.STRING", "LOCAL:HARNESS/MATCH.VERY.LONG.STRINH", 32, ("nominal",)),
    ]


def build_create_cases() -> list[dict]:
    def case(case_id, specs, categories):
        return {
            "id": case_id, "specs": specs, "categories": list(categories),
            "input": {"names": [s["name"] for s in specs],
                      "kinds": [s["kind"] for s in specs],
                      "sizes": [s["size"] for s in specs]},
        }

    plain = {"name": "LOCAL:HARNESS/NODE.A", "data": DATA_BASE, "size": 8, "kind": 2, "meta": True}
    return [
        case("create-plain", [plain], ("nominal",)),
        case("create-backslashes", [{"name": "LOCAL:HARNESS\\NODE.B", "data": DATA_BASE,
                                     "size": 16, "kind": 2, "meta": True}], ("nominal",)),
        case("create-lowercase", [{"name": "local:harness/node.c", "data": DATA_BASE,
                                   "size": 24, "kind": 2, "meta": True}], ("nominal",)),
        case("create-empty-name", [{"name": "", "data": DATA_BASE, "size": 4, "kind": 2,
                                    "meta": True}], ("boundary",)),
        case("create-null-data", [{"name": "LOCAL:HARNESS/NODE.E", "data": 0, "size": 8,
                                   "kind": 2, "meta": True}], ("boundary",)),
        case("create-kind0", [{"name": "LOCAL:HARNESS/NODE.F", "data": DATA_BASE, "size": 0,
                               "kind": 0, "meta": True}], ("nominal",)),
        case("create-kind3", [{"name": "LOCAL:HARNESS/NODE.G", "data": DATA_BASE, "size": 20604,
                               "kind": 3, "meta": True}], ("nominal",)),
        case("create-null-meta", [{"name": "LOCAL:HARNESS/NODE.H", "data": DATA_BASE, "size": 12,
                                   "kind": 1, "meta": False}], ("boundary",)),
        case("create-duplicates", [plain,
                                   {"name": "LOCAL:HARNESS/NODE.A", "data": DATA_BASE + 0x10,
                                    "size": 9, "kind": 2, "meta": True}], ("nominal",)),
        case("create-sequence", [plain,
                                 {"name": "LOCAL:HARNESS/NODE.B", "data": DATA_BASE + 0x10,
                                  "size": 9, "kind": 1, "meta": True},
                                 {"name": "LOCAL:HARNESS/NODE.C", "data": DATA_BASE + 0x20,
                                  "size": 10, "kind": 0, "meta": False}], ("nominal", "boundary")),
    ]


def build_container_cases() -> list[dict]:
    def case(case_id, name, buffer, categories, expected_terminal="returned"):
        return {
            "id": case_id, "name": name, "buffer_bytes": buffer,
            "categories": list(categories),
            "input": {"name": name, "size": len(buffer)},
            "expected_terminal": expected_terminal,
        }

    name_a = "LOCAL:models/cube.mef"
    name_b = "LOCAL:models/two.mef"
    return [
        case("container-body-one", "LOCAL:models/level1.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         ("BODY", bytes(range(8)))]), ("nominal",)),
        case("container-cstr", "LOCAL:models/level2.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         ("CSTR", b"string payload")]), ("nominal",)),
        case("container-path", "LOCAL:models/level3.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         ("PATH", b"paths/texture.bmp")]), ("nominal",)),
        case("container-nested-ires", "LOCAL:models/level4.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         build_nested_chunk("ILFF", 0x53455249,
                                            [("NAME", ("LOCAL:models/inner.mef\x00").encode("latin1")),
                                             ("BODY", b"1234")])]), ("nominal",)),
        case("container-dbug-sibling", "LOCAL:models/level5.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         ("BODY", bytes(range(4))),
                         build_nested_chunk("ILFF", 0x47554244,
                                            [("CSTR", b"debug-note\x00")])]), ("nominal",)),
        case("container-two-children", "LOCAL:models/level6.res",
             build_ilff([("NAME", ("LOCAL:models/one.mef\x00").encode("latin1")),
                         ("BODY", b"1111"),
                         ("NAME", (name_b + "\x00").encode("latin1")),
                         ("BODY", b"22222222")]), ("nominal",)),
        case("container-no-children", "LOCAL:models/empty.res",
             build_ilff([], total_override=0x14), ("boundary",)),
        case("container-malformed-tag", "LOCAL:models/bad.res",
             build_ilff([("NAME", (name_a + "\x00").encode("latin1")),
                         ("XXXX", b"payload")]), ("boundary",), "fatal_halt"),
        case("container-name-last", "LOCAL:models/trailing.res",
             # NAME chunk with next_offset 0: the walk errors after the NAME.
             build_ilff([("NAME", (name_a + "\x00").encode("latin1"))]), ("boundary",), "fatal_halt"),
        case("container-name-then-path-pair", "LOCAL:models/mixed.res",
             build_ilff([("NAME", ("LOCAL:models/tex.bmp\x00").encode("latin1")),
                         ("PATH", b"t/p"),
                         ("NAME", (name_b + "\x00").encode("latin1")),
                         ("BODY", b"mmmm")]), ("nominal",)),
    ]



def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", type=pathlib.Path, required=True)
    parser.add_argument("--binary-sha256")
    parser.add_argument("--implementation", type=pathlib.Path)
    parser.add_argument("--implementation-sha256")
    parser.add_argument("--root", type=pathlib.Path)
    parser.add_argument("--worker-candidate", type=pathlib.Path, help=argparse.SUPPRESS)
    args = parser.parse_args()
    address = "0x" + args.address[-8:].upper()
    if address not in SUPPORTED:
        raise ValueError(f"unsupported native {address}")
    if args.worker_candidate is None:
        missing = [name for name, value in (
            ("--binary-sha256", args.binary_sha256), ("--implementation", args.implementation),
            ("--implementation-sha256", args.implementation_sha256), ("--root", args.root)) if not value]
        if missing:
            parser.error(f"the following arguments are required: {', '.join(missing)}")
    root = (args.root or pathlib.Path(".")).resolve()
    binary = args.binary.resolve()
    implementation = (args.implementation or pathlib.Path(".")).resolve()
    if args.worker_candidate is None and (
            digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256):
        raise RuntimeError("retail binary or implementation hash changed")

    if args.worker_candidate:
        result = worker(binary, args.worker_candidate, address)
        print(json.dumps(result, separators=(",", ":")))
        return 0

    angr_python = pathlib.Path(os.environ.get("IGI_ANGR_PYTHON", "")).expanduser()
    if not angr_python.is_file():
        angr_python = root / ".venv-angr/bin/python"
    if not angr_python.is_file():
        raise RuntimeError("angr Python is unavailable; configure IGI_ANGR_PYTHON")

    runner_hash = digest(pathlib.Path(__file__).resolve())
    with tempfile.TemporaryDirectory(prefix="igi-resource-sys-") as temp:
        candidate, command = build_candidate(root, implementation, pathlib.Path(temp),
                                            int(address, 16))
        candidate_hash = digest(candidate)
        executed = subprocess.run(
            [str(angr_python), str(pathlib.Path(__file__).resolve()), "--address", address,
             "--binary", str(binary), "--worker-candidate", str(candidate)],
            cwd=root, text=True, capture_output=True, timeout=1500)
        if executed.returncode:
            raise RuntimeError(f"angr comparison failed: {executed.stderr[-4000:]}")
        output = json.loads(executed.stdout)

    if (digest(binary) != args.binary_sha256 or digest(implementation) != args.implementation_sha256
            or digest(pathlib.Path(__file__).resolve()) != runner_hash):
        raise RuntimeError("binary, implementation or runner changed during comparison")

    cases = output["cases"]
    expected_ids = [case["id"] for case in build_cases(address)]
    if [case["id"] for case in cases] != expected_ids:
        raise RuntimeError("worker returned an incomplete or unexpected fixture set")
    failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if not failed else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": "Retail and freshly compiled PE32 x86 candidate executed under angr with identical modeled "
                  "resource boundaries; compared return identity (count vs relocated node), pool count, "
                  "freedata call order, node anchor/semantic fields and modeled callee event traces.",
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": len(cases) - failed,
        "tests_failed": failed,
        "cases": cases,
        "runner": {"path": "scripts/resource_sys_diff_runner.py", "sha256": runner_hash},
        "retail_execution": {"tool": "angr", "version": output["angr_version"], "engine": "VEX",
                             "candidate_sha256": candidate_hash, "candidate_build_command": command},
        "limitations": [
            "Modeled boundaries: Sys_AllocateMemory_5 meta writes, fixture-driven chunk-tag/form-type and "
            "prefix-match returns, simulated CreateNode slots and container/insert call records.",
            "Node link words +0x00..+0x07 are excluded: the port mirrors retail list behaviour through its "
            "own 64-bit-safe representation; anchor +0x10..+0x1C and all semantic fields are compared.",
            "Concrete emulator fixtures, not a running retail process or a proof of all inputs.",
        ],
    }
    print(json.dumps(report, separators=(",", ":")))
    return 1 if failed else 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, ValueError, KeyError, subprocess.SubprocessError) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
