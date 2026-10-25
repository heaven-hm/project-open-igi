// Native: QFile_GetDeviceName_4B1DE0; retail VA 0x004B1DE0 (placeholder name; retail symbol unknown, and the catalog's Sys_CheckHeap_6 is contradicted by the bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1DE0-0x004B1DF7 (24 bytes, 1 block, 6 instructions: 8b4c2404 / 8d04cd00000000 / 2bc1 / 8d0480 / 8d04852c379400 / c3).
// Tools/evidence: radare2 6.2.0 af/pdf; section 21 of the evidence sheet records the callers 0x004B1420, 0x004B1510 and 0x004B1AC0.
// Behavior/CFG: returns 0x0094372C + index * 0x8C, which is record[index].+0x40 - the NUL-terminated device name pointer; no branch, no call, no memory access.
// Status/limits: BUILD_VERIFIED (1 block / 0 edges, emitted instructions identical); runtime UNOBSERVED. Owns the 0x009436EC table storage because no transcribed native writes it yet (the retail registrar 0x004B1B90 is missing).
#include "igi/retail/qfile/device_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x009436EC: the five-record device table. Port-owned until 0x004B1B90 is transcribed, exactly as native_4015f0.cpp
// owns the 0x00AFA6E0 dispatch stack.
DeviceRecord g_9436ec[5];

char* NATIVE_DECL QFile_GetDeviceName_4B1DE0(std::int32_t index)
{
    // 0x004B1DE0: ecx = index; 0x004B1DE4: eax = ecx*8; 0x004B1DEB: eax -= ecx (7*index);
    // 0x004B1DED: eax += eax*4 (35*index); 0x004B1DF0: eax = eax*4 + 0x0094372C (0x8C*index + record+0x40).
    return &::g_9436ec[index].name[0];
} // 0x004B1DF7: ret

} // extern "C"

#undef NATIVE_DECL
