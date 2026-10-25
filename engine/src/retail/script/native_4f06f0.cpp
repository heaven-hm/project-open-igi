// Native: Script_Clear_DynamicState (placeholder until VERIFIED); retail VA 0x004F06F0-0x004F06FA (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (c705ac58a700000000 c3); one of the 84 code pointers registered by the 0x004EE970 catalogue, analysed in docs/fidelity/evidence/0x4EE970-data-extents.md section 8.
// Behavior/CFG: stores 0 into the dword at 0x00A758AC (region R8 +0x08) and returns. Dynamic's class-method 2 handler, registered through 0x401530(classWord 0x54DD10, 2, this).
// Status/limits: BUILD_VERIFIED (static only); 0x00A758AC has no reader anywhere in the image, so this store is the native's only observable effect and it must stay externally visible; runtime UNOBSERVED.

#include <cstdint>

extern "C" {

extern std::uint32_t g_a758ac;                      // 0x00A758AC, region R8 +0x08
void Script_Clear_DynamicState()
{
    g_a758ac = 0;                                        // 0x004F06F0: mov dword [0xa758ac],0
}

} // extern "C"
