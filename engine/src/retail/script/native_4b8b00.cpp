// Native: Qvm_IndexBelowCount (placeholder until VERIFIED); retail VA 0x004B8B00-0x004B8B13 (20 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b542404 8b4c2408 56 33c0 8b32 3bce 5e 0f9cc0 c3); caller 0x004F1C3C.
// Behavior/CFG: returns 1 when the signed dword in its second argument is less than the signed dword pointed to by its first argument, else 0. It is a single compare with a setl, no branches.
// Status/limits: TRANSCRIBED (static only); the comparison is signed exactly as retail's setl, and the result is 0 or 1 because retail clears eax before the setl; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::int32_t Qvm_IndexBelowCount(const std::int32_t* const limit, std::int32_t index)
{
    return (index < *limit) ? 1 : 0;                     // 0x004B8B0D..0x004B8B10: cmp ecx,esi; setl al (eax was cleared at 0x004B8B09)
}

} // extern "C"
