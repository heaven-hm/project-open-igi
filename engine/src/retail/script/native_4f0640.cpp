// Native: Script_ClearStaticState (placeholder until VERIFIED); retail VA 0x004F0640-0x004F0653 (0x13 bytes, 3 blocks, 3 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (a1b058a700 85c0 740a c705b058a700000000 c3); one of the 84 code pointers registered by the 0x004EE970 catalogue, analysed in docs/fidelity/evidence/0x4EE970-data-extents.md section 8.
// Behavior/CFG: loads the dword at 0x00A758B0 (region R8 +0x0C) into EAX, and when that value is non-zero stores 0 back to the same location, then returns with EAX still holding the value that was loaded. Static's class-method 2 handler, registered through 0x401530(classWord 0x54DD0E, 2, this).
// Status/limits: BUILD_VERIFIED (static only); this is the one handler in the group that reads memory, so the load is observable and must survive -O3. The return value is retail EAX (the value loaded before the conditional store); no caller currently observes it, but it is preserved rather than dropped; runtime UNOBSERVED.

#include <cstdint>

extern "C" {

extern std::uint32_t g_a758b0;                      // 0x00A758B0, region R8 +0x0C (read and conditionally cleared)
std::uint32_t Script_ClearStaticState()
{
    const std::uint32_t previous = g_a758b0;              // 0x004F0640: mov eax,[0xa758b0]
    if (previous != 0)                                   // 0x004F0645/0x004F0647: test eax,eax; je
    {
        g_a758b0 = 0;                                    // 0x004F0649: mov dword [0xa758b0],0
    }
    return previous;                                     // 0x004F0653: ret, eax untouched after the load
}

} // extern "C"
