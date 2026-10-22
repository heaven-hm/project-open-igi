#!/usr/bin/env python3
"""Live retail-vs-candidate differential runner for simple leaf natives.

The retail bytes for the requested address are executed under Radare2 ESIL with
the same argument values and seeded globals used for the candidate. The
candidate is compiled from the current C++ implementation; its argument count
is deduced from the declared signature. Return values and observable state are
compared for each input case. Supported global-state fixtures currently include
QHash_ValueSet, Mem_InitHeapState, and Mem_InvokeNewHandler.

Unsupported shapes fail loudly instead of being silently skipped. Complex
context, device, variadic, and allocator natives need dedicated fixtures.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import re
import subprocess
import sys
import tempfile

R2 = "/opt/homebrew/bin/r2"
CXX = "c++"

GLOBAL_STATE_ADDRESSES = {"0x004B0D40", "0x004B0BA0", "0x004B0F80", "0x004B26A0"}

HARNESS = r'''
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <utility>
#include "@HEADER@"
@EXTRA_HEADER@

namespace {

alignas(16) std::uint8_t host_arg_buffer[4096];

template <class T>
T make_arg(long long value) {
    if constexpr (std::is_pointer_v<T>) {
        return reinterpret_cast<T>(&host_arg_buffer[0]);
    } else if constexpr (std::is_enum_v<T>) {
        return static_cast<T>(value);
    } else {
        return static_cast<T>(value);
    }
}

template <class R, class... A, std::size_t... I>
long long call_with(R (*fn)(A...), long long value, std::index_sequence<I...>) {
    if constexpr (std::is_void_v<R>) {
        fn(make_arg<A>(value + static_cast<long long>(I))...);
        return 0;
    } else if constexpr (std::is_pointer_v<R>) {
        return static_cast<long long>(
            reinterpret_cast<std::uintptr_t>(fn(make_arg<A>(value + static_cast<long long>(I))...)));
    } else {
        return static_cast<long long>(fn(make_arg<A>(value + static_cast<long long>(I))...));
    }
}

template <class R, class... A>
long long run(R (*fn)(A...), long long value) {
    return call_with(fn, value, std::make_index_sequence<sizeof...(A)>{});
}

template <class R, class... A>
std::size_t arity(R (*)(A...)) { return sizeof...(A); }

template <class R, class... A>
int is_void_fn(R (*)(A...)) { return std::is_void_v<R> ? 1 : 0; }

template <class R, class... A>
void print_pointer_mask(R (*)(A...)) {
    const int mask[] = { (std::is_pointer_v<A> ? 1 : 0)... };
    std::printf("MASK=");
    for (std::size_t index = 0; index < sizeof...(A); ++index) {
        std::printf("%s%d", index ? "," : "", mask[index]);
    }
    std::printf("\n");
}

void print_buffer() {
    const auto* words = reinterpret_cast<const std::uint32_t*>(host_arg_buffer);
    std::printf("BUF=");
    for (std::size_t index = 0; index < 16; ++index) {
        std::printf("%s%u", index ? "," : "", words[index]);
    }
    std::printf("\n");
}

} // namespace

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    const long long value = std::strtoll(argv[1], nullptr, 0);
@PRE_CALL@
    std::printf("ARGS=%zu\n", arity(&@QUALIFIED@));
    print_pointer_mask(&@QUALIFIED@);
    const long long result = run(&@QUALIFIED@, value);
    std::printf("VOID=%d\n", is_void_fn(&@QUALIFIED@));
    std::printf("RET=%lld\n", result);
    print_buffer();
@POST_CALL@
    return 0;
}
'''


def digest(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(1 << 20), b""):
            hasher.update(block)
    return hasher.hexdigest()


def signed32(value: int) -> int:
    value &= 0xFFFFFFFF
    return value - 0x100000000 if value & 0x80000000 else value


def candidate_globals_from_match(address: str, match: "re.Match[str] | None") -> dict:
    """Decode the harness GLOBAL= emission for the global-state leaf fixtures."""
    if match is None:
        return {}
    raw = match.group(1)
    if address == "0x004B0D40":
        return {"qhash_flag_0x009422fd": int(raw) & 0xff}
    if address == "0x004B0BA0":
        parts = [int(part) for part in raw.split(",")]
        return {
            "heap_init_flag_0x009422fc": parts[0] & 0xff,
            "qhash_flag_0x009422fd": parts[1] & 0xff,
            "heap_state_0x00a960e0": int(parts[2]),
            "head_is_sentinel_address": bool(parts[3]),
            "sentinel_is_null": bool(parts[4]),
            "tail_is_head_address": bool(parts[5]),
        }
    if address == "0x004B0F80":
        return {"backend_id_0x00a94a64": int(raw)}
    if address == "0x004B26A0":
        return {"upload_flag_0x00943b3c": int(raw) & 0xff}
    return {}


def derive_include(implementation: pathlib.Path, root: pathlib.Path) -> tuple[str, str]:
    relative = implementation.relative_to(root).as_posix()
    stem = relative.split("retail/", 1)[1].rsplit(".", 1)[0]
    header_path = root / "engine/include/igi/retail" / f"{stem}.hpp"
    header_include = f"igi/retail/{stem}.hpp"
    text = header_path.read_text(encoding="utf-8", errors="replace")
    match = re.search(r"namespace\s+igi::retail::([A-Za-z_]\w*)", text)
    namespace = f"igi::retail::{match.group(1)}" if match else "igi::retail"
    return header_include, namespace


def leaf_backend_seed_value(seed: int) -> int:
    """Deterministic per-case value for the 0x00A94A64 texture backend id fixture."""
    return (seed * 3 + 7919) & 0x7FFFFFFF


def retail_return(r2: str, binary: pathlib.Path, address: str, args: list[int], mask: list[int], ret_addr: int, seed: int = 0) -> tuple[int, list[int], dict[str, int]]:
    # ESIL stack and pointer regions live below 0x01000000 to keep every emulated
    # address small and independent of the retail globals region.
    stack = 0x00708000
    pointer_region = 0x00710000
    callback_region = 0x00720000
    # Radare2 6.2 ESIL re-initializes ESP on every aeim, so every globals map is
    # installed before the stack map and the explicit ESP write.
    globals_commands: list[str] = []
    callback = None
    callback_code = None
    globals_commands.append("aeim 0x00900000 0x300000 globals")
    if address == "0x004B0F80":
        globals_commands.append(f"ae {leaf_backend_seed_value(seed)},0x00A94A64,=[4]")
    if address == "0x004B26A0":
        globals_commands.append("ae 0,0x00943b3c,=[1]")
    if address == "0x004AD73D":
        # The port documents and implements the C-locale branch selected by a zero
        # locale id; the runner pins that retail global to keep the comparison on
        # the documented scope (docs/fidelity/scopes/0x004AD73D.md).
        globals_commands.append("ae 0,0x009361e4,=[4]")
    if address == "0x004AD722":
        callback = callback_region
        callback_code = None
        if args and args[0] != 0:
            globals_commands.append(f"ae {callback},0x00936254,=[4]")
            callback_code = ([0xB8, 1, 0, 0, 0, 0xC2, 4, 0]
                             if (args[0] & 1) else [0x31, 0xC0, 0xC2, 4, 0])
        else:
            globals_commands.append("ae 0,0x00936254,=[4]")
    if address == "0x004B0BA0":
        globals_commands.extend([
            "ae 0,0x009422fc,=[1]", "ae 0,0x009422fd,=[1]",
            "ae 0xffffffff,0x00a960e0,=[4]",
            "ae 0x11111111,0x00a960f0,=[4]", "ae 0x22222222,0x00a960f4,=[4]",
            "ae 0x33333333,0x00a960f8,=[4]",
        ])
    if address == "0x004B0D40":
        globals_commands.append("ae 0,0x009422fd,=[1]")
    commands = ["aei", *globals_commands, "aeim 0x00700000 0x200000 stack",
                f"aer esp={stack}", f"ae 0x{ret_addr:08x},{stack},=[4]"]
    if callback_code is not None:
        # Written after the stack map because aeim clears its own region.
        commands.extend(f"ae {byte},{callback + offset:#x},=[1]" for offset, byte in enumerate(callback_code))
    # Deterministically zero the emulated region used for pointer arguments.
    for offset in range(0, 0x200, 4):
        commands.append(f"ae 0,{pointer_region + offset:#x},=[4]")
    for index, value in enumerate(args):
        commands.append(f"ae {value & 0xFFFFFFFF},0x{stack + 4 + index * 4:08x},=[4]")
    commands += [f"s {address}", f"aer eip={address}", f"aesu 0x{ret_addr:08x}", "aer eax"]
    for offset in range(0, 64, 4):
        commands.append(f"ae {pointer_region + offset},[4]")
    if address == "0x004B0D40":
        commands.append("ae 0x009422fd,[1]")
    elif address == "0x004B0BA0":
        commands.extend([
            "ae 0x009422fc,[1]", "ae 0x009422fd,[1]", "ae 0x00a960e0,[4]",
            "ae 0x00a960f0,[4]", "ae 0x00a960f4,[4]", "ae 0x00a960f8,[4]",
        ])
    elif address == "0x004B0F80":
        commands.append("ae 0x00A94A64,[4]")
    elif address == "0x004B26A0":
        commands.append("ae 0x00943b3c,[1]")
    process = subprocess.run(
        [r2, "-q", "-e", "bin.relocs.apply=true", "-c", ";".join(commands), str(binary)],
        text=True, capture_output=True, check=False, timeout=30,
    )
    if process.returncode != 0:
        raise RuntimeError(f"Radare2 ESIL failed ({process.returncode}): {process.stderr.strip()[:400]}")
    values = re.findall(r"(?m)^\s*(0x[0-9a-fA-F]+|\d+)\s*$", process.stdout)
    extra_globals = {"0x004B0D40": 1, "0x004B0BA0": 6, "0x004B0F80": 1,
                     "0x004B26A0": 1}.get(address, 0)
    if len(values) < 17 + extra_globals:
        raise RuntimeError(f"could not read ESIL EAX and buffer: {process.stdout!r} {process.stderr!r}")
    eax = signed32(int(values[-(17 + extra_globals)], 0))
    buffer_end = -extra_globals if extra_globals else None
    buffer_values = values[-(16 + extra_globals):buffer_end]
    buffer_words = [int(value, 0) & 0xFFFFFFFF for value in buffer_values]
    if address == "0x004B0D40":
        globals_after = {"qhash_flag_0x009422fd": int(values[-1], 0) & 0xff}
    elif address == "0x004B0BA0":
        raw = [int(value, 0) & 0xffffffff for value in values[-6:]]
        globals_after = {
            "heap_init_flag_0x009422fc": raw[0] & 0xff,
            "qhash_flag_0x009422fd": raw[1] & 0xff,
            "heap_state_0x00a960e0": signed32(raw[2]),
            "head_is_sentinel_address": raw[3] == 0x00A960F4,
            "sentinel_is_null": raw[4] == 0,
            "tail_is_head_address": raw[5] == 0x00A960F0,
        }
    elif address == "0x004B0F80":
        globals_after = {"backend_id_0x00a94a64": signed32(int(values[-1], 0))}
    elif address == "0x004B26A0":
        globals_after = {"upload_flag_0x00943b3c": int(values[-1], 0) & 0xff}
    else:
        globals_after = {}
    return eax, buffer_words, globals_after


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--address", required=True)
    parser.add_argument("--binary", required=True, type=pathlib.Path)
    parser.add_argument("--binary-sha256", required=True)
    parser.add_argument("--implementation", required=True, type=pathlib.Path)
    parser.add_argument("--implementation-sha256", required=True)
    parser.add_argument("--root", required=True, type=pathlib.Path)
    parser.add_argument("--cases", type=int, default=8)
    args = parser.parse_args()

    address = "0x" + args.address[-8:].upper()
    binary = args.binary.resolve()
    implementation = args.implementation.resolve()
    root = args.root.resolve()
    if digest(binary) != args.binary_sha256:
        raise SystemExit("retail binary SHA-256 changed")
    if digest(implementation) != args.implementation_sha256:
        raise SystemExit("implementation SHA-256 changed")

    manifest = json.loads((root / "docs/fidelity/native-verification.json").read_text(encoding="utf-8"))
    record = next((r for r in manifest["natives"] if r["address"].upper() == address.upper()), None)
    if record is None:
        raise SystemExit(f"no manifest record for {address}")
    name = record["name"]
    header_include, namespace = derive_include(implementation, root)

    specialized_seeds = {
        # Matrix_InvalidateSlot writes matrix + slot*4 + 0x14; keep the slot within
        # the 64-byte observed buffer (slots 1..10).
        "0x004B7450": [0, 1, 2, 3, 4, 5, 6, 9],
        # String_ToUpperChar C-locale conversion boundaries a..z minus neighbours.
        "0x004AD73D": [0, 0x40, 0x41, 0x60, 0x61, 0x7A, 0x7B, 0x80],
    }
    seeds = specialized_seeds.get(
        address, [0, 1, 2, 7, 0x100, 0x7F, 0x80, 0xFF, 0x1234, 0xFFFFFFFF])[: args.cases]
    while len(seeds) < args.cases:
        seeds.append(len(seeds) * 3 + 1)

    cases = []
    with tempfile.TemporaryDirectory(prefix="igi-native-diff-") as temp:
        tempdir = pathlib.Path(temp)
        harness_path = tempdir / "candidate_runner.cpp"
        candidate = tempdir / "candidate"
        engine_lib = root / "build/engine/libigi_retail_engine.a"
        link_tail = ["-Wl,-dead_strip", "-Wl,-undefined,dynamic_lookup"]
        if engine_lib.is_file():
            link_tail.append(str(engine_lib))
        built = None
        for qualified in (f"{namespace}::{name}", name):
            extra_header = ""
            pre_call = ""
            post_call = ""
            if address == "0x004B0D40":
                extra_header = '#include "igi/retail/mem/mem_alloc.hpp"'
                pre_call = "igi::retail::mem::Mem_ResetGlobalsForTest();\n"
                post_call = 'std::printf("GLOBAL=%u\\n", static_cast<unsigned>(igi::retail::mem::Mem_GetGlobals().qhash_flag_9422fd));'
            elif address == "0x004B0BA0":
                extra_header = '#include "igi/retail/mem/mem_alloc.hpp"'
                pre_call = "igi::retail::mem::Mem_ResetGlobalsForTest();\n"
                post_call = r'''const auto& state = igi::retail::mem::Mem_GetGlobals();
    std::printf("GLOBAL=%u,%u,%d,%u,%u,%u\\n",
        static_cast<unsigned>(state.heap_init_9422fc), static_cast<unsigned>(state.qhash_flag_9422fd),
        state.state_a960e0, state.free_list_head_a960f0 == &state.sentinel_a960f4,
        state.sentinel_a960f4 == nullptr, state.free_list_back_a960f8 == &state.free_list_head_a960f0);'''
            elif address == "0x004AD722":
                extra_header = '''#include "igi/retail/mem/mem_alloc.hpp"
static std::int32_t native_leaves_test_handler(std::uint32_t size) { return (size & 1U) ? 1 : 0; }'''
                pre_call = '''igi::retail::mem::Mem_ResetGlobalsForTest();
    if (value != 0) igi::retail::mem::Mem_GetGlobals().new_handler_936254 = &native_leaves_test_handler;
'''
            elif address == "0x004B0F80":
                extra_header = '#include "igi/retail/texture/texture_loaders.hpp"'
                pre_call = ("igi::retail::texture::Texture_SetBackendId("
                            "static_cast<std::int32_t>((value * 3 + 7919) & 0x7FFFFFFF));\n")
                post_call = 'std::printf("GLOBAL=%d\\n", igi::retail::texture::Texture_GetActiveBackendId());'
            elif address == "0x004B26A0":
                extra_header = '#include "igi/retail/texture/texture_loaders.hpp"'
                post_call = 'std::printf("GLOBAL=%d\\n", igi::retail::texture::Texture_GetUploadFlag());'
            harness_path.write_text(
                HARNESS.replace("@HEADER@", header_include).replace("@QUALIFIED@", qualified)
                .replace("@EXTRA_HEADER@", extra_header).replace("@PRE_CALL@", pre_call)
                .replace("@POST_CALL@", post_call),
                encoding="utf-8")
            built = subprocess.run(
                [CXX, "-std=c++20", "-ffunction-sections", "-fdata-sections",
                 f"-I{root / 'engine/include'}", str(implementation), str(harness_path),
                 *link_tail, "-o", str(candidate)],
                cwd=root, text=True, capture_output=True, check=False, timeout=90,
            )
            if built.returncode == 0:
                break
        if built is None or built.returncode != 0:
            raise RuntimeError(f"candidate harness failed to build: {built.stderr[-1500:] if built else 'no build'}")

        arity = None
        for seed in seeds:
            actual = subprocess.run([str(candidate), str(seed)], text=True, capture_output=True,
                                    check=False, timeout=10)
            if actual.returncode != 0:
                raise RuntimeError(f"candidate failed for seed {seed:#x}: {actual.stderr.strip()[:300]}")
            match_arity = re.search(r"ARGS=(\d+)", actual.stdout)
            match_ret = re.search(r"RET=(-?\d+)", actual.stdout)
            match_mask = re.search(r"MASK=([0-9,]*)", actual.stdout)
            match_void = re.search(r"VOID=(\d)", actual.stdout)
            match_buf = re.search(r"BUF=([0-9,]*)", actual.stdout)
            match_globals = re.search(r"GLOBAL=([0-9,]+)", actual.stdout) if address in GLOBAL_STATE_ADDRESSES else None
            if not match_arity or not match_ret or not match_mask or not match_void or not match_buf:
                raise RuntimeError(f"candidate output malformed: {actual.stdout!r}")
            if address in GLOBAL_STATE_ADDRESSES and not match_globals:
                raise RuntimeError(f"candidate global-state output malformed: {actual.stdout!r}")
            arity = int(match_arity.group(1))
            mask = [int(part) for part in match_mask.group(1).split(",") if part] or [0] * arity
            is_void = bool(int(match_void.group(1)))
            candidate_return = int(match_ret.group(1))
            candidate_buffer = [int(part) for part in match_buf.group(1).split(",") if part]
            pointer_region = 0x70010000
            esil_args = [
                (pointer_region if mask[index] else (seed + index)) & 0xFFFFFFFF
                for index in range(arity)
            ]
            reference_return, reference_buffer, reference_globals = retail_return(
                R2, binary, address, esil_args, mask, 0xDEADBEEF, seed)
            if is_void:
                # A void native has no defined return; retail EAX is incidental. Compare its
                # argument-buffer side effects instead of the undefined register value.
                reference_return = 0
                candidate_return = 0
            boundary_seeds = {0, 0xFF, 0xFFFFFFFF}
            if address == "0x004B7450":
                boundary_seeds = {0, 9}
            elif address == "0x004AD73D":
                boundary_seeds = {0, 0x7B}
            elif seeds == specialized_seeds.get(address):
                boundary_seeds = {seeds[0], seeds[-1]}
            candidate_globals = candidate_globals_from_match(address, match_globals)
            cases.append({
                "id": f"seed-{seed:#x}",
                "input": {"args": esil_args, "initial_state": {"seed": seed}},
                "categories": ["boundary" if seed in boundary_seeds else "nominal"],
                "reference_output": {
                    "return_value": reference_return,
                    "state_delta": {"arg_buffer": reference_buffer, **({"globals": reference_globals} if reference_globals else {})},
                    "side_effects": [],
                },
                "candidate_output": {
                    "return_value": candidate_return,
                    "state_delta": {"arg_buffer": candidate_buffer, **({"globals": candidate_globals} if candidate_globals else {})},
                    "side_effects": [],
                },
            })

    failed = sum(case["reference_output"] != case["candidate_output"] for case in cases)
    report = {
        "schema_version": 1,
        "kind": "behavioral-test-report",
        "result": "passed" if failed == 0 else "failed",
        "executed": True,
        "oracle": "retail-binary-execution",
        "address": address,
        "binary_sha256": args.binary_sha256,
        "implementation_sha256": args.implementation_sha256,
        "method": ("Retail instruction bytes for the selected function were executed in Radare2 ESIL "
                   "with cdecl stack arguments and initialized globals; the current C++ implementation "
                   "was compiled and called with the same arguments and equivalent seeded state. Return "
                   "values and instrumented state were compared. Emulation was used because this ARM64 "
                   "macOS host cannot launch the Windows x86 retail process."),
        "observable_fields": ["return_value", "state_delta", "side_effects"],
        "float_tolerance": {"absolute": 0, "relative": 0},
        "tests_total": len(cases),
        "tests_passed": len(cases) - failed,
        "tests_failed": failed,
        "cases": cases,
    }
    print(json.dumps(report, separators=(",", ":"), sort_keys=True))
    return 0 if failed == 0 else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, RuntimeError, subprocess.TimeoutExpired) as error:
        print(str(error), file=sys.stderr)
        raise SystemExit(1)
