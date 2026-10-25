// Native: Script_Handler_4EF850 (placeholder until VERIFIED); retail VA 0x004EF850-0x004EF85A (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 c70041000000 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: stores the constant 0x41 into the dword pointed to by its second argument and returns; the first argument is not read.
// Status/limits: STRUCTURALLY_VERIFIED (static only); the constant is 0x41 ('A') exactly as retail encodes it, not a character literal with any other encoding; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint32_t Script_Handler_4EF850(void* const argument, std::uint32_t* const value)
{
    (void)argument;                                      // first argument is never read by retail
    *value = 0x41;
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value)); // retail EAX on return is the value argument                                       // 0x004EF854: mov dword [eax],0x41
}

} // extern "C"
