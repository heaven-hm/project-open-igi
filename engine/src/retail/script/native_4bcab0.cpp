// Native: Qvm_ReadIndexedInteger_4BCAB0; retail VA 0x004BCAB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BCAB0-0x004BCAD8 (41 bytes, 1 block, 0 edges, 14 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (51 8b 44 24 0c 8b 4c 24 08 6a 01 50 51 8d 54 24 0c ...), re-read from the retail
//   image; caller: 0x004B8A20 (Script_EvalArgInt_4B8A20) at 0x004B8A31; not in the manifest.
// Behavior/CFG: single-block cdecl wrapper, two stack arguments ([esp+4] argument frame, [esp+8] 32-bit index). It clears a 4-byte stack slot,
//   pushes the five arguments of Qvm_ReadArgumentValue_4BCAE0 - destination slot, size 4, frame, index, selector 1 - calls it at 0x004BCACC,
//   reads the slot back with `mov 0x14(%esp),%eax` (0x004BCAD1) and returns it as a 32-bit integer. `add $0x18,%esp` cleans the five arguments
//   plus the saved register; the callee's EAX (its success flag) is discarded, so a failed read returns the pre-cleared 0.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED; does not link yet: 0x004BCAE0 (274 bytes, 15 blocks, 100 instructions) has no engine native
//   and is declared here with NATIVE_DECL. Selector 1 is the integer branch of 0x004BCAE0's dispatch (it stores a dword into the destination),
//   in contrast with selector 2 (real, `fstps`) used by 0x004BD010 and selector 3 (string) used by 0x004BD040. The frame field layout is not
//   interpreted here; it belongs to 0x004BCAE0.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BCAE0 - the shared frame-argument reader; argument read order [esp+4] destination, [esp+8] size, [esp+0xC] frame, [esp+0x10] index,
// [esp+0x14] selector, success flag in EAX.
std::int32_t NATIVE_DECL Qvm_ReadArgumentValue_4BCAE0(void* destination, std::uint32_t size,
                                                      void* argument_frame, std::int32_t index,
                                                      std::uint32_t selector);

std::int32_t NATIVE_DECL Qvm_ReadIndexedInteger_4BCAB0(void* argument_frame, std::int32_t index)
{
    std::int32_t value = 0;                                                        // 0x004BCAC4: movl $0x0,0x14(%esp) - the 4-byte out slot
    Qvm_ReadArgumentValue_4BCAE0(&value, 4U, argument_frame, index, 1U);            // 0x004BCAB9-0x004BCACC: push 1; push index; push frame; lea slot; push 4; push slot; call 0x004BCAE0
    return value;                                                                  // 0x004BCAD1: mov 0x14(%esp),%eax
}                                                                                  // 0x004BCAD5/0x004BCAD8: add $0x18,%esp; ret

} // extern "C"

#undef NATIVE_DECL
