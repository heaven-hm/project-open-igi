// Native: QFile_SetDriverHandler_4B1DA0; retail VA 0x004B1DA0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1DA0-0x004B1DBB (28 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf; section 22.2 of the evidence sheet (qfile device-driver table).
// Behavior/CFG: stores the second argument into the 0x8C-stride device-driver record at 0x009436B0, field +0x3C (0x009436EC); no branch, no call, no return value. That word is the device sub-record handler 0x004B1420 tests and calls, so this is the only writer of the dispatcher's handler slot.
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

void NATIVE_DECL QFile_SetDriverHandler_4B1DA0(std::int32_t index, void* value)
{
    // 0x004B1DA0: ecx = index; lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4] -> index*0x8C
    // 0x004B1DAD: ecx = value; 0x004B1DB4: mov dword [eax*4 + 0x009436EC], ecx -> record+0x3C
    *reinterpret_cast<void**>(reinterpret_cast<std::uint8_t*>(&::g_9436b0[0]) + 0x8C * index + 0x3CU) = value;
}

} // extern "C"

#undef NATIVE_DECL
