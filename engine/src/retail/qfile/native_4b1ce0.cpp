// Native: QFile_SetDriverField_4B1CE0; retail VA 0x004B1CE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1CE0-0x004B1CFC (28 bytes, 1 block, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf; section 22.2 of the evidence sheet (qfile device-driver table).
// Behavior/CFG: stores the second argument into the 0x8C-stride device record at 0x009436B0, field +0x20; no branch, no call, no return value.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The table storage is owned by native_4b1180.cpp.
#include "igi/retail/qfile/driver_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL QFile_SetDriverField_4B1CE0(std::int32_t index, void* value)
{
    // 0x004B1CE0: ecx = index; lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4] -> index*0x8C
    // then ecx = value and the store `mov dword [eax*4 + 0x009436B0+field], ecx` places it
    *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(&::g_9436b0[0]) + 0x8C * index + 0x20U) = value;
}

} // extern "C"

#undef NATIVE_DECL
