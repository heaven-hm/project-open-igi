// Native: Task_GetCachedEventId_4FDEF0; retail VA 0x004FDEF0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004FDEF0-0x004FDEF5 (6 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm 3.01 over the raw bytes (a0 6a e5 54 00 c3), which agree on both instructions; `px 1` at
//   0x0054E56A gives 0xFF. Nothing else in the tree owns it yet, so this file defines it and is
//   its only writer.
// Behavior/CFG: loads the one-byte event id at 0x0054E56A into al and returns it; no argument, no branch, no store, no call.
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

// 0x0054E56A: the one-byte event id cache. Image value 0xFF ("not allocated yet"), so the initialiser is 0xFF and the symbol class is `D` (initialised `.data`, which
// starts at 0x00536000) rather than `B`. Nothing else in the tree owns it yet.
std::uint8_t g_54e56a = 0xFFU;

std::uint8_t NATIVE_DECL Task_GetCachedEventId_4FDEF0(void)
{
    NATIVE_LOG_ENTER("Task_GetCachedEventId_4FDEF0", "0x004FDEF0");
    return ::g_54e56a;                                                       // 0x004FDEF0: mov 0x0054e56a,%al; 0x004FDEF5: ret
}

} // extern "C"

#undef NATIVE_DECL
