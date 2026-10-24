// IGI native provenance: Weapon_UpdateBarrel - 0x00470F70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00470F70-Weapon_UpdateBarrel.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Weapon_UpdateBarrel.h"
void* Weapon_UpdateBarrel(void* context, float* arg2, float* arg3, float arg4)

{
    long double fpu7 = 1.0 / arg4;
    *arg2 = *(context + 0x48) * *arg3 * fpu7;
    arg2[1] = *(context + 0x4c) * arg3[1] * fpu7;
    return context;
}
