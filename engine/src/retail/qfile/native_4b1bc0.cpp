// Native: QFile_InvokeDriverSlot_4B1BC0; retail VA 0x004B1BC0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1BC0-0x004B1BD9 (26 bytes, 1 block, 0 edges, 7 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep rows).
// Behavior/CFG: dispatches the driver record's +0x04 slot (0x009436B4) with the single index argument, then pops the pushed copy; the slot's EAX is returned. No branch.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Any word it touches and no other translation unit owns is declared here and starts zeroed.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

#include "igi/retail/qfile/driver_table.hpp"

#include <cstring>


std::int32_t NATIVE_DECL QFile_InvokeDriverSlot_4B1BC0(std::int32_t index)
{
    using DriverOperation = std::int32_t (NATIVE_DECL*)(std::int32_t index);
    DriverOperation operation;
    std::memcpy(&operation, &::g_9436b0[0] + 0x8C * index + 0x04U, sizeof operation);  // 0x004B1BC5-0x004B1BD1: call [eax*4 + 0x009436B4]
    return operation(index);                                                          // 0x004B1BC4/0x004B1BD8: push index; call; pop ecx; ret
}   // 0x004B1BD9: ret

} // extern "C"

#undef NATIVE_DECL
