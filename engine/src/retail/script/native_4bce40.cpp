// Native: Qvm_PushStringOperand_4BCE40; retail VA 0x004BCE40 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCE40-0x004BCEB3 (116 bytes, 1 block, 0 edges, 51 instructions), bytes re-read from the image (53 55 56 8b 74 24 14 57 8b fe 83 c9 ff 33 c0 f2 ae ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image plus the linear disassembly; callers found by scanning every `call` in .text: 0x004BCE03 (0x004BCD90), 0x004BD951, 0x004BDBE3, 0x004BE2A4, 0x004BEA08, 0x004BEA7B - all pass the same three-argument shape.
// Behavior/CFG: single-block cdecl helper, three stack arguments ([esp+4] interpreter state, [esp+8] NUL-terminated string, [esp+0xC] type tag). It measures strlen+1 (0x004BCE4F `repnz scas` then `not`), rounds that up to a dword count (0x004BCE61 `add $3`, 0x004BCE64 `and $~3`, 0x004BCE76 `shr $2`), decrements the type index at +0x1C (0x004BCE67/0x004BCE68) and stores the tag at type_stack[new index] (0x004BCE6B), subtracts the dword count from the value index at +0x18 (0x004BCE79/0x004BCE7D), copies exactly strlen+1 bytes to value_stack + index*4 (0x004BCE99 `rep movsl`, 0x004BCEA0 `rep movsb`), then decrements the value index once more (0x004BCEA8) and stores the dword count at value_stack[new index] (0x004BCEAE). It returns the final value index in EAX (0x004BCEA2-0x004BCEAA keep it there to the `ret`).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs no callees (leaf); the state layout is the QVM interpreter prefix documented below. The padding bytes between the copied string and the reserved dword boundary are left untouched by the retail and are left untouched here (memcpy of strlen+1 bytes only). State object layout used by this cluster (read from these bytes; identical offsets to engine/include/igi/retail/script/qvm_native_state.hpp, whose field names were corrected to match these bytes: +0x00 is the byte offset added to the +0x14 pointer (0x004BCFD0 reads both and indexes one by the other): +0x00 byte offset, +0x10 saved instruction pointer (written from +0x14 by 0x004BCD90), +0x14 instruction/node pointer, +0x18 value stack index (dwords), +0x1C type stack index, +0x20 error status, +0x24 value stack pointer, +0x28 type stack pointer, +0x2C stop flag, +0x68 string scratch.
#include <cstddef>
#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Qvm_PushStringOperand_4BCE40(void* state_object, const char* string, std::int32_t tag)
{
    std::uint8_t* const state = static_cast<std::uint8_t*>(state_object);

    // 0x004BCE4A-0x004BCE59: strlen+1 of the string argument (the retail scans with `repnz scas` and inverts the counter).
    const std::size_t text_size = std::strlen(string) + 1U;
    // 0x004BCE61-0x004BCE64 then 0x004BCE76: dword count of the dword-aligned reservation.
    const std::uint32_t reserved_dwords =
        static_cast<std::uint32_t>((text_size + 3U) & ~static_cast<std::ptrdiff_t>(3U)) / 4U;

    // 0x004BCE5B-0x004BCE6B: --type index, then type_stack[index] = tag.
    std::int32_t const type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C) - 1;
    *reinterpret_cast<std::int32_t*>(state + 0x1C) = type_index;
    reinterpret_cast<std::int32_t*>(*reinterpret_cast<void**>(state + 0x28))[static_cast<std::ptrdiff_t>(type_index)] = tag;

    // 0x004BCE6E-0x004BCE82: value index -= dword count, and the copy destination is value_stack + index*4.
    std::int32_t const reserved_index = *reinterpret_cast<std::int32_t*>(state + 0x18) - static_cast<std::int32_t>(reserved_dwords);
    *reinterpret_cast<std::int32_t*>(state + 0x18) = reserved_index;
    std::uint8_t* const copy_destination =
        static_cast<std::uint8_t*>(*reinterpret_cast<void**>(state + 0x24)) + reserved_index * 4;

    // 0x004BCE85-0x004BCEA0: measure the string again and copy exactly strlen+1 bytes (dwords then tail bytes).
    std::memcpy(copy_destination, string, text_size);

    // 0x004BCEA2-0x004BCEAE: --value index once more, then value_stack[index] = dword count.
    std::int32_t const length_index = *reinterpret_cast<std::int32_t*>(state + 0x18) - 1;
    *reinterpret_cast<std::int32_t*>(state + 0x18) = length_index;
    reinterpret_cast<std::int32_t*>(*reinterpret_cast<void**>(state + 0x24))[static_cast<std::ptrdiff_t>(length_index)] =
        static_cast<std::int32_t>(reserved_dwords);

    // 0x004BCEA2/0x004BCEAA leave the decremented value index in EAX; the retail callers ignore it.
    return length_index;
}

} // extern "C"

#undef NATIVE_DECL
