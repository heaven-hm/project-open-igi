// IGI native provenance: Camera_UpdateViewCone_24 - 0x004AC7CF - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC7CF-Camera_UpdateViewCone_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_UpdateViewCone_24.h"
int Camera_UpdateViewCone_24(int context, int arg2)

{
    int f0 = arg2 / 0x20;
    int* dest_val = context + (f0 << 2);
    int i = Camera_ResetMatrix_28(*dest_val, 1 << ((0x1f - arg2 % 0x20) & 0x1f), dest_val);
    int source_1 = f0 - 1;

    if (f0 - 1 >= 0)
    {
        int* dest_1 = context + (source_1 << 2);

        while (i)
        {
            i = Camera_ResetMatrix_28(*dest_1, 1, dest_1);
            source_1 -= 1;
            dest_1 -= 4;

            if (source_1 < 0)
                break;
        }
    }

    return i;
}
