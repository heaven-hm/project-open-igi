// Native: QFile_InvokeDriverSlot_4B1760; retail VA 0x004B1760 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1760-0x004B177E (31 bytes, 1 block, 0 edges).
// Tools/evidence: radare2 6.2.0 af/afij/pdf; section 22.3 of the evidence sheet (device-operation gate family 0x004B1690-0x004B17F0).
// Behavior/CFG: reads the device index from *context and calls the driver record's +0x24 slot (0x009436D4) with (index, context); returns the slot's EAX. No branch. Retail pushes context then the index and cleans 8.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Table storage is owned by native_4b1180.cpp (g_9436b0); the slot is only populated once the qfile init chain lands. The slot's return type is unproven.
#include "igi/retail/qfile/driver_table.hpp"

#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1760(std::int32_t* context)
{
    using DriverOperation = std::int32_t (NATIVE_DECL*)(std::int32_t index, std::int32_t* context);

    const std::int32_t index = *context;                                // 0x004B1769: ecx = [edx]
    DriverOperation operation;                                          // (index*8-index)*5*4 = index*0x8C, then call [eax*4 + 0x009436D4]
    std::memcpy(&operation, &::g_9436b0[0] + 0x8C * index + 0x24U, sizeof operation);
    return operation(index, context);                                                              // 0x004B1774: push arguments, call, clean the stack, ret
}

} // extern "C"

#undef NATIVE_DECL
