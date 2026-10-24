// Native: QFile_SetDriverField_4B1D60; retail VA 0x004B1D60 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1D60-0x004B1D7B (28 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf; section 22.2 of the evidence sheet (qfile device-driver table).
// Behavior/CFG: stores the second argument into the 0x8C-stride device-driver record at 0x009436B0, field +0x34 (0x009436E4); no branch, no call, no return value. Retail dispatch gates 0x004B17C0 and 0x004B17F0 both call this slot.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Table storage is owned by native_4b1180.cpp (g_9436b0). GCC strength-reduces the retail (index*8-index)*5*4 chain to `imul $0x8C`; blocks/edges and the store offset match retail.
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

void NATIVE_DECL QFile_SetDriverField_4B1D60(std::int32_t index, void* value)
{
    // 0x004B1D60: ecx = index; lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4] -> index*0x8C
    // 0x004B1D6D: ecx = value; 0x004B1D74: mov dword [eax*4 + 0x009436E4], ecx -> record+0x34
    *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(&::g_9436b0[0]) + 0x8C * index + 0x34U) = value;
}

} // extern "C"

#undef NATIVE_DECL
