// Native: FP_InitExtendedPrecision; retail VA 0x00498460.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 0x00498460..0x00498476; Binary Ninja 0x00498460; paired Frida capture retail=3435 candidate=3435 calls.
// Behavior/CFG: FWAIT; FNSTCW the x87 control word into the global at 0x006E5BC4; OR that word with 0x0C00 to select extended precision; FLDCW it back; RET. Every other control bit is preserved.
// Status/limits: VERIFIED; the instruction sequence, the global address and the masked bits are the retail's, and the paired runtime capture observed the same call count on both sides.
#include <cstdint>

namespace igi::retail::core
{
// The retail x87 control word global at 0x006E5BC4.
std::uint16_t g_6e5bc4 = 0U;

void FP_InitExtendedPrecision()
{
    __asm__ volatile("fwait\n\tfnstcw %0\n\torw $0x0c00, %0\n\tfldcw %0"
                     : "+m"(g_6e5bc4) : : "cc", "memory");
}
}
