// Native: Script_Clear_LevelState_4F0610 (placeholder until VERIFIED); retail VA 0x004F0610-0x004F061A (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (c705a458a700000000 c3); one of the 84 code pointers registered by the
// 0x004EE970 catalogue, analysed in docs/fidelity/evidence/0x4EE970-data-extents.md section 8.
// Behavior/CFG: stores 0 into the dword at 0x00A758A4 (region R8 +0x00) and returns. Level's class-method 2 handler, registered through 0x401530(classWord 0x54DD0C, 2, this).
// Status/limits: TRANSCRIBED (static only); 0x00A758A4 has no reader anywhere in the image, so this store is the native's only observable effect and it must stay externally visible or -O3 could delete it; runtime UNOBSERVED.

#include <cstdint>

extern "C" {

extern std::uint32_t g_a758a4;                      // 0x00A758A4, region R8 +0x00
void Script_Clear_LevelState_4F0610()
{
    g_a758a4 = 0;                                        // 0x004F0610: mov dword [0xa758a4],0
}

} // extern "C"
