// IGI native provenance: Weapon_CycleFireMode_6 - 0x0047B270 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0047B270-Weapon_CycleFireMode_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047B270-Weapon_CycleFireMode_6.hpp"
int Weapon_CycleFireMode_6(int context, int arg2)

{
    memcpy(context + 0x78, arg2, 0x18);
    return context;
}
