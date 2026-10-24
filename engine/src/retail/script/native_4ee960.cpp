// Native: Script_HashBucket (placeholder until VERIFIED); retail VA 0x004EE960-0x004EE96F (16 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (8b442404 33d2 b9fb000000 f7f1 8bc2 c3): unsigned div by 0xFB and eax = edx, i.e. the remainder.
// Behavior/CFG: returns arg0 modulo 251 (unsigned 32-bit division; the divisor constant 0xFB is retail's bucket count); single straight-line block.
// Status/limits: TRANSCRIBED (static only); unsigned division preserved (retail zeroes edx before div, no signed fixup), divisor kept as 251 exactly; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint32_t Script_HashBucket(std::uint32_t const value)
{
    return value % 0xFBU; // 0x004EE966: mov ecx,0xfb; 0x004EE96B: div ecx; eax = remainder
}

} // extern "C"
