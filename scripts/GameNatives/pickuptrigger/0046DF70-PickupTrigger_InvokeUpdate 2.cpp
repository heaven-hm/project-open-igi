// IGI native provenance: PickupTrigger_InvokeUpdate - 0x0046DF70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046DF70-PickupTrigger_InvokeUpdate.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PickupTrigger_InvokeUpdate.h"

int PickupTrigger_InvokeUpdate(int* arg1)

{
    int context_val;
    int var_4_1 = context_val;
    float var_4 = -0.261799395f;
    int result;

    for (int i = 0; i < 3;)
    {
        result = KnifeStab_Update(arg1, var_4);

        if (result)
            break;

        i += 1;
        var_4 = var_4 + 0.174532935f;
    }

    return result;
}
