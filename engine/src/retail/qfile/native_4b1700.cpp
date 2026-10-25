// Native: QFile_InvokeDriverSlot_4B1700; retail VA 0x004B1700 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1700-0x004B172A (43 bytes, 1 block, 0 edges, 16 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf; section 22.2 of the evidence sheet. 84 direct call sites in .text (0x00405C7E, 0x0045DED8, 0x004BB981 ...).
// Behavior/CFG: reads the device index from *context, computes index*0x8C and calls the driver record's +0x14 slot (0x009436C4) with (index, context, argument2, argument3); the fourth slot is never read. Corrected 2026-10-25: the body previously forwarded argument3/argument4, which is one slot too far - 0x004B170C loads original arg2 into esi and 0x004B1717 pushes it, while 0x004B1705 loaded arg3 for the earlier push. The earlier claim here that argument2 is "pushed by callers, never read" was false. No branch.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. GCC at the project flags tail-call-optimises this into a register rewrite plus `jmp *slot` where retail pushes four arguments, calls, cleans 0x10 and returns: same slot, same arguments in the same cdecl order, same blocks/edges (1/0), but the return goes straight to the caller. Accepted and recorded rather than defeated with a helper, which rule 9 forbids. The slot's own return type is unproven; table storage is owned by native_4b1180.cpp.
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

std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1700(std::int32_t* context,
                                                       std::int32_t argument2,
                                                       std::int32_t argument3,
                                                       std::int32_t argument4)
{
    (void)argument4;                                                    // 0x004B1700 never reads the fourth slot after the prologue
    using DriverOperation = std::int32_t (NATIVE_DECL*)(std::int32_t index, std::int32_t* context,
                                                      std::int32_t argument2, std::int32_t argument3);
    const std::int32_t index = *context;                                // 0x004B1709: ecx = [edx]
    DriverOperation operation;                                          // 0x004B1710-0x004B171F: call [eax*4 + 0x009436C4]
    std::memcpy(&operation, &::g_9436b0[0] + 0x8C * index + 0x14U, sizeof operation);
    return operation(index, context, argument2, argument3);             // 0x004B171A-0x004B1726: four cdecl arguments, clean 0x10
}

} // extern "C"

#undef NATIVE_DECL
