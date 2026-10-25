// Native: Task_GetCachedEventId_4F1FC0; retail VA 0x004F1FC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1FC0-0x004F1FC5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm 3.01 over the raw bytes (a0 02 6c a7 00 c3), which agree on both instructions; `px 1` at
//   0x00A76C02 gives 0x00. The byte is owned and written by core/native_4f1ee0.cpp, which fills it with
//   QTask_AllocateEvent, so this file only reads it and declares it `extern` rather than defining a second copy of the same retail
//   object (§10.15 rule 13).
// Behavior/CFG: loads the one-byte event id at 0x00A76C02 into al and returns it; no argument, no branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. One byte is the width this load proves; the value's meaning is not established. This getter is the
//   event source of a link in core/native_45e450.cpp.
//   PLACEMENT FLAG (owner-ruled, not a defect): this file sits in core/ while the 0x4F1FD0 family and sys/native_4f0f10.cpp are in sys/. The owner
//   ruled on 2026-10-25 to leave it where it is and record the question here rather than move tracked files, so the bytes and the path are unchanged.
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

// 0x00A76C02: the one-byte event id cache. Owned by core/native_4f1ee0.cpp; this file only reads it.
extern std::uint8_t g_a76c02;

std::uint8_t NATIVE_DECL Task_GetCachedEventId_4F1FC0(void)
{
    NATIVE_LOG_ENTER("Task_GetCachedEventId_4F1FC0", "0x004F1FC0");
    return ::g_a76c02;                                                       // 0x004F1FC0: mov 0x00a76c02,%al; 0x004F1FC5: ret
}

} // extern "C"

#undef NATIVE_DECL
