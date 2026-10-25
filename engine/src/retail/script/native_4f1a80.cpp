// Native: Script_TaskContainer_Append (placeholder until VERIFIED); retail VA 0x004F1A80-0x004F1AE8 (105 bytes, 4 blocks, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly (full listing in 0x00508FA0-disassembly.txt); 7 call sites in 0x508FA0 all clean add esp,0x18; fifth argument proven to be the leftover first argument of the preceding 0x4F0F80 call (7 at 0x509153, 5 at 0x509178); Debug_Error = 0x004AF7B0 retail_error.hpp.
// Behavior/CFG: validates arg4 != -1 (else Debug_Error("Undefined parameter type.") then spins forever at 0x4F1AA0), then appends the 16-byte record {arg3-arg2, arg4, arg5, arg5&0xFF} at payload + count*16, reloading the count before every field store exactly as retail does, and increments the count; returns the container pointer.
// Status/limits: BUILD_VERIFIED (static only); the fifth parameter is stack-passed by the retail through stack reuse, so the transcription takes it as an explicit parameter with the same value - a documented ABI-shape difference, not a behavioural one; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5)
{
    std::int32_t const delta = arg3 - arg2;                        // 0x004F1A88: sub ecx,edx
    if (arg4 == -1) {                                              // 0x004F1A8E: cmp edx,0xFFFFFFFF
        Debug_Error("Undefined parameter type.");                   // 0x004F1A98: call 0x4AF7B0
        for (;;) { }                                               // 0x004F1AA0: jmp $ (retail spins)
    }
    std::uint8_t* const base = reinterpret_cast<std::uint8_t*>(container);
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4)) = delta;            // 0x004F1AB5
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 4) = arg4;         // 0x004F1AC1
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 8) = arg5;         // 0x004F1AD1
    *reinterpret_cast<std::uint8_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 0x0C) = static_cast<std::uint8_t>(arg5); // 0x004F1ADE
    container[0] = container[0] + 1;                               // 0x004F1AE6: mov [eax],ecx (count+1)
    return container;                                              // eax unchanged
}

} // extern "C"
