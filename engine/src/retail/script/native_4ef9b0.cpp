// Native: Script_Handler_4EF9B0 (placeholder until VERIFIED); retail VA 0x004EF9B0-0x004EF9C5 (22 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 56 57 8b7c240c 8b30 b90b000000 f3a5 5f 5e c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: copies eleven dwords (0x2C bytes) from the buffer whose address is stored at its second argument's target into its first argument: src = *arg2, then 11 dwords from src to arg1.
// Status/limits: TRANSCRIBED (static only); the copy is retail's inline rep movsd of 11 dwords, transcribed as an explicit loop so no library call is introduced; runtime UNOBSERVED.
// Load order matches retail: the source pointer is read before the first source word. The earlier
// `mov eax,[esp+4]` in the candidate is a stack argument load, not a memory dereference.
#include <cstdint>

extern "C" {

void Script_Handler_4EF9B0(std::uint32_t* const destination, void** const source_slot)
{
    const std::uint32_t* const source = reinterpret_cast<std::uint32_t*>(*source_slot); // 0x004EF9BA: mov esi,[eax]
    for (std::int32_t word = 0; word < 0xB; word += 1)                                    // 0x004EF9BC/0x004EF9C1: mov ecx,0xb; rep movsd
    {
        destination[word] = source[word];
    }
}

} // extern "C"
