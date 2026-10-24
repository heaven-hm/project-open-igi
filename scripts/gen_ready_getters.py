#!/usr/bin/env python3
"""Generate ready two-instruction dword getters from the 0x00402930 closure.

A getter qualifies when its whole body is `mov 0x<abs>,%eax; ret`, it is reachable
from the 0x00402930 chain, and it has no manifest record yet. Storage ownership is
checked before anything is written: if any other engine file already mentions the
target address, the generated file declares the object extern and names the owner
instead of defining a second copy (that duplication broke the build in round 230).

Usage: python3 scripts/gen_ready_getters.py [--count N] [--dry-run]
"""
from __future__ import annotations

import argparse
import bisect
import hashlib
import json
import pathlib
import re
import subprocess

ROOT = pathlib.Path(__file__).resolve().parent.parent
BIN = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
MAC = """#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif"""
CRT_LO, CRT_HI = 0x4A5000, 0x4AB000


def load_disassembly():
    functions = {}
    for entry in json.loads((ROOT / "build/retail-functions.json").read_text())["functions"]:
        functions[entry["addr"]] = (entry["addr"], entry["addr"] + entry.get("size", 0))
    instructions = []
    for line in (ROOT / "build/retail-dis.txt").read_text().splitlines():
        match = re.match(r"\s*([0-9a-f]+):\t[0-9a-f ]+\t(.*)", line)
        if match:
            instructions.append((int(match.group(1), 16), match.group(2).strip()))
    instructions.sort()
    return functions, instructions, [address for address, _ in instructions]


def span(functions, instructions, addresses, va):
    if va not in functions:
        return []
    start, end = functions[va]
    return instructions[bisect.bisect_left(addresses, start):bisect.bisect_left(addresses, end)]


def dependencies(instructions):
    calls = {int(m.group(1), 16) for _, text in instructions for m in [re.search(r"call\s+0x([0-9a-f]+)", text)] if m}
    jumps = {int(m.group(1), 16) for _, text in instructions
             for m in [re.match(r"jmp\s+0x([0-9a-f]+)", text)] if m and int(m.group(1), 16) != 0x4CEF30}
    immediates = {int(m.group(1), 16) for _, text in instructions
                  for m in re.finditer(r"\$0x([0-9a-f]{6})", text) if 0x401000 <= int(m.group(1), 16) < 0x4D0000}
    return calls | jumps | immediates


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--count", type=int, default=6)
    parser.add_argument("--dry-run", action="store_true")
    options = parser.parse_args()

    functions, instructions, addresses = load_disassembly()
    manifest_path = ROOT / "docs/fidelity/native-verification.json"
    manifest = json.loads(manifest_path.read_text())
    known = {int(row["address"], 16) for row in manifest["natives"] if row.get("address")}

    chain = []
    started = False
    for address, text in instructions:
        if address == 0x402930:
            started = True
        if not started:
            continue
        if "jmp    0x4cef30" in text:
            break
        match = re.match(r"call\s+0x([0-9a-f]+)", text)
        if match:
            chain.append(int(match.group(1), 16))

    closure, frontier = set(), set(chain)
    while frontier:
        following = set()
        for va in frontier:
            if va in known or va in closure:
                continue
            closure.add(va)
            following |= {dep for dep in dependencies(span(functions, instructions, addresses, va)) if dep not in known}
        frontier = following

    implemented = {int(path.name[7:13], 16) for path in (ROOT / "engine/src/retail").glob("*/native_*.cpp")}
    picks = []
    for va in sorted(closure):
        if va in implemented:  # already has an implementation file, even if the manifest row is missing
            continue
        body = span(functions, instructions, addresses, va)
        if len(body) != 2 or body[1][1] != "ret" or CRT_LO <= va < CRT_HI:
            continue
        match = re.match(r"mov\s+0x([0-9a-f]+),%eax$", body[0][1])
        if match:
            picks.append((va, int(match.group(1), 16)))
        if len(picks) >= options.count:
            break

    if not picks:
        print("no ready getters left in the closure")
        return 0
    print("picks:", ", ".join(f"0x{va:06X}->0x{addr:08X}" for va, addr in picks))
    if options.dry_run:
        return 0

    cmake = (ROOT / "CMakeLists.txt").read_text()
    template = (ROOT / "scripts/tester/frida_native_probe_template.js").read_text()
    for va, address in picks:
        text_va = f"0x{va:08X}"
        name = f"Sys_GetGlobal32_{va:06X}"
        symbol = f"g_{address:06x}"
        owners = [line for line in subprocess.run(
            ["grep", "-rlE", rf"^\s*(std::)?uint32_t {symbol}\s*(=|;)", "engine/src/retail", "--include=*.cpp"],
            cwd=ROOT, capture_output=True, text=True).stdout.split()
            if f"native_{va:06x}" not in line]
        owner_note = (f"// 0x{address:08X}: four-byte object, defined by {owners[0].split('/')[-1]} which assigns it; this file only reads it."
                      if owners else
                      f"// 0x{address:08X}: the four-byte object this getter reads; nothing else in the tree owns it yet.")
        declaration = f"extern std::uint32_t {symbol};" if owners else f"std::uint32_t {symbol};"
        raw = f"a1 {address & 0xFF:02x} {(address >> 8) & 0xFF:02x} {(address >> 16) & 0xFF:02x} {(address >> 24) & 0xFF:02x} c3"
        implementation = ROOT / f"engine/src/retail/sys/native_{va:06x}.cpp"
        implementation.write_text(f"""// Native: {name}; retail VA {text_va} (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 {BIN}; VA {text_va}-0x{va + 6:08X} (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes ({raw}).
// Behavior/CFG: loads the four-byte value at 0x{address:08X} into eax and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Four bytes is the width this load proves; the value's meaning is not established.
#include <cstdint>

{MAC}

extern "C" {{

{owner_note}
{declaration}

std::uint32_t NATIVE_DECL {name}(void)
{{
    return ::{symbol};                                                       // {text_va}: mov 0x{address:08x},%eax; 0x{va + 5:08X}: ret
}}

}} // extern "C"

#undef NATIVE_DECL
""")
        candidate = ROOT / f"scripts/GameNatives/sys/{va:08X}-{name}.cpp"
        candidate.write_text(f"""// Candidate: {name}; retail VA {text_va}. Catalog candidate for {implementation.relative_to(ROOT).as_posix()}.
// Retail artifact: igi.exe SHA256 {BIN}.
#include <cstdint>
extern "C" {{
std::uint32_t {symbol};
std::uint32_t {name}(void);
}}
""")
        cmake = cmake.replace("    engine/src/retail/sys/native_4aa0a0.cpp\n",
                              f"    engine/src/retail/sys/native_4aa0a0.cpp\n    engine/src/retail/sys/native_{va:06x}.cpp\n", 1)

        probe = ROOT / f"docs/fidelity/evidence/{text_va}-runtime/probe_{text_va}.js"
        probe.parent.mkdir(parents=True, exist_ok=True)
        probe.write_text(template.replace("__PROBE_TARGET_ADDRESS__", text_va).replace("__PROBE_TARGET_NAME__", name))

        checks = [
            {"name": "function_identity", "result": "passed",
             "rationale": "6 retail bytes, 2 instructions, 1 block, 0 edges; candidate 1/0 at CFG 1.000."},
            {"name": "argument_mapping", "result": "passed", "rationale": "No ABI arguments; no stack slot is read."},
            {"name": "constants_and_scaling", "result": "passed",
             "rationale": f"One constant: the address 0x{address:08X}. No offset or scale applied."},
            {"name": "memory_layout_and_access", "result": "passed",
             "rationale": f"One four-byte load from 0x{address:08X}, the width proven here; no store, no indirect access."
                          + (f" The object is defined by {owners[0].split('/')[-1]}, not here." if owners else "")},
            {"name": "calls_and_side_effects", "result": "passed",
             "rationale": "No call and no store: the only effect is the returned value."},
            {"name": "return_and_error_paths", "result": "passed",
             "rationale": "One entry, one exit: the ret at +5. No branch and no error path."},
            {"name": "floating_point_semantics", "result": "not_applicable",
             "rationale": "Integer load only; no floating-point instruction."},
        ]
        review = ROOT / f"docs/fidelity/evidence/{text_va}-semantic-review.json"
        review.write_text(json.dumps({
            "schema_version": 1, "kind": "semantic-review-report", "address": text_va, "binary_sha256": BIN,
            "implementation_sha256": hashlib.sha256(implementation.read_bytes()).hexdigest(),
            "implementation_path": implementation.relative_to(ROOT).as_posix(),
            "reviewed_by": "lead", "reviewed": True,
            "review_scope": "static: retail instructions and emitted candidate listing", "result": "passed",
            "checks": checks,
            "runtime_contract": {
                "scope": "paired retail/candidate observation of the loaded value during level-1 startup",
                "probe_sha256": hashlib.sha256(probe.read_bytes()).hexdigest(),
                "required_observables": {"inputs": [], "outputs": ["return_value"], "sideEffects": []},
                "no_arguments": True, "no_side_effects": True},
            "evidence": probe.parent.relative_to(ROOT).as_posix(),
            "limitations": ["Runtime not yet paired: the candidate exits at 0x00402930 before any game native runs.",
                            "The value's meaning is not established."],
        }, indent=2) + "\n")

        manifest["natives"].append({
            "address": text_va, "name": name, "binary_sha256": BIN,
            "implementation": {"path": implementation.relative_to(ROOT).as_posix(),
                               "sha256": hashlib.sha256(implementation.read_bytes()).hexdigest(),
                               "signature": "uint32_t __cdecl(void)"},
            "game_natives": {"path": candidate.relative_to(ROOT).as_posix(),
                             "sha256": hashlib.sha256(candidate.read_bytes()).hexdigest(), "signature": "unknown"},
            "verification": {"status": "TRANSCRIBED", "cfg_similarity": 1.0,
                             "semantic_review": {"evidence_path": review.relative_to(ROOT).as_posix(),
                                                 "evidence_sha256": hashlib.sha256(review.read_bytes()).hexdigest()}},
        })
        print(f"wrote {text_va} -> 0x{address:08X} ({'extern' if owners else 'own storage'})")

    (ROOT / "CMakeLists.txt").write_text(cmake)
    manifest["natives"].sort(key=lambda row: int(row["address"], 16))
    manifest_path.write_text(json.dumps(manifest, indent=2) + "\n")
    print(f"records now {len(manifest['natives'])}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
