// Native: Script_ParameterEventSlotClear (placeholder until VERIFIED); retail VA 0x004F1B50-0x004F1B5C (12 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (0fbe442404 c680c858a7.. 00 c3 90); the byte index is a signed char ([esp+4]) and the array is the same 16-slot table declared by 0x4F1B20.
// Behavior/CFG: clears the indexed slot of the 16-byte parameter-event table to zero and returns nothing.
// Status/limits: STRUCTURALLY_VERIFIED (static only); the signed-char index is preserved as retail sign-extends it, so a negative index addresses before the table exactly as retail does; the array itself is owned by 0x4F1B20, not redefined here; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint8_t g_a758c8[16]; // 0x00A758C8, owned by 0x4F1B20

void Script_ParameterEventSlotClear(std::int8_t const index)
{
    g_a758c8[index] = 0; // 0x004F1B55: mov byte [eax+0xa758c8],0 with eax = movsx [esp+4]
}

} // extern "C"
