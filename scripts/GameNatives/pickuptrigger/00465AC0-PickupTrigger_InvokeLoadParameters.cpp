// IGI native provenance: PickupTrigger_InvokeLoadParameters - 0x00465AC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00465AC0-PickupTrigger_InvokeLoadParameters.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PickupTrigger_InvokeLoadParameters.hpp"

int PickupTrigger_InvokeLoadParameters(int* arg1, int arg2)

{
    int var_4 = arg2;
    arg1[0x4e] = 0;
    return EditRigidObjAttachedToBone_LoadParameters(arg1);
}
