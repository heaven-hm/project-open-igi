// Native: Display_ClearSurfaces_5 (placeholder until VERIFIED); retail VA 0x00491E20-0x00491E25 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 cc0802644f55e0c8cc177f618e27139a7d602b03476385bac26736e90554a283.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 20235400 c3); image dword at 0x00542320 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542320]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_ClearSurfaces_5()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542320); // a1 20235400 c3
}
} // extern "C"
