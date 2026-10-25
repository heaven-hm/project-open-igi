// Native: Qvm_ReadIndexedArgument; retail VA 0x004BD010 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BD010-0x004BD038 (41 bytes, 1 block, 0 edges, 14 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (51 8b 44 24 0c 8b 4c 24 08 6a 02 50 51 8d 54 24 0c ...), re-read from the retail image; the shared declaration this file satisfies already sits at engine/include/igi/retail/script/qvm_execute.hpp:68 area and engine/src/retail/script/qvm_execute.cpp (which declares and calls it without defining it); callers: 0x004B8A50 (Qvm_GetFloatArg) and the QTask handler natives.
// Behavior/CFG: single-block cdecl wrapper, two stack arguments ([esp+4] argument frame, [esp+8] 32-bit index). It clears a 4-byte stack slot, pushes the five arguments of Qvm_ReadArgumentValue_4BCAE0 - destination slot, size 4, frame, index, selector 2 - calls it at 0x004BD02C and then loads the slot with `flds 0x14(%esp)` (0x004BD031) so the real is returned in ST0. `add $0x18,%esp` cleans all five arguments plus the saved register; the callee's EAX (its success flag) is discarded here exactly as in the retail, so a failed read leaves the pre-cleared 0.0 value as the result.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; does not link yet: the called 0x004BCAE0 (274 bytes, 15 blocks, 100 instructions) has no engine native and is declared here with NATIVE_DECL. The selector value 2 is the real-return branch of 0x004BCAE0's dispatch (`fstps` into the destination); selector 1 is the integer branch and selector 3 the string branch, which is why this wrapper is the real variant. The 4-byte slot is declared float because the retail clears it with `movl $0` and reads it with `flds` (single precision in, extended out); no float-to-double rewrite is performed. The frame's own field layout is not interpreted here; it belongs to 0x004BCAE0.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BCAE0 - evaluates one argument of the interpreter frame and writes it into `destination` as integer (selector 1), real (selector 2) or
// NUL-terminated string (selector 3); returns its success flag in EAX. Only the callee's argument read order is established here:
// it reads [esp+4] destination, [esp+8] size, [esp+0xC] frame, [esp+0x10] index, [esp+0x14] selector.
std::int32_t NATIVE_DECL Qvm_ReadArgumentValue_4BCAE0(void* destination, std::uint32_t size,
                                                      void* argument_frame, std::int32_t index,
                                                      std::uint32_t selector);

long double NATIVE_DECL Qvm_ReadIndexedArgument(void* argument_frame, std::int32_t index)
{
    NATIVE_LOG_ENTER("Qvm_ReadIndexedArgument", "0x004BD010");
    float value = 0.0F;                                                            // 0x004BD024: movl $0x0,0x14(%esp) - the 4-byte out slot
    Qvm_ReadArgumentValue_4BCAE0(&value, 4U, argument_frame, index, 2U);            // 0x004BD019-0x004BD02C: push 2; push index; push frame; lea slot; push 4; push slot; call 0x004BCAE0
    return static_cast<long double>(value);                                        // 0x004BD031: flds 0x14(%esp) - real returned in ST0, no conversion
}                                                                                  // 0x004BD035/0x004BD038: add $0x18,%esp; ret

} // extern "C"

#undef NATIVE_DECL
