// Native: Script_Handler_4EFA10 (placeholder until VERIFIED); retail VA 0x004EFA10-0x004EFA1A (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 c70001000000 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: stores the constant 1 into the dword pointed to by its second argument and returns; the first argument is not read.
// Status/limits: TRANSCRIBED (static only); the constant is 1 exactly as retail encodes it; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void Script_Handler_4EFA10(void* const argument, std::uint32_t* const value)
{
    (void)argument;                                      // first argument is never read by retail
    *value = 1;                                          // 0x004EFA14: mov dword [eax],1
}

} // extern "C"
