// IGI native provenance: Weapon_FireBullet_4 - 0x00475310 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00475310-Weapon_FireBullet_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00475310-Weapon_FireBullet_4.hpp"
int Weapon_FireBullet_4(int context, int arg2, int arg3)

{
    memcpy(arg3, context + 0x188, 0x28);
    memcpy(arg2, context + 0x170, 0x18);
    return context;
}
