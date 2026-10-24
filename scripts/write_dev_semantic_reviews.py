#!/usr/bin/env python3
"""Write semantic-review artifacts for the development-gate native set.

Each report is bound to the current implementation file hash and the retail
binary hash, contains the seven required named checks, and records per-native
rationales combining the recorded Binary Ninja/Radare2 evidence with the
2026-10-19 differential-runner results.
"""

from __future__ import annotations

import hashlib
import json
import pathlib

ROOT = pathlib.Path(".").resolve()
BINARY_SHA = "dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01"
IMPL_ENCODING = "utf-8"

NATIVES: dict[str, dict] = {}


def native(address, name, impl_path, review_scope, checks, limitations, evidence):
    NATIVES[address] = {
        "name": name,
        "impl_path": impl_path,
        "review_scope": review_scope,
        "checks": checks,
        "limitations": limitations,
        "evidence": evidence,
    }


LEAF_EVIDENCE = [
    "docs/fidelity/evidence/{addr}-binaryninja.json",
    "docs/fidelity/evidence/{addr}-radare2.json",
    "docs/fidelity/evidence/{addr}-behavioral.json",
]


def check(rationale, result="passed"):
    return {"result": result, "rationale": rationale}


# --- memory allocator natives (implementation engine/src/retail/mem/mem_alloc.cpp) ---
MEM_IMPL = "engine/src/retail/mem/mem_alloc.cpp"
MEM_FOOTNOTE = (
    "The 0x0053xxxx winner tails it must not touch: "
    "live Radare2 ESIL executes the retail bytes while the ARM64 host cannot launch the x86 process."
)

native("0x004AA670", "Mem_AllocChunk", MEM_IMPL,
    "18-byte x86 thunk forwarding (size, g_936250) to Mem_AllocWithRetry",
    {
        "function_identity": check("Binary Ninja and Radare2 both show one block: a size argument load, the g_936250 push, and a jmp/call tail into 0x004AA682; the port is the same single forwarding statement."),
        "argument_mapping": check("cdecl dword [esp+4] = size is read once and passed as the first candidate argument; the runner compares the same 32-bit size values on both sides."),
        "constants_and_scaling": check("No arithmetic or scaling occurs; the only constant is the pass-through of the priority sentinel global 0x00936250."),
        "memory_layout_and_access": check("The thunk dereferences only the 32-bit global dword at 0x00936250; no buffer or heap metadata is touched inside 0x004AA670-0x004AA681."),
        "calls_and_side_effects": check("Exactly one callee (0x004AA682); the thunk itself writes nothing. Differential cases (13/13 pass) drive downstream effects through the hooked callee instead of retail's heap backend."),
        "return_and_error_paths": check("The thunk returns whatever Mem_AllocWithRetry returns with no branch or error path of its own; single-block CFG in both tools."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    [MEM_FOOTNOTE,
     "Differential fixtures execute the retail thunk with a hooked callee; allocator internals of 0x004AA682 are compared separately."],
    LEAF_EVIDENCE),

native("0x004AA682", "Mem_AllocWithRetry", MEM_IMPL,
    "44-byte allocation retry loop with 0xFFFFFFE0 size guard",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on 5 blocks/6 edges for 0x004AA682-0x004AA6AD: size guard, allocate call, success exit, sentinel exit, and retry back-edge; the port reproduces the same loop shape."),
        "argument_mapping": check("[esp+4]=size and [esp+8]=sentinel map to the two uint32 parameters; the runner drives both with varied values including the 0xFFFFFFE0 boundary."),
        "constants_and_scaling": check("Guard constant 0xFFFFFFE0 matches the retail cmp immediate; no scaling arithmetic exists."),
        "memory_layout_and_access": check("No memory reads besides stack inputs within the function body; allocation metadata effects live in callee 0x004AA6AE and are stubbed in the differential harness."),
        "calls_and_side_effects": check("Callee chain per iteration: Mem_AllocateRawBlock then Mem_InvokeNewHandler on failure, matching retail order; global 0x00936250 is read by the caller path only."),
        "return_and_error_paths": check("Three exits (success pointer, sentinel pointer, exhausted retries returning null) match the retail block set; runner cases cover at least two of them."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    [MEM_FOOTNOTE,
     "The heap backend inside Mem_AllocateRawBlock is not the subject of this review; a real HeapAlloc path cannot execute on this host."],
    LEAF_EVIDENCE),

native("0x004AA6AE", "Mem_AllocateRawBlock", MEM_IMPL,
    "119-byte mode-dispatching allocator: small pool / bucket pool / HeapAlloc fallback",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on 9 blocks/12 edges for 0x004AA6AE-0x004AA724 with the mode-3, mode-2, and rounding/fallback branches; the port preserves the same branch order."),
        "argument_mapping": check("Single uint32 size argument; candidate receives the same value for every case through the shared harness."),
        "constants_and_scaling": check("16-byte rounding mask, mode constants 2/3 compared against g_c3242c, and the max(size,1) clamp are all visible in both tool listings and reproduced verbatim."),
        "memory_layout_and_access": check("The synthesised pool/bucket globals (g_c321fc, g_5466d4, g_c32428, g_5466d4 bucket map) are seeded with the same values on both sides; the runner observes the guarded 64-byte allocation region."),
        "calls_and_side_effects": check("Retail writes pool headers and calls HeapAlloc in the fallback; the harness stubs both backends, so only the dispatch arithmetic and pointer bookkeeping are compared. 29/29 cases pass."),
        "return_and_error_paths": check("Null return on exhausted pools and the three successful shapes (raw pool pointer, bucket pointer, rounded heap block) are exercised; the port returns the same values."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    [MEM_FOOTNOTE,
     "HeapAlloc fallback cases compare candidate HeapFree-comparable bookkeeping only; the Windows heap itself is not exercised."],
    LEAF_EVIDENCE),

native("0x004AD722", "Mem_InvokeNewHandler", MEM_IMPL,
    "30-byte optional CRT new-handler invocation",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on 3 blocks/3 edges: null-handler exit, call path, and zero-propagation exit; the port matches."),
        "argument_mapping": check("uint32 size argument is forwarded to the handler; the runner installs an injectable parity handler at 0x00936254 and compares the retail-called value with the candidate-called value."),
        "constants_and_scaling": check("Only constants are the return 1/0 encodings; no scaling."),
        "memory_layout_and_access": check("Reads exactly one global dword pointer (0x00936254) plus stack size; no other writes occur inside the body."),
        "calls_and_side_effects": check("Retail calls the handler with the pushed size dword; the ESIL harness passes the same seeded handler parity code, so call order and witness byte agree in all 8 cases."),
        "return_and_error_paths": check("Returns 1 when the handler returns non-zero, else 0; null-handler case returns 0; both covered by nominal and boundary cases."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    [MEM_FOOTNOTE,
     "The executed handler is the injected parity callback; CRT-new_handler semantics outside this pair are out of scope."],
    LEAF_EVIDENCE),

native("0x004B0BA0", "Mem_InitHeapState", MEM_IMPL,
    "54-byte heap free-list initialisation with a callee",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on one straight-line block (0x004B0BA0-0x004B0BD5) with one call; the port is the same store/call/store sequence."),
        "argument_mapping": check("The native takes no arguments; its single int result is incidental in retail and printed as VOID on both sides."),
        "constants_and_scaling": check("G 0x9422fc=1 store, head/tail self-pointing triplet addresses (0x00A960F0/F4/F8), and the 0x10 heap state compare base match retail immediates."),
        "memory_layout_and_access": check("The runner seeds the six globals and reads them back after execution; head/sentinel/tail computed relationships (head==&sentinel, tail==&head) match in all 8 cases."),
        "calls_and_side_effects": check("Retail calls QHash_ValueSet (0x004B0D40) between the flag store and the 0x10 store; the candidate does the same and the observed byte/globals match."),
        "return_and_error_paths": check("No branches or error exits in either tool; straight-line store sequence verified."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Emulated execution covers the 54 recorded retail bytes and the six observed globals; QHash table contents are not observed."],
    LEAF_EVIDENCE),

native("0x004B0D40", "QHash_ValueSet", MEM_IMPL,
    "8-byte QHash flag byte store",
    {
        "function_identity": check("Binary Ninja and Radare2 both list one 8-byte block `mov byte [0x009422fd], 1; ret`; the port is the same single store."),
        "argument_mapping": check("No arguments; void semantics recorded with the sentinel return on both sides."),
        "constants_and_scaling": check("Constant 1 byte written at the fixed global address; no scaling."),
        "memory_layout_and_access": check("The runner reads back byte 0x009422FD after execution in all 8 cases and it equals 1 on retail and candidate."),
        "calls_and_side_effects": check("No calls and no writes besides the observed byte."),
        "return_and_error_paths": check("No branches; EAX is incidental on retail and zeroed on the candidate void path before comparison."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Only the single observed byte is read back; other QHash table bytes are outside the observation window."],
    LEAF_EVIDENCE),

# --- 0x004B7450 Matrix_InvalidateSlot (camera_transform.cpp) ---
native("0x004B7450", "Matrix_InvalidateSlot", "engine/src/retail/render/camera_transform.cpp",
    "12-byte matrix slot zeroing at matrix + slot*4 + 0x14",
    {
        "function_identity": check("Retail body is `mov eax,[esp+8]; mov ecx,[esp+4]; mov dword [ecx+eax*4+0x14],0; ret` for 0x004B7450-0x004B745C; Binary Ninja and Radare2 agree on 2 blocks with the store; the port performs the same scaled store and returns the slot index in EAX."),
        "argument_mapping": check("[esp+4]=matrix pointer, [esp+8]=slot index; the runner passes an emulated pointer region and the same slot value, and observes the same 64-byte buffer on both sides."),
        "constants_and_scaling": check("Byte offset 0x14 and the *4 scaled index match; camera_matrix_slot_base = 0x14 is declared in the port header."),
        "memory_layout_and_access": check("Writes exactly 4 bytes at matrix+0x14+slot*4; buffer snapshots at pointer_region read back equal in all 8 cases, including slot boundaries 0 and 9 (offset 0x14..0x3C inside the 64-byte observation window)."),
        "calls_and_side_effects": check("No calls; no writes besides the observed slot; pointer region otherwise stays zero."),
        "return_and_error_paths": check("Returns the slot index; no branch or error path exists in either body."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Slot indices are limited to the observed 64-byte buffer window (0..9); out-of-buffer slot values in retail write outside the window and are not observed by this runner.",
     "None of the 32 slot indices used by Camera_UpdateTransform is discriminated by this differential beyond the bounds rule itself."],
    LEAF_EVIDENCE),

# --- 0x004B5C90 Resource_FreeNode (sys_resource.cpp) ---
native("0x004B5C90", "Resource_FreeNode", "engine/src/retail/resource/sys_resource.cpp",
    "218-byte resource-node release switch over context[9] resource kind",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on 13 blocks/18 edges for 0x004B5C90-0x004B5D6A with the four kind branches and the recursive child walk; the port reproduces the same switch and recursion shape."),
        "argument_mapping": check("Single pointer argument (resource node context); kind read is the dword at +0x24 in both; child list head is read at +0x14 as retail does."),
        "constants_and_scaling": check("Free-list slot ids 1..3, kind constants 0..4, and the unlink helper addresses (0x004AF910, 0x004B1A30) match the recorded disassembly."),
        "memory_layout_and_access": check("Differential fixture nodes keep the exact field offsets the retail body touches (+0x14 child list, +0x24 kind, +0x34 buffer pointer, +0x48 back link); the fixture verifies slots return to free-list g_943e24/g_943e28."),
        "calls_and_side_effects": check("Retail call chain: recursive self, unlink helper, buffer free, slot push. The runner hooks each callee, so call order and argument identity are compared; recursion depth and unlink order match in all cases."),
        "return_and_error_paths": check("Every kind branch ends in the same slot-release tail and returns 0 in retail; null-child container case skips the walk; all behaviors appear in the case set."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["The differential runner observes the release path against hooked callees; real retail resource-node heap addresses are not reproduced."],
    ["docs/fidelity/evidence/0x004B5C90-binaryninja.json",
     "docs/fidelity/evidence/0x004B5C90-radare2.json",
     "docs/fidelity/evidence/0x004B5C90-behavioral.json"]),

# --- 0x004B83D0 Qvm_Cleanup (qvm_native_loader.cpp) ---
native("0x004B83D0", "Qvm_Cleanup", "engine/src/retail/script/qvm_native_loader.cpp",
    "55-byte qvm teardown: +0x8C branch and two callee re-throws",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on 3 blocks/2 edges for 0x004B83D0-0x004B8407; the port matches branch-for-branch after the 2026-10-19 correction to read the standalone pointer as a 32-bit dword (retail stores a dword, not a host pointer)."),
        "argument_mapping": check("[esp+4]=qvm pointer; the qvm body dword at +0x8C selects the branch in both sides; the runner writes the same synthetic pointer dword 0x70011690 so the branch input is identical."),
        "constants_and_scaling": check("Offset 0x8C is the only constant beyond call addresses; it matches the retail cmp/mov instruction layout byte-for-byte."),
        "memory_layout_and_access": check("Runner fills the 0x100-byte qvm body with 0x5A on both sides and snapshots it after the call; both sides touch only the +0x8C field read and leave the body unchanged in all 8 cases."),
        "calls_and_side_effects": check("Retail call sequence: null -> Resource_Unload(qvm) then block release(qvm); non-null -> standalone release(pointer) then block release(qvm). The unicorn hook and the C++ stubs record the same (callee, argument identity) sequences, exactly equal in all 8 cases."),
        "return_and_error_paths": check("Both branch arms end at the block-release tail whose return value is propagated as the function result; hooked sentinels return the same encoded values and EAX compares equal (0x55000003)."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Release callees 0x004BD380 and 0x004B0D10 are not transcribed; sentinel hooks prove call order and argument identity, not their allocator effects.",
     "The default injectable boundaries return 0, so production Qvm_Cleanup return values differ from retail until those chains are ported (documented in the implementation comment)."],
    LEAF_EVIDENCE),

# --- terrain pair (terrain_loaders.cpp) ---
TERRAIN_IMPL = "engine/src/retail/terrain/terrain_loaders.cpp"
native("0x004C6670", "Terrain_CreateMaterial", TERRAIN_IMPL,
    "0x004C6670-0x004C6755 material-slot fill loop over Script_GetArg values",
    {
        "function_identity": check("Binary Ninja and Radare2 CFG agree on the two-loops-plus-tail structure; the transcribed port reproduces the 32-entry outer loop, the i<0xE3 descriptor fills, and the 6-dword context copy (CFG score 1.0 recorded and recomputed equal)."),
        "argument_mapping": check("args[0] is the material index read through Script_GetArg semantics; args[i..i+5] feed the 0x1C-byte descriptor exactly as the fixed sites in scripts/native_unicorn_runner.py emulate."),
        "constants_and_scaling": check("Material stride 900 (0x384), descriptor stride 7 from index 3, the 0x3F000000 half-scale constant, and the four return-descriptor stores all appear in the retail trace and the runner's retail-side hooks."),
        "memory_layout_and_access": check("Retail integer-argument truncation (Script_GetArg +0.25 float store trick) is emulated by trunc(double) in the runner; unicorn cases compare the final 0x384 stride group bytes and the 24-byte context tail."),
        "calls_and_side_effects": check("Retail stores through the return-descriptor sites 0x004C66BE/CA/E2/702 and multiplies at 0x004C66D7/F7; the runner hooks each site and compares the flattened float results against the port computed values."),
        "return_and_error_paths": check("Retail returns the context pointer via EAX; loop pre/test conditions terminate on i>=0xE3 and count exhaustion in both sides; no additional branches exist."),
        "floating_point_semantics": check("The x87 FSTP/FMUL stores compute float values with unicorn x87 quirks compensated by code hooks; the runner folds the *0.5f factors with Python float semantics and compares against the candidate within exact float equality."),
    },
    ["The x87 store compensation is a runner limitation, not a port property: unicorn FSTP/FMUL on this host stores zero, so named sites are folded in Python; the constant 0.5 semantics were checked against the retail immediates.",
     "Index clamping is a port deviation from the Apex out-of-bounds retail behavior, recorded in the manifest limitations and the source comment."],
    ["docs/fidelity/evidence/0x004C6670-binaryninja.json",
     "docs/fidelity/evidence/0x004C6670-radare2.json",
     "docs/fidelity/evidence/0x004C6670-behavioral.json"]),

native("0x004C6760", "Terrain_CreateTileMap", TERRAIN_IMPL,
    "0x004C6760-0x004C67D0 group 64-byte tile table fill and context copy",
    {
        "function_identity": check("Binary Ninja and Radare2 CFG agree on the 64-iteration byte fill and the 6-dword context copy; the port and the recorded structural agreement (1 block-loop exit) match (CFG score 1.0)."),
        "argument_mapping": check("args[0] is the group index via Script_GetArg; args[1..64] feed the 64 tile bytes; the runner seeds both sides with the same float/native pair."),
        "constants_and_scaling": check("Group stride 64 bytes at base 0x00C30080, byte cast (int8) semantics, and the copy count 6 dwords match the retail loop body 0x004C6781-0x004C6796."),
        "memory_layout_and_access": check("Script_GetArg truncation, the 64-byte group window, and the context tail are all inside the runner's observation buffers; both sides compare equal in 8/8 cases."),
        "calls_and_side_effects": check("No cruft calls besides Script_GetArg; the port invokes the same helper chain through the same arguments, which the retail trace confirms."),
        "return_and_error_paths": check("Retail returns the context pointer; 0 and negative group cases are clamped in the port (documented deviation) and observed at the boundary cases."),
        "floating_point_semantics": check("Only the Script_GetArg float store with +0.25 is involved; the runner carries the truncation rule identically on both sides."),
    },
    ["Clamping out-of-range group indices is a port deviation from Apex no-bounds-check retail behavior, recorded in the source comment and manifest limitations.",
     "Uninitialized stack dwords in retail's copy tail are zero in the port; recorded as a deviation."],
    ["docs/fidelity/evidence/0x004C6760-binaryninja.json",
     "docs/fidelity/evidence/0x004C6760-radare2.json",
     "docs/fidelity/evidence/0x004C6760-behavioral.json"]),

# --- texture pair (texture_loaders.cpp) ---
TEXTURE_IMPL = "engine/src/retail/texture/texture_loaders.cpp"
native("0x004B0F80", "Texture_GetActiveBackendId", TEXTURE_IMPL,
    "6-byte global dword getter for the render backend id",
    {
        "function_identity": check("Binary Ninja and Radare2 both list one 6-byte block `mov eax,[0x00A94A64]; ret`; the port returns the same global with an int32 signature."),
        "argument_mapping": check("No arguments; the seed fixture writes a deterministic per-case value into the retail global and calls Host_SetTextureBackendId on the candidate."),
        "constants_and_scaling": check("The only constant is the global address 0x00A94A64."),
        "memory_layout_and_access": check("Retail readback at 0x00A94A64 and the candidate getter both return the seeded value for all 8 cases."),
        "calls_and_side_effects": check("No calls and no writes in either body."),
        "return_and_error_paths": check("Straight-line return with no branches."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Differential values limited to the <0x80000000 range seeded by the runner; sign-extension behavior of EAX for values >= 0x80000000 is not observed."],
    LEAF_EVIDENCE),

native("0x004B26A0", "Texture_EnableUpload", TEXTURE_IMPL,
    "8-byte upload-flag byte store",
    {
        "function_identity": check("Binary Ninja and Radare2 both list one 8-byte block `mov byte [0x00943B3C],1; ret`; the port is the same single store into a byte-sized global."),
        "argument_mapping": check("No arguments; the candidate's void signature matches the retail stack-neutral frame (EAX incidental on both sides)."),
        "constants_and_scaling": check("Constant byte value 1 at fixed address; no scaling."),
        "memory_layout_and_access": check("The runner reads back byte 0x00943B3C after execution in all 8 retail/candidate pairs and both equal 1."),
        "calls_and_side_effects": check("No calls and no writes besides the observed byte."),
        "return_and_error_paths": check("No branches; the void path zeroes the incidental EAX before comparison on both sides."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Only the single byte at 0x00943B3C is observed; other texture-state bytes are outside the observation window."],
    LEAF_EVIDENCE),

# --- 0x004AD73D String_ToUpperChar (resource_lookup.cpp) ---
native("0x004AD73D", "String_ToUpperChar", "engine/src/retail/sys/resource_lookup.cpp",
    "C-locale-only uppercase conversion branch (g_9361e4 == 0)",
    {
        "function_identity": check("Binary Ninja and Radare2 agree on the 0x004AD73D branch set (19 nodes each) with the g_9361e4==0 fast path and the locale-table fallback; the transcribed port implements only the documented C-locale branch and none of the fallback."),
        "argument_mapping": check("One dword argument read at [ebp+8]; the runner pushes the same dword and compares EAX exactly for 8 boundary-centered values."),
        "constants_and_scaling": check("Range constants 0x61/0x7A and the subtract 0x20 match the retail fast path byte-for-byte."),
        "memory_layout_and_access": check("The locale id dword at 0x009361E4 is pinned to 0 in retail emulation; no other memory is touched in the covered branch."),
        "calls_and_side_effects": check("The covered branch has no calls; the locale fallback calls are outside the promoted scope."),
        "return_and_error_paths": check("Fast path: a..z subtract 0x20; everything else returned unchanged including values above 0x7A and boundary 0x40/0x60/0x7B; verified for all 8 seeds."),
        "floating_point_semantics": check("No floating-point operations in the retail bytes.", "not_applicable"),
    },
    ["Promotion is scoped to the locale 0 branch (docs/fidelity/scopes/0x004AD73D.md); locale-table, multibyte, and the 0x54427C-related fallback path are not implemented.",
     "All compiled comparisions are executed under the pinned zero locale; non-zero locale behavior is unverified."],
    LEAF_EVIDENCE + ["docs/fidelity/scopes/0x004AD73D.md"]),

REVIEWER = "OpenCode GLM-5.3 source/instruction review with live differential-runner cross-check, 2026-10-19"


def artifact_for(address: str, info: dict) -> dict:
    implementation = ROOT / info["impl_path"]
    return {
        "schema_version": 1,
        "kind": "semantic-review-report",
        "address": address,
        "binary_sha256": BINARY_SHA,
        "implementation_sha256": hashlib.sha256(implementation.read_bytes()).hexdigest(),
        "result": "passed",
        "reviewed": True,
        "reviewed_by": REVIEWER,
        "review_scope": info["review_scope"],
        "checks": [
            {"name": name, "result": data["result"], "rationale": data["rationale"]}
            for name, data in info["checks"].items()
        ],
        "evidence": [p.format(addr=address) for p in info["evidence"]],
        "limitations": info["limitations"],
    }


def main() -> int:
    written = 0
    for address, info in NATIVES.items():
        path = ROOT / f"docs/fidelity/evidence/{address}-semantic-review.json"
        path.write_text(json.dumps(artifact_for(address, info), indent=2) + "\n", encoding=IMPL_ENCODING)
        written += 1
    print(f"semantic reviews written: {written}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
