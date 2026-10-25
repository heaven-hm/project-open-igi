// Native: Task_GetCachedEventId_4E6610; retail VA 0x004E6610-0x004E6628 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004E6610-0x004E6628 (25 bytes, 3 blocks, 8 instructions; per-block instruction counts 3 / 4 / 1 from af + afbj).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image; the cached byte is 0x00548813 (image value 0xFF), the comparison value is 0xFF and the allocation call is 0x004017C0 with a single pushed 0. Same body shape as the sibling lazy allocator at 0x00467D90, but with a different cache and event argument; they are separate routines at separate VAs.
// Behavior/CFG: lazy one-shot event allocation. Block 1 (`mov al,[0x548813]; cmp al,0xFF; jne`) skips the allocation when the cache is already set; block 2 (`push 0; call 0x4017C0; add esp,4; mov [0x548813],al`) allocates event id 0 and stores its low byte; block 3 ret. The value returned in al is that cache byte whether it was already set or just written.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. QTask_AllocateEvent returns the full id in EAX and retail keeps only AL, so the return type is byte-sized. The single branch condition translated is `cached == 0xFF` (0x004E6615/0x004E6617); the 0xFF sentinel is retail's image value, not an assumption, and this routine is the only writer of the cache it owns.
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

std::uint8_t g_548813 = 0xFFU; // retail 0x00548813: cached event id byte; image value 0xFF means "not allocated yet"

std::uint8_t NATIVE_DECL Task_GetCachedEventId_4E6610(void)
{
    if (::g_548813 == 0xFFU)                                                 // 0x004E6610/0x004E6615/0x004E6617: mov al,[0x548813]; cmp al,0xFF; jne
    {
        ::g_548813 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));      // 0x004E6619/0x004E661B/0x004E6620/0x004E6623: push 0; call; add esp,4; mov [0x548813],al
    }
    return ::g_548813;                                                       // 0x004E6628: ret (result in al)
}

} // extern "C"

#undef NATIVE_DECL
