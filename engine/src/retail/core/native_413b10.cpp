// Native: Core_GetFlagByte_413B10; retail VA 0x00413B10 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413B10-0x00413B15 (6 bytes, 1 block, 2 instructions: a0 ad915300 / c3).
// Tools/evidence: radare2 6.2.0 pd; callee gap for 0x00427FF0 (registrar "Missile"), which calls it to read an event id.
// Behavior/CFG: returns the byte at 0x005391AD; no branch, no call, no stack use.
// Status/limits: STRUCTURALLY_VERIFIED for the CFG only (one block, no edges); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean); runtime UNOBSERVED. Codegen deviation recorded, identical to the sibling 0x00413B20: retail writes only AL (mov al, byte [0x005391AD]) and leaves EAX bits 8-31 as the caller left them, while the C++ byte return makes the compiler zero-extend into EAX. The byte-level result is identical for every caller that consumes AL or a uint8_t. Single-owner read: the byte comes from its one owner g_5391ac declared in engine/src/retail/core/native_413df0.cpp, which is also what 0x00413DF0 writes.
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
// engine/src/retail/core/native_413df0.cpp (image values FF FF FF FF FF FF 00). 0x005391AD is its index 1.
extern std::uint8_t g_5391ac[7];

std::uint8_t NATIVE_DECL Core_GetFlagByte_413B10()
{
    return ::g_5391ac[1];   // 0x00413B10: mov al, byte [0x005391AD]
}                            // 0x00413B15: ret

} // extern "C"

#undef NATIVE_DECL
