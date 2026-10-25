// Native: Qvm_PopStringOperand_4BCC00; retail VA 0x004BCC00 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCC00-0x004BCCB1 (178 bytes, 9 blocks, 11 edges, 68 instructions), bytes re-read from the image (56 8b 74 24 08 57 8b 46 1c 8b 4e 28 8b 14 81 8b 44 24 18 3b ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004BCB58 (0x004BCAE0), 0x004BCDCA (0x004BCD90), 0x004BDA95, 0x004BE1D9 and 0x004BE1F6 - every site pushes four arguments and compares the fourth against the operand tag.
// Behavior/CFG: cdecl reader that returns a string out of the current operand; four stack arguments ([esp+4] state, [esp+8] string out, [esp+0xC] size out, [esp+0x10] expected tag). When type_stack[type index] already equals the expected tag (0x004BCC06-0x004BCC15) it pops the operand through 0x004BCCC0 with the tag out suppressed (0x004BCC1B-0x004BCC24), reloads the resulting pointer, stores the dword at value_stack[value index] into its own argument word (0x004BCC29-0x004BCC3A, a store no path reads back) and returns strlen(pointer)+1 through the size out (0x004BCC40-0x004BCC49). Otherwise it resolves the operand through 0x004BCD90 (0x004BCC52-0x004BCC58) and, only if that succeeds and the resolved node's dword 0 is 0 or 2 (0x004BCC64-0x004BCC71), exports node dword 0x18 and its strlen+1 when node dword 4 is 3 (0x004BCC73-0x004BCC8F); when node dword 4 is not 3 it still returns 1 with both outs untouched (0x004BCC91). Failure zeroes both outs and returns 0 (0x004BCC99-0x004BCCB1).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs 0x004BCCC0 and 0x004BCD90 (both transcribed in this wave). The resolved node's field meanings are not established; only the observed dword offsets 0, 4 and 0x18 are used. The retail's store into its own [esp+4] argument word at 0x004BCC3A is reproduced through a local because a C++ body cannot write its own incoming argument slot; no path reads that word back, so the behaviour is unchanged.
#include "dev_log.hpp"
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

std::int32_t NATIVE_DECL Qvm_PopOperand_4BCCC0(void* state_object, void* out_value, std::uint32_t* out_size,
                                               std::uint32_t* out_tag);
std::int32_t NATIVE_DECL Qvm_ResolveOperand_4BCD90(void* state_object, void** out_node);

std::int32_t NATIVE_DECL Qvm_PopStringOperand_4BCC00(void* state_object, void** out_string, std::uint32_t* out_size,
                                                     std::uint32_t expected_tag)
{
    NATIVE_LOG_ENTER("Qvm_PopStringOperand_4BCC00", "0x004BCC00");
    std::uint8_t* const state = static_cast<std::uint8_t*>(state_object);

    // 0x004BCC06-0x004BCC15: current operand tag versus the expected tag.
    std::int32_t const type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C);
    std::uint32_t const current_tag = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x28))
                                          [static_cast<std::ptrdiff_t>(type_index)];

    if (current_tag == expected_tag) {
        // 0x004BCC17-0x004BCC24: pop with tag out suppressed; the retail passes its own argument word as the size scratch.
        std::uint32_t discarded_size = 0;
        Qvm_PopOperand_4BCCC0(state_object, out_string, &discarded_size, nullptr);

        // 0x004BCC29-0x004BCC3A: reload the popped pointer, and store value_stack[value index] into the retail's own argument
        // word (dead store - no later instruction reads it).
        void* const popped_value = *out_string;
        discarded_size = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x24))
                             [static_cast<std::ptrdiff_t>(*reinterpret_cast<std::int32_t*>(state + 0x18))];
        (void)discarded_size;

        // 0x004BCC3E-0x004BCC49: size out = strlen(popped pointer) + 1.
        *out_size = static_cast<std::uint32_t>(std::strlen(static_cast<const char*>(popped_value)) + 1U);
        return 1;
    }

    // 0x004BCC52-0x004BCC62: resolve the operand; the retail passes its own argument word as the node out slot.
    void* node = nullptr;
    if (Qvm_ResolveOperand_4BCD90(state_object, &node) == 0) {
        *out_string = nullptr;                                                      // 0x004BCCA3
        *out_size = 0U;                                                             // 0x004BCCA9
        return 0;                                                                   // 0x004BCCAF
    }

    std::uint8_t* const node_bytes = static_cast<std::uint8_t*>(node);
    std::uint32_t const node_tag = *reinterpret_cast<std::uint32_t*>(node_bytes);                    // 0x004BCC68
    if (node_tag != 0U && node_tag != 2U) {                                                          // 0x004BCC6A-0x004BCC71
        *out_string = nullptr;
        *out_size = 0U;
        return 0;
    }

    if (*reinterpret_cast<std::uint32_t*>(node_bytes + 4) == 3U) {                                    // 0x004BCC73
        void* const text = *reinterpret_cast<void**>(node_bytes + 0x18);                              // 0x004BCC79
        *out_string = text;                                                                          // 0x004BCC86
        *out_size = static_cast<std::uint32_t>(std::strlen(static_cast<const char*>(text)) + 1U);      // 0x004BCC88-0x004BCC8F
    }
    return 1;                                                                                        // 0x004BCC91-0x004BCC98
}

} // extern "C"

#undef NATIVE_DECL
