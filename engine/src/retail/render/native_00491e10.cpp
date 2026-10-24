// Native: Display_SwapBuffers_5_491E10 (placeholder until VERIFIED); retail VA 0x00491E10-0x00491E15 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 86c1e83ddd7b8c496527adafef85f1c623225791359d8bf8adbd9ad652122cd9.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 1c235400 c3); image dword at 0x0054231C holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x0054231C]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SwapBuffers_5_491E10()
{
    return *reinterpret_cast<std::int32_t const*>(0x0054231C); // a1 1c235400 c3
}
} // extern "C"
