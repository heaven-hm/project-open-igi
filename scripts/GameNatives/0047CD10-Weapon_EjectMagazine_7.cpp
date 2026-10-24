// IGI native provenance: Weapon_EjectMagazine_7 - 0x0047CD10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047CD10-Weapon_EjectMagazine_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047CD10-Weapon_EjectMagazine_7.hpp"
void* Weapon_EjectMagazine_7(int context) {
    return context * 0x168 + &g_5be3e0;
}
