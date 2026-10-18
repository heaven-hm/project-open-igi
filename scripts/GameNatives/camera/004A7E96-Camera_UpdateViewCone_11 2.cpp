// IGI native provenance: Camera_UpdateViewCone_11 - 0x004A7E96 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7E96-Camera_UpdateViewCone_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateViewCone_11.h"
int* Camera_UpdateViewCone_11(int context, int* arg2, int* arg3, int* arg4)

{
    int* i = arg2;
    int* i_1 = arg2 - 1;

    while (i > 0)
    {
        i = Camera_ApplyPostProcess_11(context, arg3, arg4);

        if (*arg4 == 0xffffffff)
            break;

        i = i_1;
        i_1 -= 1;
    }

    return i;
}
