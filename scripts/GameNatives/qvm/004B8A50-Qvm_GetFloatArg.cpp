// Canonical GameNatives candidate: Qvm_GetFloatArg - retail VA 0x004B8A50.
//
// THIS IS A RECONSTRUCTION, NOT A VERBATIM ORIGINAL SOURCE.
// The Project IGI 1 retail binary ships without source. This file is a transcription of the
// machine code at 0x004B8A50 in igi.exe, written from the retail disassembly/decompilation and
// kept in C++ form so the candidate can be reviewed. It is not the original source and it does
// not claim instruction-for-instruction identity.
//
// Artifact: retail igi.exe, SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Retail body: VA 0x004B8A50-0x004B8A7B, 44 bytes,
//              SHA-256 209a558f2efce4270ed8eca0ca5e9c586022d1ea9482970483249b59e5944d63
//              (verified by hashing the retail .text bytes at file offset 0xB8A50).
// Tools/evidence: Binary Ninja 6.0.10601 MCP, Radare2 6.2.0 and angr 10.0.4.post1 CFGFast on the
//              retail IGI.EXE; docs/fidelity/evidence/0x004B8A50-*.json.
// CFG: 3 blocks / 2 edges in the retail image (Binary Ninja, Radare2 and the recompiled candidate
//              object all agree; angr splits at the calls and recovers 5 blocks / 4 edges, no
//              unresolved jumps).
// Signature: x86 cdecl long double(void* argument_frame, int32_t index). Binary Ninja's stored
//              name (Profile_ValueSet) and prototype int32_t(float,float) are stale: the 201 call
//              sites (for example DefineHumanPlayerGeneral at 0x00410070) call it cdecl and consume
//              the result with `fstp dword [global]`, and both of its callees leave an 80-bit value
//              in ST0, so the helper returns the real on the x87 stack, not an integer in EAX. The
//              name Qvm_GetFloatArg is a descriptive label, not a recovered retail symbol.
// Behavior: reads the frame flag at +0x04; when it is non-zero the argument comes from the frame's
//              indexed value reader 0x004BD010(frame, index), otherwise from the argument table at
//              +0x08: the 32-bit entry at table + index*4 is an expression node evaluated by
//              0x004B9660. Both callees return the value in ST0 and this helper returns it
//              unchanged (tail call, no conversion of its own).
// Status: STRUCTURALLY_VERIFIED against retail 0x004B8A50 (3 blocks/2 edges in both the retail image
//              and the recompiled object). Arithmetic, calls and side effects are reviewed in
//              docs/fidelity/evidence/0x004B8A50-semantic-review.json. NOT BUILD_VERIFIED as a
//              standalone unit, NOT RUNTIME_TESTED and NOT VERIFIED: the two retail callees are
//              declared but not transcribed on this branch, so the object does not link yet and no
//              argument frame is ever produced by candidate code.
//
// Canonical implementation record: engine/src/retail/script/qvm_execute.cpp (this file is the
// address-bound GameNatives reference copy of the same transcription, not a second implementation).
//
// The previous file for this address was an uncorrected Ghidra/r2 dump at
// scripts/GameNatives/profile/004B8A50-Profile_ValueSet.cpp, which carried the stale
// int(float,float) signature and an uninitialised return on the indexed branch. It was archived to
// docs/fidelity/evidence/superseded/004B8A50-Profile_ValueSet-ghidra-dump-20261023.cpp and replaced
// by this transcription of the retail body under the retail-address-qualified qvm name.

#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

// Retail callees of 0x004B8A50. Neither is transcribed on this branch; both are declared with
// their retail cdecl signature and stay unresolved at link time, exactly as recorded for the
// canonical implementation.
extern "C" {

// 0x004BD010 - reads argument `index` from the interpreter frame and returns it as a real in ST0
// (the retail body ends with `fld dword [esp+0x14]`).
long double IGI_RETAIL_CDECL Qvm_ReadIndexedArgument_4BD010(void* argument_frame,
                                                            std::int32_t index);

// 0x004B9660 - evaluates one argument expression node and leaves its real value in ST0
// (`fild` / `fld dword` / `fld qword`); its recursion, type dispatch, "Unknown identifier
// '%s' in expression, line %d" error report and 0x00A965A8 error store live inside it.
long double IGI_RETAIL_CDECL Qvm_EvaluateArgumentValue_4B9660(const void* node);

} // extern "C"

namespace {

// Retail offsets inside the frame handed to a running script native.
constexpr std::size_t kArgumentTableFlagOffset = 0x04U;
constexpr std::size_t kArgumentTableOffset = 0x08U;

[[nodiscard]] std::uint32_t load_word(const void* base, std::size_t offset) noexcept
{
    std::uint32_t value = 0;
    std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof value);
    return value;
}

[[nodiscard]] void* pointer_from_word(std::uint32_t word) noexcept
{
    // The retail keeps 32-bit pointers in its QVM object and argument table.
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(word));
}

} // namespace

extern "C" long double IGI_RETAIL_CDECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index)
{
    if (load_word(argument_frame, kArgumentTableFlagOffset) != 0U)
    {
        return Qvm_ReadIndexedArgument_4BD010(argument_frame, index);
    }

    const std::uint32_t table = load_word(argument_frame, kArgumentTableOffset);
    const auto* entries = reinterpret_cast<const std::uint32_t*>(
        static_cast<std::uintptr_t>(table));
    const std::uint32_t node = entries[static_cast<std::uint32_t>(index)];
    return Qvm_EvaluateArgumentValue_4B9660(pointer_from_word(node));
}
