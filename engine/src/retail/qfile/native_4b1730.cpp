// Native: QFile_InvokeDriverSlot_4B1730; retail VA 0x004B1730 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1730-0x004B175A (43 bytes, 1 block, 0 edges, 16 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes 8b542404 56 8b742410 8b0a 56 8b742410 8d04cd00000000 56 2bc1 52 51 8d0480 ff1485c8369400 83c410 5e c3; section 22.3 of the evidence sheet.
// Behavior/CFG: reads the device index from *context and calls the driver record's +0x18 slot (0x009436C8) with (index, context, argument2, argument3). The two middle arguments are re-read from the stack after each push, which is retail's own ordering: the third argument is loaded first and pushed first, then the second. Returns the slot's EAX; the only saved register is ESI and the stack is cleaned by 0x10.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Table storage is owned by native_4b1180.cpp (g_9436b0); the slot is only populated once the qfile init chain lands, so this build dispatches a null slot. The slot's return type is unproven.
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

std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1730(std::int32_t* context,
                                                       std::int32_t argument2,
                                                       std::int32_t argument3)
{
    using DriverOperation = std::int32_t (NATIVE_DECL*)(std::int32_t index, std::int32_t* context,
                                                       std::int32_t argument2, std::int32_t argument3);

    const std::int32_t index = *context;                                // 0x004B1739: ecx = [edx]
    DriverOperation operation;                                          // 0x004B1740-0x004B174F: index*0x8C, call [eax*4 + 0x009436C8]
    std::memcpy(&operation, &::g_9436b0[0] + 0x8C * index + 0x18U, sizeof operation);
    return operation(index, context, argument2, argument3);             // 0x004B173B-0x004B1756: push argument3, argument2, context, index; clean 0x10
}

} // extern "C"

#undef NATIVE_DECL
