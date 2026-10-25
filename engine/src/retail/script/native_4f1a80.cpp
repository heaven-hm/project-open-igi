// Native: Script_TaskContainer_Append (placeholder until VERIFIED); retail VA 0x004F1A80-0x004F1AE8 (105 bytes, 4 blocks, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly; SIX arguments. Entry stack is [esp+4]=container, [esp+8]=arg2, [esp+0xC]=arg3, [esp+0x10]=arg4, [esp+0x14]=arg5, [esp+0x18]=arg6. After `push ebx; push esi; push edi` (12 bytes) the read at 0x004F1AB1 `mov bl, byte [esp+0x20]` is entry+0x14 = arg5, read as a BYTE; the intervening `pop edi` at 0x004F1ABD removes 4 of those bytes, so the second read of the same textual offset, 0x004F1AC9 `mov esi, dword [esp+0x20]`, is entry+0x18 = arg6, read as a DWORD. Both are stored (esi at +0x08, bl at +0x0C), so neither is dead. At the 0x508FA0 call sites the two words pushed for the preceding 0x4F0F80 call are reused as arg5 (7) and arg6 (0), which is why the deferred cleanup is `add esp,0x18`; Debug_Error = 0x004AF7B0 retail_error.hpp.
// Behavior/CFG: validates arg4 != -1 (else Debug_Error("Undefined parameter type.") then spins forever at 0x4F1AA0), then appends the 16-byte record {arg3-arg2, arg4, arg6, byte(arg5)} at payload + count*16, reloading the count before every field store exactly as retail does, and increments the count; returns the container pointer.
// Status/limits: BUILD_VERIFIED (static only); the six-argument shape and both stores are read from the instructions above. Earlier evidence in this file claimed only five arguments and stored arg5 in both fields - that was a misreading of two same-offset reads separated by `pop edi`, corrected here. Callers that still pass five arguments leave arg6 reading out of frame, so the call-site backfill must land with or before this change; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

extern "C" {

void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6)
{
    std::int32_t const delta = arg3 - arg2;                        // 0x004F1A88: sub ecx,edx
    if (arg4 == -1) {                                              // 0x004F1A8E: cmp edx,0xFFFFFFFF
        Debug_Error("Undefined parameter type.");                   // 0x004F1A98: call 0x4AF7B0
        for (;;) { }                                               // 0x004F1AA0: jmp $ (retail spins)
    }
    std::uint8_t* const base = reinterpret_cast<std::uint8_t*>(container);
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4)) = delta;            // 0x004F1AB5
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 4) = arg4;         // 0x004F1AC1
    *reinterpret_cast<std::int32_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 8) = arg6;         // 0x004F1AD1: store esi (arg6)
    *reinterpret_cast<std::uint8_t*>(*reinterpret_cast<std::uint8_t**>(base + 8) + (container[0] << 4) + 0x0C) = static_cast<std::uint8_t>(arg5); // 0x004F1ADE: store bl (byte of arg5)
    container[0] = container[0] + 1;                               // 0x004F1AE6: mov [eax],ecx (count+1)
    return container;                                              // eax unchanged
}

} // extern "C"
