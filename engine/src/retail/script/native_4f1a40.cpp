// Native: Script_ParameterTypeFree (placeholder until VERIFIED); retail VA 0x004F1A40-0x004F1A55 (21 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 listing from docs/fidelity/evidence/0x004F19C0-listing.txt (8b442404 8d0cc0 8d1448 c704952859a7.. 00 c3 90): arg0*19*4 offset from 0xA758E0 with the used dword at +0x48.
// Behavior/CFG: clears the used dword of the descriptor slot selected by arg0; single store, no branches.
// Status/limits: STRUCTURALLY_VERIFIED (static only); same slot layout as 0x4F19C0 and 0x4F1BC0; storage owned by 0x4F1840; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint32_t g_a758e0[0x4C0]; // 0x00A758E0, owned by 0x4F1840

void Script_ParameterTypeFree(std::int32_t const index)
{
    g_a758e0[index * 19 + 18] = 0; // 0x004F1A4A: mov dword [edx*4+0xa75928],0
}

} // extern "C"
