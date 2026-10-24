// IGI native provenance: CameraBase_Cleanup_7 - 0x004F0380 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0380-CameraBase_Cleanup_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Cleanup_7.h"
int* CameraBase_Cleanup_7(int context, int* arg2)

{
    int dest_val = context;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    *arg2 = ~i;
    return arg2;
}
