// IGI native provenance: Camera_ApplyBandFilter_12 - 0x004A7F2A - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7F2A-Camera_ApplyBandFilter_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A7F2A-Camera_ApplyBandFilter_12.hpp"
BOOL Camera_ApplyBandFilter_12(int context, int arg2, int arg3, int arg4 )

{
    int item_8 = arg3;
    int arg_4;
    int data_val = arg_4;
    unsigned int data_1;

    if (data_val + 1 > 0x100)
    {
        char entity_3 = data_val >> 8;
        int saved_source_1;

        if (!(*(&(**&g_544070)[entity_3] + 1) & 0x80))
        {
            *(&item_8 + 1) = 0;
            item_8 = data_val;
            saved_source_1 = 1;
        }
        else
        {
            *(&item_8 + 2) = 0;
            item_8 = entity_3;
            *(&item_8 + 1) = data_val;
            saved_source_1 = 2;
        }

        BOOL result = Camera_SetFieldOfView_27(1, &item_8, saved_source_1, (&arg_4 + 2), 0, 0, 1);

        if (!result)
            return result;

        data_1 = *(&arg_4 + 2);
    }
    else
        data_1 = (**&g_544070)[data_val];

    return data_1 & arg4;
}
