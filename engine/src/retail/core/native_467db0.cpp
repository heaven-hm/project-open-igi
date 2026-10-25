// Native: Task_GetCachedEventId_467DB0; retail VA 0x00467DB0-0x00467DCB (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00467DB0-0x00467DCB (28 bytes, 3 blocks, 8 instructions, counted per block with af + afbj over the exact image).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf plus a pd read at the VA and at the allocator 0x004017C0; the immediate 0x0181 is the push argument of the allocation call, 0x0053F583 is the cached byte (image value 0xFF, retail's own "not yet allocated" sentinel) and 0xFF is the comparison value. 0x00467DB0 has three references image-wide (0x0044142D, 0x00467764 and 0x00467B5E), so it is not private to the registrar that shares its cache. This body is instruction-for-instruction the same shape as the already-built sibling 0x00467D90 (core/native_00467d90.cpp) against its own cache byte, which is why it takes the sibling's placeholder name rather than a getter-of-a-global name: it is not a plain load, it allocates on first use.
// Behavior/CFG: lazy one-shot allocation of event id 0x0181. Block 1 loads the cached byte at 0x0053F583 and compares it with 0xFF; only when it is 0xFF does block 2 call QTask_AllocateEvent(0x181), discard the pushed argument with `add esp,4` and store the returned low byte back to 0x0053F583; block 3 returns. The returned value is that byte whether it was cached or just allocated.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. QTask_AllocateEvent returns the full id in EAX and retail stores only AL, so the return type here is byte-sized. The 0xFF sentinel is retail's image value, not an assumption, and this routine is the only writer of the cache it owns - 0x00467700 also writes 0x0053F583, but with the value this routine has just returned, so it cannot change what is read next. The allocator 0x004017C0 is transcribed in the 0x0040xxxx lane (core/native_4017c0.cpp or equivalent) and is declared here rather than duplicated; it scans the byte array at 0x0054F9F2 with a stride of 6 from index 0x20 up to 0x0054FF26 and, when it finds no free slot, prints "Unable to allocate event" through 0x004AF7B0 and loops forever at 0x004017E8.
#include "dev_log.hpp"
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

std::uint8_t g_53f583 = 0xFFU; // retail 0x0053F583: cached event id byte; image value 0xFF means "not allocated yet"

std::uint8_t NATIVE_DECL Task_GetCachedEventId_467DB0(void)
{
    NATIVE_LOG_ENTER("Task_GetCachedEventId_467DB0", "0x00467DB0");
    if (::g_53f583 == 0xFFU)                                                     // 0x00467DB0/0x00467DB5/0x00467DB7: mov al,[0x53F583]; cmp al,0xFF; jne
    {
        ::g_53f583 = static_cast<std::uint8_t>(QTask_AllocateEvent(0x181U));     // 0x00467DB9/0x00467DBE/0x00467DC3/0x00467DC6: push 0x181; call; add esp,4; mov [0x53F583],al
    }
    return ::g_53f583;                                                           // 0x00467DCB: ret (result in al)
}

} // extern "C"

#undef NATIVE_DECL
