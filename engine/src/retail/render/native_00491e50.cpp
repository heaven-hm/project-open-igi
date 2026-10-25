// Native: Display_SetGammaRamp_5 (placeholder until VERIFIED); retail VA 0x00491E50-0x00491E55 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 840d57fe0a4f10bbfe6a1b8435edf98b8bed8a5b41ce9756c7eedcd104d86bb3.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 30235400 c3); image dword at 0x00542330 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542330]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: BUILD_VERIFIED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SetGammaRamp_5()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542330); // a1 30235400 c3
}
} // extern "C"
