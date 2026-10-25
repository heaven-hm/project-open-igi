// Native: Task_GetCachedEventId_477C40; retail VA 0x00477C40 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00477C40-0x00477C45 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm 3.01 over the raw bytes (a0 bc 07 54 00 c3), which agree on both instructions; `px 1` at
//   0x005407BC gives 0xFF. The byte is owned and written by core/native_477e30.cpp, which fills it with
//   QTask_AllocateEvent, so this file only reads it and declares it `extern` rather than defining a second copy of the same retail
//   object (§10.15 rule 13).
// Behavior/CFG: loads the one-byte event id at 0x005407BC into al and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. One byte is the width this load proves; the value's meaning is not established. This getter is the
//   event source of a link in core/native_45e450.cpp.
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

// 0x005407BC: the one-byte event id cache. Owned by core/native_477e30.cpp; this file only reads it.
extern std::uint8_t g_5407bc;

std::uint8_t NATIVE_DECL Task_GetCachedEventId_477C40(void)
{
    NATIVE_LOG_ENTER("Task_GetCachedEventId_477C40", "0x00477C40");
    return ::g_5407bc;                                                       // 0x00477C40: mov 0x005407bc,%al; 0x00477C45: ret
}

} // extern "C"

#undef NATIVE_DECL
