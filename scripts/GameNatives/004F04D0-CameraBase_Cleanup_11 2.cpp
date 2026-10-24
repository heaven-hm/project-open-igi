// IGI native provenance: CameraBase_Cleanup_11 - 0x004F04D0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F04D0-CameraBase_Cleanup_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "CameraBase_Cleanup_11.h"
int CameraBase_Cleanup_11(int context, int* arg2)

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

    int result = ~i;
    int source_1;
    int dest_3;
    dest_3 = memcpy(*arg2, dest_val - result, result & 0xfffffffc);
    memcpy(dest_3, source_1, result & 3);
    return result;
}
