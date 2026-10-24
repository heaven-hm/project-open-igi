// Native: Script_ParameterPropertySet_4F1BC0 (placeholder until VERIFIED); retail VA 0x004F1BC0-0x004F1C15 (86 bytes, 6 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (docs/fidelity/evidence/0x004F1BC0-listing.txt): 53 8b5c240c 55 8b6c240c 0fbed3 8d44ed00 56 57 bee458a700 8d4c4500 8b44241c 03ca 33ff 89048de858.. / loop 8b4644 85c0 7413 392e 750f 8b4c241c 51 53 57 e8beffffff 83c40c / 83c64c 47 81fee46ba700 7cda / 5f 5e 5d 5b c3.
// Behavior/CFG: writes value into the property dword (slot*19 + 2 + sign-extended arg1) of the 19-dword descriptor table at 0xA758E0, then walks the 64 slots and for every ACTIVE slot (used dword 18 != 0) whose parent dword 1 equals arg0 it recurses with that slot as the new base, so the value reaches the whole descriptor subtree.
// Status/limits: TRANSCRIBED (static only); the +0x44 access is dword 18 (the used flag proven by 0x4F19C0/0x4F1A40) and [esi] is dword 1 (parent link), which is why the walk starts at 0xA758E4 = base+4; recursion depth is bounded by the parent chain; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

extern std::uint32_t g_a758e0[0x4C0]; // 0x00A758E0, owned by 0x4F1840

void Script_ParameterPropertySet_4F1BC0(std::int32_t const base, std::int8_t const sub, std::uint32_t const value)
{
    g_a758e0[base * 19 + 2 + static_cast<std::int32_t>(sub)] = value;      // 0x004F1BE4: mov [ecx*4+0xa758e8],eax
    for (std::int32_t slot = 0; slot < 64; slot += 1) {                     // 0x004F1BEB..0x004F1C0F: [esi] from 0xA758E4 to 0xA76BE4, stride 0x4C -> 64 slots
        if (g_a758e0[slot * 19 + 18] != 0 &&                                // 0x004F1BEB: mov eax,[esi+0x44]
            g_a758e0[slot * 19 + 1] == static_cast<std::uint32_t>(base)) {  // 0x004F1BF2: cmp [esi],ebp
            Script_ParameterPropertySet_4F1BC0(slot, sub, value);           // 0x004F1BFD
        }
    }
}

} // extern "C"
