// IGI native provenance: Camera_ApplyPostProcess_10 - 0x004A7290 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7290-Camera_ApplyPostProcess_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyPostProcess_10.h"
int* Camera_ApplyPostProcess_10(int context, int arg2, int arg3, int arg4, int* arg5)

{
    int item_c = arg3;
    int arg_c;
    int item_10 = arg_c;

    if (!arg4)
    {
        Camera_ResetMatrix_25(&arg_c, item_10);
        *arg5 = arg_c;
        return arg5;
    }

    Camera_UpdateViewCone_25(&item_c, item_10);
    *arg5 = item_c;
    arg5[1] = arg3;
    return arg5;
}
