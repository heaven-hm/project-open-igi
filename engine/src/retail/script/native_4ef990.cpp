// Native: Script_Handler_4EF990 (placeholder until VERIFIED); retail VA 0x004EF990-0x004EF9A5 (22 bytes, 3 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442408 56 8b742408 57 8b38 b90b000000 f3a5 5f 5e c3); one of the 83 handler pointers embedded by the 0x004EE970 catalogue.
// Behavior/CFG: copies eleven dwords (0x2C bytes) from its first argument to the buffer whose address is stored at its second argument's target: dest = *arg2, then 11 dwords from arg1 to dest.
// Status/limits: TRANSCRIBED (static only); the copy is retail's inline rep movsd of 11 dwords, transcribed as an explicit loop so no library call is introduced; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

void Script_Handler_4EF990(const std::uint32_t* const source, void** const destination_slot)
{
    std::uint32_t* const destination = reinterpret_cast<std::uint32_t*>(*destination_slot); // 0x004EF99A: mov edi,[eax]
    for (std::int32_t word = 0; word < 0xB; word += 1)                                       // 0x004EF99C/0x004EF9A1: mov ecx,0xb; rep movsd
    {
        destination[word] = source[word];
    }
}

} // extern "C"
