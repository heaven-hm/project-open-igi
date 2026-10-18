// IGI native provenance: Camera_ResetMatrix_24 - 0x004AC825 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC825-Camera_ResetMatrix_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ResetMatrix_24.h"
int Camera_ResetMatrix_24(int context, int* arg2)

{
    int entity_val;
    int item_8 = entity_val;
    int item_c = entity_val;
    void* dest_val = arg2 - 1;
    int result = 0;
    int f0 = (dest_val + 1) / 0x20;
    int data_5;
    int temp_1;
    temp_1 = HIGHD(dest_val + 1);
    data_5 = LOWD(dest_val + 1);
    int* data_7 = context + (f0 << 2);

    if (*data_7 & 1 << ((0x1f - COMBINE(temp_1, data_5) % 0x20) & 0x1f) && !Camera_ApplyPostProcess_24(context, dest_val + 2))
        result = Camera_UpdateViewCone_24(context, dest_val);

    *data_7 &= 0xffffffff << ((0x1f - COMBINE(temp_1, data_5) % 0x20) & 0x1f);

    if (f0 + 1 < 3)
        memset(context + ((f0 + 1) << 2), 0, (3 - (f0 + 1)) << 2);

    return result;
}
