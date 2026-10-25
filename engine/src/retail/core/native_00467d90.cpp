// Native: Task_GetCachedEventId_467D90; retail VA 0x00467D90-0x00467DAB (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00467D90-0x00467DAB
//   (28 bytes, 3 blocks, 8 instructions, counted per block with af + afbj over the exact image).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf plus a pd 12 read at the VA; the immediate 0x0181 is the push argument of the allocation call,
//   0x0053F582 is the cached byte (image value 0xFF, retail's own "not yet allocated" sentinel) and 0xFF is the comparison value.
// Behavior/CFG: lazy one-shot allocation of event id 0x0181. Block 1 loads the cached byte at 0x0053F582 and compares it with 0xFF; only when
//   it is 0xFF does block 2 call QTask_AllocateEvent(0x181), discard the pushed argument with `add esp,4` and store the returned low byte back
//   to 0x0053F582; block 3 returns. The returned value is that byte whether it was cached or just allocated.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. QTask_AllocateEvent returns the full id in EAX and retail stores only AL, so the return type
//   here is byte-sized. The 0xFF sentinel is retail's image value, not an assumption, and this routine is the only writer of the cache it owns.
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

std::uint8_t g_53f582 = 0xFFU; // retail 0x0053F582: cached event id byte; image value 0xFF means "not allocated yet"

std::uint8_t NATIVE_DECL Task_GetCachedEventId_467D90(void)
{
    if (::g_53f582 == 0xFFU)                                                     // 0x00467D90/0x00467D95/0x00467D97: mov al,[0x53F582]; cmp al,0xFF; jne
    {
        ::g_53f582 = static_cast<std::uint8_t>(QTask_AllocateEvent(0x181U));     // 0x00467D99/0x00467D9E/0x00467DA3/0x00467DA6: push 0x181; call; add esp,4; mov [0x53F582],al
    }
    return ::g_53f582;                                                           // 0x00467DAB: ret (result in al)
}

} // extern "C"

#undef NATIVE_DECL
