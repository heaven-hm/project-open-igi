// Native: QFile_GetResolvedAlias_4B1FB0; retail VA 0x004B1FB0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1FB0-0x004B1FB5 (6 bytes, 1 block, 0 edges, 2 instructions); 0x004B1FB6-0x004B1FBF is 0x90 nop padding and 0x004B1FC0 begins the next native.
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes (b8 18 3a 94 00 c3) with NASM ndisasm; immediate references to 0x00943A18 are exactly four: 0x004B10A5, 0x004B1169 (the two copies in 0x004B1020), 0x004B121F (0x004B1200 initialises the first byte to 0) and 0x004B1FB1 (this getter).
// Behavior/CFG: no arguments, returns the 0x00943A18 buffer address in eax; one block, straight-line, no side effects.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Its caller at 0x004B5F6C is the only consumer; in the port it replaces QFile_GetResolvedAlias (alias_resolve.cpp) so that deleting that file keeps resource_lifecycle.cpp's Sys_Resource call wired to the retail buffer.
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

// 0x00943A18: defined in qfile/native_4b1020.cpp (the landed writer of the buffer).
extern std::uint8_t g_943a18[0x80];

} // extern "C"

namespace igi::retail::qfile {

char* NATIVE_DECL QFile_GetResolvedAlias_4B1FB0()
{
    NATIVE_LOG_ENTER("QFile_GetResolvedAlias_4B1FB0", "0x004B1FB0");
    return reinterpret_cast<char*>(&::g_943a18[0]);                   // 0x004B1FB0: mov eax,0x00943A18; 0x004B1FB5: ret
}

} // namespace igi::retail::qfile

#undef NATIVE_DECL
