// Native: Qvm_PopOperand_4BCCC0; retail VA 0x004BCCC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCCC0-0x004BCD8E (208 bytes, 24 blocks, 79 instructions), bytes re-read from the image (8b 4c 24 10 8b 44 24 04 56 57 85 c9 74 0b 8b 50 1c 8b 70 28 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004BCC24 (0x004BCC00), 0x004BCDAE (0x004BCD90), 0x004BCEE1 and 0x004BCF0B (0x004BCEC0), 0x004BD86C - every site pushes four arguments.
// Behavior/CFG: cdecl pop of one interpreter operand; four stack arguments ([esp+4] state, [esp+8] value out, [esp+0xC] size out, [esp+0x10] tag out). It reads the tag at type_stack[type index] (0x004BCCD9), stores it through the optional tag out (0x004BCCCA/0x004BCCD7), then switches: tags 1/2/5/6 copy the dword at value_stack[index] to the value out and advance both indexes (0x004BCD14-0x004BCD37, size 4); tags 3/4/7 store value_stack+index*4+4 (the first byte past the length dword) to the value out, read the dword length at value_stack[index], report size length+1 and set value index = index + 1 + length (0x004BCD3E-0x004BCD6C). Any other tag calls Debug_Error with the byte-identical retail string "Unknown data type." at 0x00547AB4 and then spins at 0x004BCD8E forever (0x004BCD81-0x004BCD8E). Returns 1, leaving it in EAX.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs Debug_Error (0x004AF7B0, engine/src/retail/core/retail_error.cpp). Tag meanings beyond the observed 1/2/5/6 dword versus 3/4/7 length-and-bytes split are not established here; the constants are copied from the retail compares, not interpreted.
#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Debug_Error(const char* format, ...);

std::int32_t NATIVE_DECL Qvm_PopOperand_4BCCC0(void* state_object, void* out_value, std::uint32_t* out_size,
                                               std::uint32_t* out_tag)
{
    std::uint8_t* const state = static_cast<std::uint8_t*>(state_object);

    // 0x004BCCC0-0x004BCCD7: the tag at type_stack[type index] is read first and exported when the caller asks for it.
    std::int32_t const type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C);
    std::uint32_t const tag = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x28))
                                  [static_cast<std::ptrdiff_t>(type_index)];
    if (out_tag != nullptr) {
        *out_tag = tag;
    }

    std::uint32_t element_size = 0;

    if (tag == 1U || tag == 2U || tag == 5U || tag == 6U) {
        // 0x004BCD14-0x004BCD37: dword operand; copy the value out and advance both stack indexes by one.
        if (out_value != nullptr) {
            *static_cast<std::uint32_t*>(out_value) =
                reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x24))
                    [static_cast<std::ptrdiff_t>(*reinterpret_cast<std::int32_t*>(state + 0x18))];
        }
        *reinterpret_cast<std::int32_t*>(state + 0x1C) = type_index + 1;
        *reinterpret_cast<std::int32_t*>(state + 0x18) = *reinterpret_cast<std::int32_t*>(state + 0x18) + 1;
        element_size = 4U;
    } else if (tag == 3U || tag == 4U || tag == 7U) {
        // 0x004BCD3E-0x004BCD50: the value out receives the first byte past the length dword of the stored block.
        if (out_value != nullptr) {
            *static_cast<void**>(out_value) = static_cast<void*>(
                reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x24)) +
                *reinterpret_cast<std::int32_t*>(state + 0x18) + 1);
        }
        // 0x004BCD52-0x004BCD6C: size = stored dword count + 1; value index = index + 1 + count; type index += 1.
        std::int32_t const dword_count = reinterpret_cast<std::int32_t*>(*reinterpret_cast<void**>(state + 0x24))
                                             [static_cast<std::ptrdiff_t>(*reinterpret_cast<std::int32_t*>(state + 0x18))];
        element_size = static_cast<std::uint32_t>(dword_count + 1);
        *reinterpret_cast<std::int32_t*>(state + 0x1C) = type_index + 1;
        *reinterpret_cast<std::int32_t*>(state + 0x18) =
            *reinterpret_cast<std::int32_t*>(state + 0x18) + 1 + dword_count;
    } else {
        // 0x004BCD81-0x004BCD8E: byte-identical retail message, then `jmp $` so this path never returns.
        Debug_Error("Unknown data type.");                                          // 0x00547AB4
        for (;;) {
        }
    }

    // 0x004BCD6F-0x004BCD80: optional size out, then EAX = 1.
    if (out_size != nullptr) {
        *out_size = element_size;
    }
    return 1;
}

} // extern "C"

#undef NATIVE_DECL
