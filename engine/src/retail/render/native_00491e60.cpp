// Native: Display_CreateSurface_5 (placeholder until VERIFIED); retail VA 0x00491E60-0x00491E65 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 115900e59199027eb6bea677a6e2bda8553bd688aa9e2815546db5955ef699ef.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 34235400 c3); image dword at 0x00542334 holds 0xFFFFFFFF. The slot is the named object g_542334 defined in engine/src/retail/render/native_491ea0.cpp, so this file references it by name instead of repeating the address.
// Behavior/CFG: mov eax, dword [0x00542334]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: BUILD_VERIFIED (static only); the returned dword is the engine global g_542334 (retail 0x00542334, image value -1), which retail also writes at 0x004917AB and reads in 0x00491EA0; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

extern std::int32_t g_542334; // 0x00542334, defined in engine/src/retail/render/native_491ea0.cpp

std::int32_t Display_CreateSurface_5()
{
    return g_542334; // a1 34235400 c3
}
} // extern "C"
