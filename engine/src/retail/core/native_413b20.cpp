// Native: Core_GetFlagByte_413B20; retail VA 0x00413B20 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413B20-0x00413B25 (6 bytes, 1 block, 2 instructions: a0 ae915300 / c3).
// Tools/evidence: radare2 6.2.0 pd; ranked as a self-contained leaf of the startup closure in evidence section 20.
// Behavior/CFG: returns the byte at 0x005391AE; no branch, no call, no stack use.
// Status/limits: BUILD_VERIFIED (1 block / 0 edges); runtime UNOBSERVED. Codegen deviation recorded: retail writes only AL (mov al, byte [0x005391AE]) and leaves EAX bits 8-31 as the caller left them, while the C++ byte return makes the compiler zero-extend into EAX. The byte-level result is identical for every caller that consumes AL or a uint8_t. Single-owner read since 2026-10-25: this file used to define a private `g_5391ae = 0xFFU`, which the registrar 0x00413DF0 never writes, so the native returned the image value forever instead of the event id stored at 0x005391AE. The byte now comes from its one owner g_5391ac (index 2) declared in engine/src/retail/core/native_413df0.cpp, which is also what 0x00413DF0 writes (see docs/fidelity/evidence/data-slot-ownership-20261025.md).
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x005391AC: the seven-byte event-id block 0x005391AC..0x005391B2, owned by the registrar 0x00413DF0 in
// engine/src/retail/core/native_413df0.cpp (image values FF FF FF FF FF FF 00). 0x005391AE is its index 2.
extern std::uint8_t g_5391ac[7];

std::uint8_t NATIVE_DECL Core_GetFlagByte_413B20()
{
    return ::g_5391ac[2];   // 0x00413B20: mov al, byte [0x005391AE]
}                           // 0x00413B25: ret

} // extern "C"

#undef NATIVE_DECL
