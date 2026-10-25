// Native: Display_CloseDevice_5 (placeholder until VERIFIED); retail VA 0x00491DE0-0x00491DE5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 c2e1a0056243a3f0ae9be868c4cfb9c06d0929348feff0ba86216c7605eb9fe4.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 10235400 c3); image dword at 0x00542310 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542310]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: BUILD_VERIFIED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_CloseDevice_5()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542310); // a1 10235400 c3
}
} // extern "C"
