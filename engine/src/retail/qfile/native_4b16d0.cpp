// Native: QFile_DeviceSlot10Call_4B16D0; retail VA 0x004B16D0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B16D0-0x004B16FA (43 bytes, 1 block, 0 edges, 20 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes (8b542404 56 8b742410 8b0a 56 8b742410 8d04cd00000000 56 2bc1 52 51 8d0480 ff1485c0369400 83c410 5e c3); section 22.3 of the evidence sheet.
// Behavior/CFG: reads the device index stored at the word the first argument points to, indexes the five 0x8C-byte records at 0x009436B0, calls that record's +0x10 slot with (index, argument1, argument2, argument3), and returns the slot's value. The call target comes from the record, so the body has no direct callee and no branch.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The +0x10 slot is installed by 0x004B1C60 from the 0x00509F50 registrar with 0x0050A450, which is not transcribed yet, so the slot stays null in the current tree and this dispatch would fault exactly as retail's would with an unregistered slot. 0x009436B0 storage is owned by native_4b1180.cpp; the slot offset is taken from the raw table bytes because DeviceRecord deliberately names only +0x00, +0x1C and +0x40.
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

using DeviceSlot10Handler = std::int32_t(NATIVE_DECL*)(std::int32_t index, void* context,
                                                       std::int32_t argument2, std::int32_t argument3);

std::int32_t NATIVE_DECL QFile_DeviceSlot10Call_4B16D0(void* context, std::int32_t argument2, std::int32_t argument3)
{
    const std::int32_t index = *reinterpret_cast<const std::int32_t*>(context); // 0x004B16D9: mov ecx,[edx]
    const std::uintptr_t record =                                              // 0x004B16E0-0x004B16EC: lea eax,[ecx*8]; sub eax,ecx; lea eax,[eax+eax*4]
        reinterpret_cast<std::uintptr_t>(&::g_9436b0[0]) + 0x8CU * static_cast<std::uintptr_t>(index);
    const DeviceSlot10Handler handler =                                        // 0x004B16EF: call [eax*4+0x009436C0]
        *reinterpret_cast<const DeviceSlot10Handler*>(record + 0x10U);
    return handler(index, context, argument2, argument3);                      // 0x004B16D5-0x004B16EC: push argument3; push argument2; push context; push index
}

} // extern "C"

#undef NATIVE_DECL
