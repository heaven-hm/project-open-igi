// Native: Camera_Get_4F1000 (placeholder until VERIFIED); retail VA 0x004F1000-0x004F1003 (4 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0 bytes a1 90 0c a7 00 c3; xref writer 0x004EE970 (CameraBase_Init) at 0x004EF4DA.
// Behavior/CFG: mov eax,[0x00A70C90]; ret - returns the second camera dword.
// Status/limits: STRUCTURALLY_VERIFIED; runtime-written address in the .data zero-fill tail (no file bytes), port global starts at 0; only 0x4EE970 can change it. Runtime UNOBSERVED.
#include "igi/retail/render/camera_state.hpp"

extern "C" std::uint32_t g_a70c90 = 0U; // retail 0x00A70C90

extern "C" {

std::uint32_t Camera_Get_4F1000()
{
    return g_a70c90; // 0x004F1000: a1 90 0c a7 00 (mov eax,[0xA70C90]); c3
}

} // extern "C"
