// Native: Script_InitTypeCatalogue (placeholder until VERIFIED); retail VA 0x004EE970-0x004EF7EC (3709 bytes, ~1000 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing committed at docs/fidelity/evidence/0x004EE970-full-listing.txt (pD 3709 @ 0x4EE970); data extents under derivation in docs/fidelity/evidence/0x4EE970-data-extents.md.
// Behavior/CFG: IN PROGRESS - section 1 only (0x004EE970-0x004EEA2D). It clears 0x1000 dwords at 0x00A71890, then registers the five base classes Level, Static, Dynamic, Idle and CameraBase through 0x4018F0/0x401900 into the class-id words at 0x0054DD0C, 0x0054DD0E, 0x0054DD10, 0x0054DD12 and 0x0054DD14, then allocates two events through 0x4017C0 storing the returned bytes at 0x0054DDA4 and 0x0054DDA5, and one through 0x401810 storing the returned dword at 0x0054DDA8. The remaining sections continue past 0x004EEA2D and are NOT yet transcribed.
// Status/limits: PARTIAL TRANSCRIPTION - sections 2..N outstanding, so this file is deliberately NOT wired into the build and its native is not claimed as transcribable. The registration call site pushes two zero words before 0x4018F0; 0x401900 consumes only four arguments (verified inside the callee: [esp+8], [esp+0xC], [esp+0x18], [esp+0x1C]), so the fifth word is retail's own unused padding and must not become a fifth parameter. Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {

extern std::uint32_t g_a71890[0x1000]; // 0x00A71890, cleared here with the retail rep stosd (ecx = 0x1000)
extern std::uint16_t g_54dd0c[5];      // 0x0054DD0C..0x0054DD14, class ids for the five base classes
extern std::uint8_t g_54dda4;          // 0x0054DDA4
extern std::uint8_t g_54dda5;          // 0x0054DDA5
extern std::uint32_t g_54dda8;         // 0x0054DDA8

std::uint8_t QTask_NewQTaskEventNoContextFn(std::uint16_t value);        // 0x004EEA27: call 0x401810, one argument (retail pushes zero), returns AL

void Script_InitTypeCatalogue()
{
    for (std::int32_t index = 0; index < 0x1000; index += 1)            // 0x004EE974/0x004EE982: mov ecx,0x1000; rep stosd at [0xa71890]
    {
        g_a71890[index] = 0;                                            // 0x004EE979/0x004EE982: xor eax,eax; rep stosd
    }
    g_54dd0c[0] = TaskType_Register("Level", 0x38, QTask_GetCurrentType(), 0);        // 0x004EE984..0x004EE999: push ebx; push ebx; call 0x4018f0; push eax; push 0x38; push str.Level; call 0x401900; add esp,0x14; mov word [0x54dd0c],ax
    g_54dd0c[1] = TaskType_Register("Static", 0x20, QTask_GetCurrentType(), 0);      // 0x004EE99F..0x004EE9B6
    g_54dd0c[2] = TaskType_Register("Dynamic", 0x20, QTask_GetCurrentType(), 0);     // 0x004EE9BC..0x004EE9D3
    g_54dd0c[3] = TaskType_Register("Idle", 0x20, QTask_GetCurrentType(), 0);        // 0x004EE9D9..0x004EE9F0
    g_54dd0c[4] = TaskType_Register("CameraBase", 0x20, QTask_GetCurrentType(), 0);  // 0x004EE9F6..0x004EEA0B
    g_54dda4 = QTask_AllocateEvent(0);                                  // 0x004EEA11: call 0x4017c0 with the zero pushed at 0x004EEA0A; 0x004EEA17: mov byte [0x54dda4],al
    g_54dda5 = QTask_AllocateEvent(0);                                  // 0x004EEA1C: call 0x4017c0 with the zero pushed at 0x004EEA16; 0x004EEA22: mov byte [0x54dda5],al
    g_54dda8 = QTask_NewQTaskEventNoContextFn(0);                        // 0x004EEA27: call 0x401810 with the zero pushed at 0x004EEA21; 0x004EEA2D: mov dword [0x54dda8],eax (AL zero-extended)
}

} // extern "C"
