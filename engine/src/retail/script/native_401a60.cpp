// Native: Script_GetCascadeWord_401A60 (placeholder until VERIFIED); retail VA 0x00401A60-0x00401A65 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 linear decode; derived in docs/fidelity/evidence/0x4EE970-data-extents.md section 12
//   (on s12.2). One of 0x4010B0's callees; 2 call sites in the image.
// Behavior/CFG: reads the dword global at 0x00567C58 and returns it in EAX. Zero arguments: the body touches no
//   [esp+N] or [ebp+N] operand at all.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED. Return type is dword by the emitted load width; the
//   caller-facing meaning of the value is NOT proven and the global's owner (its writer) is not yet transcribed.
#include <cstdint>

extern "C" {

// The retail reader at 0x00401A60 only loads this address; its writer is not transcribed, so this is a declaration,
// not storage. Do not define it here: only the retail writer may define a global.
extern std::uint32_t g_567c58;

std::uint32_t Script_GetCascadeWord_401A60()
{
    return g_567c58;   // 0x00401A60: mov eax, dword [0x567c58] ; 0x00401A65: ret
}

} // extern "C"
