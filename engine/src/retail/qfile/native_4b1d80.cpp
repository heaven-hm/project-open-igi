// Native: QFile_SetDriverField_4B1D80; retail VA 0x004B1D80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1D80-0x004B1D9B (28 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf; section 22.2 of the evidence sheet (qfile device-driver table, sixteen-setter family).
// Behavior/CFG: stores the second argument into the 0x8C-stride device-driver record at 0x009436B0, field +0x4C (0x009436FC); no branch, no call, no return value. Completes the sixteen-setter family described in section 22.3.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Table storage is owned by native_4b1180.cpp (g_9436b0). GCC strength-reduces the retail (index*8-index)*5*4 chain to imul 0x8C; blocks, edges and the store offset match retail.
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

void NATIVE_DECL QFile_SetDriverField_4B1D80(std::int32_t index, void* value)
{
    // 0x004B1D80: ecx = index; lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4] -> index*0x8C
    // then ecx = value and the store places it at record+0x4C (0x009436FC)
    *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(&::g_9436b0[0]) + 0x8C * index + 0x4CU) = value;
}

} // extern "C"

#undef NATIVE_DECL
