// Native: Script_SetConstant_51BAA0 (placeholder until VERIFIED); retail VA 0x0051BAA0-0x0051BAAA (11 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 c7000c000000 c3); analysed in docs/fidelity/evidence/0x4EE970-data-extents.md section 9. One of the 84 handler pointers registered by 0x004EE970.
// Behavior/CFG: stores the constant 0xC into the dword pointed to by its value argument and returns that argument's address in EAX. It reads only the value argument (the dispatcher's second argument), never arg0, and makes exactly one 4-byte write-only dereference; there is no global, no string and no callee.
// Status/limits: BUILD_VERIFIED (static only); the store cannot be elided by -O3 because the pointer escapes into the caller's memory, so no volatile is needed; the returned EAX is retail's value argument, preserved for fidelity even though no current caller observes it; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint32_t Script_SetConstant_51BAA0(void* const arg0, std::uint32_t* const value)
{
    (void)arg0;                                          // 0x0051BAA0: first argument is never read by retail
    *value = 0xC;                                   // 0x0051BAA4: mov dword [eax],0xC
    return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(value)); // 0x0051BAAA: ret with EAX = value
}

} // extern "C"
