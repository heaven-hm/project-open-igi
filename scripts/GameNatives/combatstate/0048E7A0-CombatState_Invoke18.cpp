// IGI native provenance: CombatState_Invoke18 - 0x0048E7A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E7A0-CombatState_Invoke18.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048E7A0-CombatState_Invoke18.hpp"
int CombatState_Invoke18(void* arg1, int* arg2, int* arg3)

{
    if (!FX_SmokeEmit_18(arg1, arg2, 0, 0))
        return 0;

    *arg3 = *arg2;
    return 1;
}
