// Native: Task_GetCachedEventId_460C10; retail VA 0x00460C10-0x00460C28 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00460C10-0x00460C28
//   (25 bytes, 3 blocks, 8 instructions; per-block instruction counts 3 / 4 / 1 from af + afbj).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; the cached byte is 0x0053F315 (image value 0xFF), the comparison value is
//   0xFF and the allocation call is 0x004017C0 with a single pushed 0. Same body shape as the sibling lazy allocators at 0x00467D90 and
//   0x00460BF0 (whose cache 0x0053F314 sits one byte below this one); each is a separate routine at its own VA.
// Behavior/CFG: lazy one-shot event allocation. Block 1 (`mov al,[0x53F315]; cmp al,0xFF; jne`) skips allocation when the cache is already
//   set; block 2 (`push 0; call 0x4017C0; add esp,4; mov [0x53F315],al`) allocates event id 0 and stores its low byte; block 3 ret. The value
//   returned in al is that cache byte whether it was already set or just written.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. QTask_AllocateEvent returns the full id in EAX and retail keeps only AL, so the return type is
//   byte-sized. The single branch condition translated is `cached == 0xFF` (0x00460C15/0x00460C17); the 0xFF sentinel is retail's image
//   value, and this routine is the only writer of the cache it owns.
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

std::uint8_t g_53f315 = 0xFFU; // retail 0x0053F315: cached event id byte; image value 0xFF means "not allocated yet"

std::uint8_t NATIVE_DECL Task_GetCachedEventId_460C10(void)
{
    if (::g_53f315 == 0xFFU)                                                 // 0x00460C10/0x00460C15/0x00460C17: mov al,[0x53F315]; cmp al,0xFF; jne
    {
        ::g_53f315 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));      // 0x00460C19/0x00460C1B/0x00460C20/0x00460C23: push 0; call; add esp,4; mov [0x53F315],al
    }
    return ::g_53f315;                                                       // 0x00460C28: ret (result in al)
}

} // extern "C"

#undef NATIVE_DECL
