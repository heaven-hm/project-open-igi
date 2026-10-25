// Native: Sys_GetGlobal16_443E80; retail VA 0x00443E80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00443E80-0x00443E87 (7 bytes, 1 block, 0 edges, 2 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (66 a1 80 c6 53 00 c3).
// Behavior/CFG: loads the two-byte value at 0x0053C680 into ax and returns it; no argument, no branch, no store, no call.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Two-byte is the width this load proves; the value's meaning is not established.
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

// 0x0053C680: the two-byte object this getter reads. It is WRITTEN by the registrar at
// 0x004436A7 (native_443680.cpp), so by the writer-owns rule that routine is its owner;
// this file holds the storage only and the definition should migrate there when that
// native lands.
// Retail 0x0053C680 holds 0x0181 in the image -- the unregistered sentinel of a
// word-sized task-id slot -- not zero. It is overwritten at startup by the registrar
// at 0x004436A7, but the initial value must still match the bytes.
std::uint16_t g_53c680 = 0x181U;

std::uint16_t NATIVE_DECL Sys_GetGlobal16_443E80(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobal16_443E80", "0x00443E80");
    return ::g_53c680;                                                       // 0x00443E80: mov 0x0053c680,%ax; 0x00443E86: ret
}

} // extern "C"

#undef NATIVE_DECL
