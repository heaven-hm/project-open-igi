// Native: Sys_GetGlobalPtr_413DE0; retail VA 0x00413DE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00413DE0-0x00413DE5 (6 bytes, 1 block, 0 edges, 2 instructions: b8 28e25600 / c3).
// Tools/evidence: radare2 6.2.0 af/afij and the raw bytes decode as `mov $0x0056E228,%eax` then `ret`: it publishes the constant address 0x0056E228. That address is the first of the four name-space objects the registrar 0x00413DF0 owns - 0x0056E248, 0x0056E238, 0x0056E218, 0x0056E228, each 0x10 bytes, the stride fixed by 0x0056E258, all zero in the image. Call sites: 0x0046CCEC (the result is pushed into 0x004F1E20 with slot 7) and 0x0047819A.
// Behavior/CFG: returns the address 0x0056E228 in EAX; no argument, no branch, no memory access, no call, no stack use.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The return type follows the caller that consumes it: engine/src/retail/core/native_477e30.cpp declares `std::uint32_t Sys_GetGlobalPtr_413DE0(void)`, and retail loads the address as a 32-bit immediate. The address is taken from the single owner g_56e228 declared in engine/src/retail/core/native_413df0.cpp rather than a bare literal, so that object keeps one owner; on i686 the codegen is the same `mov $0x56e228,%eax`. The object's meaning is not established beyond the 0x10-byte extent its stride fixes.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0056E228: the fourth name-space object of the registrar 0x00413DF0 in engine/src/retail/core/native_413df0.cpp
// (0x10 bytes, zero in the image; 0x0056E258, the next address any code references, fixes the stride).
extern std::uint8_t g_56e228[0x10];

std::uint32_t NATIVE_DECL Sys_GetGlobalPtr_413DE0(void)
{
    NATIVE_LOG_ENTER("Sys_GetGlobalPtr_413DE0", "0x00413DE0");
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(::g_56e228));   // 0x00413DE0: mov $0x0056E228,%eax
}                                                                                     // 0x00413DE5: ret

} // extern "C"

#undef NATIVE_DECL
