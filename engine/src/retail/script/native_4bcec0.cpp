// Native: Qvm_EvaluateOperandPair_4BCEC0; retail VA 0x004BCEC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCEC0-0x004BCFC3 (260 bytes, 12 blocks, 14 edges, 103 instructions), bytes re-read from the image (8b 44 24 04 83 ec 20 8b 48 1c 8b 50 28 53 33 db 8b 0c 8a 56 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004BCB94 and 0x004BCBB7 (0x004BCAE0), 0x004BD781, 0x004BD7E5, 0x004BDA7A, 0x004BE633, 0x004BE659, 0x004BE6B7 - all push three arguments.
// Behavior/CFG: cdecl evaluator with three stack arguments ([esp+4] state, [esp+8] integer out, [esp+0xC] real out). It switches on type_stack[type index]: tag 1 pops the operand into the integer out through 0x004BCCC0 and stores (float)*(int*)integer out into the real out (0x004BCED4-0x004BCEFB, `fildl`/`fstps` at 0x004BCEE6/0x004BCEEF, returns 1); tag 2 pops into the real out and stores the round-to-nearest 64-bit conversion truncated to 32 bits into the integer out (0x004BCEFC-0x004BCF2D, `flds`/`fistpll` at 0x004BCF10/0x004BCF19, returns 2). Every other tag resolves the operand through 0x004BCD90 (0x004BCF2E-0x004BCF3E) and requires the resolved node's dword 0 to be 0 or 2 (0x004BCF44-0x004BCF4D); it then calls the function pointer at node +0x10 with (local 24-byte buffer, node dword 0x18, 0) (0x004BCF4F-0x004BCF5B, caller cleans 12 bytes), copies 6 dwords of the returned pointer into the local buffer (0x004BCF62-0x004BCF73, `mov $6,%ecx; rep movsl`), and dispatches on the node's dword 4: 1 stores the buffer dword 0 as an integer and as a real (0x004BCF75-0x004BCF94, returns the node dword value 1), 2 converts the double at buffer +8 to the integer out and the double at buffer +0 to the real out (0x004BCF95-0x004BCFBB, `fistpll`/`fstps`, returns 2). Any other combination returns 0 (0x004BCFBC-0x004BCFC3).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs 0x004BCCC0 and 0x004BCD90 (transcribed in this wave); the indirect call target is runtime data, not a fixed retail VA. The x87 conversions are reproduced with `llrintf`/`static_cast<float>` because 0x004BCF19 and 0x004BCFA3 use `fistpll`, which rounds per the x87 control word (default round-to-nearest) and keeps the low 32 bits. The return values 1/2 are the retail EAX values and are reproduced; their meaning is unestablished.
#include <cmath>
#include <cstddef>
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
using QvmNodeValueMethod = void* (__cdecl*)(void* destination, void* source, std::uint32_t argument);
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
using QvmNodeValueMethod = void* (__attribute__((cdecl))*)(void* destination, void* source, std::uint32_t argument);
#else
#define NATIVE_DECL
using QvmNodeValueMethod = void* (*)(void* destination, void* source, std::uint32_t argument);
#endif

extern "C" {

std::int32_t NATIVE_DECL Qvm_PopOperand_4BCCC0(void* state_object, void* out_value, std::uint32_t* out_size,
                                               std::uint32_t* out_tag);
std::int32_t NATIVE_DECL Qvm_ResolveOperand_4BCD90(void* state_object, void** out_node);

std::int32_t NATIVE_DECL Qvm_EvaluateOperandPair_4BCEC0(void* state_object, void* out_integer, void* out_real)
{
    std::uint8_t* const state = static_cast<std::uint8_t*>(state_object);

    // 0x004BCEC7-0x004BCED4: tag of the current operand.
    std::int32_t const type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C);
    std::uint32_t const tag = reinterpret_cast<std::uint32_t*>(*reinterpret_cast<void**>(state + 0x28))
                                  [static_cast<std::ptrdiff_t>(type_index)];

    if (tag == 1U) {
        // 0x004BCED9-0x004BCEE1: PopOperand(state, integer out, 0, 0).
        Qvm_PopOperand_4BCCC0(state_object, out_integer, nullptr, nullptr);
        // 0x004BCEE6-0x004BCEEF: fildl (integer out), fstps (real out).
        std::int32_t const integer_value = *static_cast<std::int32_t*>(out_integer);
        *static_cast<float*>(out_real) = static_cast<float>(integer_value);
        return 1;                                                                   // 0x004BCEF2
    }

    if (tag == 2U) {
        // 0x004BCF01-0x004BCF0B: PopOperand(state, real out, 0, 0).
        Qvm_PopOperand_4BCCC0(state_object, out_real, nullptr, nullptr);
        // 0x004BCF10: flds (real out).
        float const real_value = *static_cast<const float*>(out_real);
        // 0x004BCF19: fistpll rounds per the x87 control word into a 64-bit temporary.
        std::int64_t const widened = std::llrintf(real_value);
        // 0x004BCF1D-0x004BCF22: the low dword of that temporary goes to the integer out.
        *static_cast<std::int32_t*>(out_integer) = static_cast<std::int32_t>(widened);
        return 2;                                                                   // 0x004BCF24
    }

    // 0x004BCF2E-0x004BCF3E: resolve the operand through 0x004BCD90 (its node out slot is the retail's own argument word).
    void* node = nullptr;
    if (Qvm_ResolveOperand_4BCD90(state_object, &node) == 0) {
        return 0;                                                                   // 0x004BCFBC
    }

    std::uint8_t* const node_bytes = static_cast<std::uint8_t*>(node);
    std::uint32_t const node_tag = *reinterpret_cast<std::uint32_t*>(node_bytes);    // 0x004BCF44
    if (node_tag != 0U && node_tag != 2U) {                                         // 0x004BCF46-0x004BCF4D
        return 0;                                                                   // 0x004BCFBC
    }

    // 0x004BCF4F-0x004BCF5B: call node dword 0x10 with (local buffer, node dword 0x18, 0); the caller cleans 12 bytes.
    std::uint32_t buffer[6] = {0U, 0U, 0U, 0U, 0U, 0U};                              // 24-byte local at [esp-0x18]
    QvmNodeValueMethod const method =
        reinterpret_cast<QvmNodeValueMethod>(*reinterpret_cast<void**>(node_bytes + 0x10));
    void* const source = *reinterpret_cast<void**>(node_bytes + 0x18);                // 0x004BCF4F
    void* const returned = method(buffer, source, 0U);

    // 0x004BCF62-0x004BCF73: mov $6,%ecx; rep movsl - a forward 6-dword copy of the returned pointer into the local buffer.
    for (std::uint32_t index = 0U; index < 6U; ++index) {
        buffer[index] = static_cast<std::uint32_t const*>(returned)[index];
    }

    // 0x004BCF6D: node dword 4 selects the conversion.
    std::uint32_t const selector = *reinterpret_cast<std::uint32_t*>(node_bytes + 4);
    if (selector == 1U) {
        // 0x004BCF75-0x004BCF94: buffer dword 0 as integer out, and fildl/fstps as real out.
        std::int32_t const integer_value = static_cast<std::int32_t>(buffer[0]);
        *static_cast<std::int32_t*>(out_integer) = integer_value;
        *static_cast<float*>(out_real) = static_cast<float>(integer_value);
        return 1;                                                                   // EAX still holds the selector value
    }
    if (selector == 2U) {
        // 0x004BCF9A-0x004BCFBB: double at buffer +8 as integer out (fistpll), double at buffer +0 as real out (fstps).
        double integer_source = 0.0;
        double real_source = 0.0;
        std::uint8_t const* const buffer_bytes = reinterpret_cast<std::uint8_t const*>(buffer);
        __builtin_memcpy(&integer_source, buffer_bytes + 8, sizeof integer_source);
        __builtin_memcpy(&real_source, buffer_bytes, sizeof real_source);
        std::int64_t const widened = std::llrint(integer_source);
        *static_cast<std::int32_t*>(out_integer) = static_cast<std::int32_t>(widened);
        *static_cast<float*>(out_real) = static_cast<float>(real_source);
        return 2;                                                                   // EAX still holds the selector value
    }

    return 0;                                                                       // 0x004BCFBC
}

} // extern "C"

#undef NATIVE_DECL
