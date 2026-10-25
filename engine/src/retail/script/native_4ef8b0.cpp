// Native: Script_Handler_4EF8B0 (placeholder until VERIFIED); retail VA 0x004EF8B0-0x004EF8BA (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 c7000b000000 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: stores the constant 0xB into the dword pointed to by its second argument and returns; the first argument is not read.
// Status/limits: BUILD_VERIFIED (static only); the constant is 0xB exactly as retail encodes it; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint32_t Script_Handler_4EF8B0(void* const argument, std::uint32_t* const value)
{
    (void)argument;                                      // first argument is never read by retail
    *value = 0xB;
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value)); // retail EAX on return is the value argument                                        // 0x004EF8B4: mov dword [eax],0xb
}

} // extern "C"
