// Native: Camera_Get_4F0F80 (placeholder until VERIFIED); retail VA 0x004F0F80-0x004F0F83 (4 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a1 74 18 a7 00 c3; xref writer 0x004EE970 (CameraBase_Init) at 0x004EEE5F.
// Behavior/CFG: mov eax,[0x00A71874]; ret - returns the camera dword.
// Status/limits: TRANSCRIBED; the address is runtime-written and in the .data zero-fill tail (no file bytes), so the port global starts at 0 and only the untranscribed writer 0x4EE970 can change it. Runtime UNOBSERVED.
#include "igi/retail/render/camera_state.hpp"

extern "C" std::uint32_t g_a71874 = 0U; // retail 0x00A71874

extern "C" {

std::uint32_t Camera_Get_4F0F80()
{
    return g_a71874; // 0x004F0F80: a1 74 18 a7 00 (mov eax,[0xA71874]); c3
}

} // extern "C"
