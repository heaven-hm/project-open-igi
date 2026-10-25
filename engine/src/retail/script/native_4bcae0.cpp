// Native: Qvm_ReadArgumentValue_4BCAE0; retail VA 0x004BCAE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCAE0-0x004BCBF1 (274 bytes, 15 blocks, 18 edges, 100 instructions), bytes re-read from the image (83 ec 0c 53 55 56 8b 74 24 24 8b 54 24 28 57 8b 46 0c 33 db ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; callers: 0x004BCACC (0x004BCAB0, selector 1), 0x004BD02C (0x004BD010, selector 0) and 0x004BD05A (0x004BD040, selector 3) - the three accessors already transcribed in this folder.
// Behavior/CFG: cdecl accessor with five stack arguments ([esp+4] destination, [esp+8] destination size, [esp+0xC] argument frame, [esp+0x10] argument index, [esp+0x14] selector). It caches the interpreter pointer at frame +0x0C (0x004BCAEF), saves interpreter +0x14 to the stack and +0x14/+0x1C in registers (0x004BCAF4-0x004BCAFD), loads the argument node from the frame table at frame +0x10 indexed by the argument index (0x004BCB01-0x004BCB07) into interpreter +0x14 (0x004BCB09) and runs 0x004BCFD0 (0x004BCB0F/0x004BCB10). It then re-reads the interpreter (0x004BCB15) and only proceeds when the type index at +0x1C is below its saved value (0x004BCB1B) and the status at +0x20 is zero (0x004BCB27). Selector 1 evaluates through 0x004BCEC0 and stores the integer slot to the destination (0x004BCBBC-0x004BCBCB); selector 2 evaluates and stores the real slot (0x004BCBA0-0x004BCBA8); selector 3 pops a special operand with tag 3 through 0x004BCC00 (0x004BCB58), copies destination size - 1 bytes with the strncpy routine at 0x004A5500 (0x004BCB76) and terminates at destination[size-1] (0x004BCB82). Any other selector calls Debug_Error with the byte-identical retail string "Unknown data type." at 0x00547AB4 and spins at 0x004BCB49 forever. The epilogue restores interpreter +0x14, +0x1C and +0x18 (0x004BCBD5-0x004BCBEA) and returns the success flag that was accumulated in EBX (0), 1 on a completed conversion (0x004BCBE7).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs 0x004BCFD0 (qvm_stack_natives.cpp), 0x004BCEC0, 0x004BCC00 (this wave), the MSVC strncpy at 0x004A5500 (identified, not hand-implemented - see docs/fidelity/evidence/qvm-4a5500-strncpy.md) and Debug_Error (0x004AF7B0). The retail reuses its own +0x0C and +0x10 argument words as the real and integer out slots of 0x004BCEC0 and the string/size out slots of 0x004BCC00; a C++ body cannot address incoming argument words, so two locals carry those slots. Every path reads the slots only through the callee's writes, so the behaviour is unchanged; the argument frame fields +0x0C (interpreter) and +0x10 (node table) are used as observed, their wider meaning is not established here.
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "igi/retail/script/qvm_stack_natives.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Debug_Error(const char* format, ...);
std::int32_t NATIVE_DECL Qvm_PopStringOperand_4BCC00(void* state_object, void** out_string, std::uint32_t* out_size,
                                                     std::uint32_t expected_tag);
std::int32_t NATIVE_DECL Qvm_EvaluateOperandPair_4BCEC0(void* state_object, void* out_integer, void* out_real);
// 0x004A5500 is the statically linked MSVC strncpy, not a game routine: the body carries the MSVC word-scan
// constants 0x7EFEFEFF (0x004A5591) and 0x81010100 (0x004A55A4), zero-fills a short source to the requested count
// (0x004A55E7-0x004A55EF) and leaves the destination in EAX (0x004A55F6). All fifteen retail call sites pass
// (destination, source, count), e.g. this one at 0x004BCB70-0x004BCB76. Following the 0x004A53B3 sprintf precedent in
// engine/src/retail/error/error_show.cpp, the port defines no body and binds the call to the candidate CRT symbol.

std::int32_t NATIVE_DECL Qvm_ReadArgumentValue_4BCAE0(void* destination, std::uint32_t size, void* argument_frame,
                                                      std::int32_t index, std::uint32_t selector)
{
    std::uint8_t* const frame = static_cast<std::uint8_t*>(argument_frame);

    // 0x004BCAEF: the interpreter pointer lives at frame +0x0C.
    std::uint8_t* state = *reinterpret_cast<std::uint8_t**>(frame + 0x0C);

    // 0x004BCAF4-0x004BCAFD: save interpreter +0x18 (stack) and +0x14/+0x1C (registers).
    std::int32_t const saved_value_index = *reinterpret_cast<std::int32_t*>(state + 0x18);  // retail: saved through [esp+0x14]
    void* const saved_node = *reinterpret_cast<void**>(state + 0x14);          // retail: ebp
    std::int32_t const saved_type_index = *reinterpret_cast<std::int32_t*>(state + 0x1C);   // retail: edi

    // 0x004BCB01-0x004BCB09: node = frame table[index], published as the current instruction.
    void* const node = *reinterpret_cast<void**>(frame + 0x10 + static_cast<std::ptrdiff_t>(index) * 4);
    *reinterpret_cast<void**>(state + 0x14) = node;

    // 0x004BCB0F/0x004BCB10: run the bytecode for that node.
    static_cast<void>(igi::retail::script::Qvm_ExecuteInstructions(
        reinterpret_cast<igi::retail::script::QvmNativeStatePrefix*>(state)));

    // 0x004BCB15: the retail re-reads the interpreter pointer from the frame before testing the results.
    state = *reinterpret_cast<std::uint8_t**>(frame + 0x0C);

    std::int32_t result = 0;                                                   // 0x004BCAF2: ebx = 0

    // 0x004BCB1B: proceed only when the type index is still below the saved one, and 0x004BCB27: status is zero.
    if (*reinterpret_cast<std::int32_t*>(state + 0x1C) < saved_type_index &&
        *reinterpret_cast<std::uint32_t*>(state + 0x20) == 0U) {
        // The retail passes its own +0x10 argument word as the integer out and its own +0x0C argument word as the real out; the
        // two locals below carry those slots.
        std::uint32_t integer_slot = 0;
        std::uint32_t real_slot = 0;

        std::uint32_t const selected = selector - 1U;                          // 0x004BCB2F-0x004BCB3A
        if (selected == 0U) {
            // 0x004BCBAC-0x004BCBCB: integer destination.
            if (Qvm_EvaluateOperandPair_4BCEC0(state, &integer_slot, &real_slot) != 0) {
                *static_cast<std::int32_t*>(destination) = static_cast<std::int32_t>(integer_slot);
                result = 1;
            }
        } else if (selected == 1U) {
            // 0x004BCB89-0x004BCBAA: real destination, `flds`/`fstps` at 0x004BCBA0/0x004BCBA8.
            if (Qvm_EvaluateOperandPair_4BCEC0(state, &integer_slot, &real_slot) != 0) {
                *static_cast<float*>(destination) = *reinterpret_cast<const float*>(&real_slot);
                result = 1;
            }
        } else if (selected == 2U) {
            // 0x004BCB4B-0x004BCB87: special operand copied into the destination as a NUL-terminated string.
            void* string_value = nullptr;
            std::uint32_t string_size = 0;
            if (Qvm_PopStringOperand_4BCC00(state, &string_value, &string_size, 3U) != 0) {
                // 0x004BCB76: strncpy(destination, string, size - 1); retail calls its linked MSVC strncpy at 0x004A5500.
                std::strncpy(static_cast<char*>(destination), static_cast<const char*>(string_value), size - 1U);
                // 0x004BCB82: destination[size - 1] = 0.
                static_cast<char*>(destination)[size - 1U] = 0;
                result = 1;
            }
        } else {
            // 0x004BCB3C-0x004BCB49: byte-identical retail message, then `jmp $`.
            Debug_Error("Unknown data type.");                                 // 0x00547AB4
            for (;;) {
            }
        }
    }

    // 0x004BCBD2-0x004BCBEA: restore +0x14, +0x1C and +0x18, then 0x004BCBE7 leaves the flag in EAX.
    *reinterpret_cast<void**>(state + 0x14) = saved_node;
    *reinterpret_cast<std::int32_t*>(state + 0x1C) = saved_type_index;
    *reinterpret_cast<std::int32_t*>(state + 0x18) = saved_value_index;
    return result;
}

} // extern "C"

#undef NATIVE_DECL
