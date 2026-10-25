// Native: Qvm_ResolveOperand_4BCD90; retail VA 0x004BCD90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCD90-0x004BCE34 (165 bytes, 7 blocks, 69 instructions), bytes re-read from the image (51 56 8b 74 24 0c 8b 46 1c 8b 4e 28 8b 04 81 83 f8 06 75 15 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004BCC58 (0x004BCC00), 0x004BCF34 (0x004BCEC0), 0x004BD895 and 0x004BDAB2 - all four push exactly two arguments.
// Behavior/CFG: cdecl resolver with two stack arguments ([esp+4] state, [esp+8] node out). It switches on type_stack[type index]: tag 6 pops the operand through 0x004BCCC0 into the node out and returns 1 (0x004BCD9F-0x004BCDB8, EAX still holds PopOperand's 1); tag 4 calls 0x004BCC00 to read the operand as a string (0x004BCDC7-0x004BCDD3; the retail does not test that call's flag), passes the string and the node-out pointer to 0x004BAB80 (0x004BCDD9) and returns 1 when that reports nonzero (0x004BCDE1-0x004BCDEC). Otherwise it copies state +0x14 into state +0x10 (0x004BCDF1/0x004BCE00), sets state +0x20 to the constant 7 (0x004BCDF9), pushes the string back as a tag-4 operand through 0x004BCE40 (0x004BCE03) and copies strlen+1 bytes of the string into the state scratch at +0x68 (0x004BCE14-0x004BCE2D), returning 0 (0x004BCE30). All other tags return 0.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs 0x004BCCC0, 0x004BCC00, 0x004BCE40 (transcribed in this wave) and 0x004BAB80 (missing, declared; 0x004BAB80 is itself a three-argument forwarder to 0x004C0620 with the symbol-table global at 0x00A43E88). What 0x004BAB80 reports and why the failure path sets state +0x20 = 7 and copies state +0x14 to +0x10 are not established here; the constants and stores are reproduced verbatim. The retail writes the string pointer and the size into its own incoming argument word and a saved register slot; this file uses two locals because a C++ body cannot address those slots, and no path reads them after the write.
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
std::int32_t NATIVE_DECL Qvm_PopStringOperand_4BCC00(void* state_object, void** out_string, std::uint32_t* out_size,
                                                     std::uint32_t expected_tag);
std::int32_t NATIVE_DECL Qvm_PushStringOperand_4BCE40(void* state_object, const char* string, std::int32_t tag);
// 0x004BAB80: forwards to 0x004C0620 with the symbol-table global at 0x00A43E88 (see native_4bacd0.cpp) as the first argument.
std::int32_t NATIVE_DECL Script_QHashSymbolLookup_4BAB80(const void* string, void** out_node);

std::int32_t NATIVE_DECL Qvm_ResolveOperand_4BCD90(void* state_object, void** out_node)
{
    NATIVE_LOG_ENTER("Qvm_ResolveOperand_4BCD90", "0x004BCD90");
    std::uint8_t* const state = static_cast<std::uint8_t*>(state_object);

    // 0x004BCD96-0x004BCD9C: tag of the current operand.
    std::int32_t const type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C);
    std::uint32_t const tag = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x28))
                                  [static_cast<std::ptrdiff_t>(type_index)];

    if (tag == 6U) {
        // 0x004BCDA4-0x004BCDB8: pop the operand and report success (EAX still carries PopOperand's 1).
        Qvm_PopOperand_4BCCC0(state_object, out_node, nullptr, nullptr);
        return 1;
    }

    if (tag == 4U) {
        // 0x004BCDBE-0x004BCDD3: read the operand as a string; the retail ignores the returned flag.
        void* string_value = nullptr;                                                // retail used its own argument word
        std::uint32_t string_size = 0;                                               // retail used its saved register slot
        Qvm_PopStringOperand_4BCC00(state_object, &string_value, &string_size, 4U);

        // 0x004BCDD7-0x004BCDD9: look the string up, forwarding the caller's node-out pointer.
        std::int32_t const found = Script_QHashSymbolLookup_4BAB80(string_value, out_node);
        if (found != 0) {
            return 1;                                                                // 0x004BCDE1-0x004BCDEC
        }

        // 0x004BCDF1-0x004BCE00: state +0x10 = state +0x14.
        *reinterpret_cast<std::uint32_t*>(state + 0x10) = *reinterpret_cast<std::uint32_t*>(state + 0x14);
        // 0x004BCDF9: state error status = 7.
        *reinterpret_cast<std::uint32_t*>(state + 0x20) = 7U;
        // 0x004BCDF4-0x004BCE03: push the string back as a tag-4 operand.
        Qvm_PushStringOperand_4BCE40(state_object, static_cast<const char*>(string_value), 4);

        // 0x004BCE08-0x004BCE2D: copy strlen+1 bytes of the same string into the state scratch at +0x68.
        std::memcpy(state + 0x68, string_value, std::strlen(static_cast<const char*>(string_value)) + 1U);
        return 0;                                                                    // 0x004BCE30
    }

    return 0;                                                                        // 0x004BCE30
}

} // extern "C"

#undef NATIVE_DECL
