// Native: Script_Handler_4EFA90 (placeholder until VERIFIED); retail VA 0x004EFA90-0x004EFA9E (15 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 8b542404 8b08 8b02 8901 c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: copies the dword pointed to by its first argument into the dword whose address is stored at its second argument's target: dest = *arg2, value = *arg1, *dest = value.
// Status/limits: TRANSCRIBED (static only); the two dereferences are real memory reads and writes and are both preserved; runtime UNOBSERVED.
// Residual difference (Tester-ruled, do not 'fix' with volatile): the optimiser emits the *arg1 load before
// the *arg2 (destination-pointer) load. Retail loads the destination pointer first. Both are loads with no
// store between them, so no success-path state differs. Only the fault order changes if both operands are
// invalid at once.
#include <cstdint>

extern "C" {

void Script_Handler_4EFA90(std::uint32_t* const source, void** const destination_slot)
{
    std::uint32_t* const destination = reinterpret_cast<std::uint32_t*>(*destination_slot); // 0x004EFA98: mov ecx,[eax]
    *destination = *source;                                                                   // 0x004EFA9A/0x004EFA9C: mov eax,[edx]; mov [ecx],eax
}

} // extern "C"
