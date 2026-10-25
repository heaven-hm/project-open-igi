// Native: Task_GetCachedEventId_5096A0; retail VA 0x005096A0-0x005096B8 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x005096A0-0x005096B8 (25 bytes, 3
//   blocks, 8 instructions, counted per block with af + afbj over the exact image).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf plus a pd 15 read at the VA; `px 1` at 0x0054E83E gives 0xFF, retail's own "not yet allocated"
//   sentinel, and 0xFF is also the immediate the comparison uses. The allocation argument is the literal ZERO pushed at 0x005096A9, and
//   QTask_AllocateEvent (0x004017C0) is declared by igi/retail/core/task_types.hpp. The body is instruction-for-instruction the same shape as
//   0x00450CA0, which is a separate routine with its own cache byte and is transcribed separately - this is a duplicated retail pattern, not a
//   duplicate implementation.
// Behavior/CFG: lazy one-shot allocation of an event id. Block 1 (0x005096A0-0x005096A5) loads the cached byte at 0x0054E83E and compares it with
//   0xFF; the conditional jump to block 3 is taken when it is not 0xFF. Block 2 (0x005096A9-0x005096B3) pushes the literal 0, calls
//   QTask_AllocateEvent, discards the pushed argument with `add esp,4` and stores the returned low byte back to 0x0054E83E, then falls into block 3.
//   Block 3 (0x005096B8) returns, with the cached byte as the result whether it was cached or just allocated.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. QTask_AllocateEvent returns the id in EAX and this body stores only AL, so the return type here is
//   byte-sized. The 0xFF sentinel is retail's image value, not an assumption, and this routine is the only writer of the cache it owns. The meaning of
//   the ZERO argument is NOT established: 0x00467D90 passes 0x0181 to the same callee, so the argument is load-bearing somewhere and is transcribed
//   as the literal retail pushes rather than interpreted.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0054E83E: the cached event id byte. Image value 0xFF ("not allocated yet"), so the initialiser is 0xFF and the symbol class is `D` (initialised
// `.data`) rather than `B`. This file is its only writer.
std::uint8_t g_54e83e = 0xFFU;

std::uint8_t NATIVE_DECL Task_GetCachedEventId_5096A0(void)
{
    if (::g_54e83e == 0xFFU)                                                      // 0x005096A0/0x005096A5/0x005096A7: mov al,[0x54E83E]; cmp al,0xFF; jne
    {
        // 0x005096A9: push 0 (the allocation argument); 0x005096AB: call 0x4017C0; 0x005096B0: add esp,4; 0x005096B3: mov [0x54E83E],al
        ::g_54e83e = static_cast<std::uint8_t>(QTask_AllocateEvent(0U));
    }
    return ::g_54e83e;                                                           // 0x005096B8: ret (result in al)
}

} // extern "C"
