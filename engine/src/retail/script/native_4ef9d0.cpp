// Native: Script_Handler_4EF9D0 (placeholder until VERIFIED); retail VA 0x004EF9D0-0x004EF9DA (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 c7002c000000 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: stores the constant 0x2C into the dword pointed to by its second argument and returns; the first argument is not read.
// Status/limits: TRANSCRIBED (static only); the constant is 0x2C exactly as retail encodes it; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void Script_Handler_4EF9D0(void* const argument, std::uint32_t* const value)
{
    (void)argument;                                      // first argument is never read by retail
    *value = 0x2C;                                       // 0x004EF9D4: mov dword [eax],0x2c
}

} // extern "C"
