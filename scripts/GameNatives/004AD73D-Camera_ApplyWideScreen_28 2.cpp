// IGI native provenance: Camera_ApplyWideScreen_28 - 0x004AD73D - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD73D-Camera_ApplyWideScreen_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_ApplyWideScreen_28.h"
unsigned int __fastcall Camera_ApplyWideScreen_28(int context)

{
    int item_8 = context;
    unsigned int arg_4;

    if (!g_9361e4)
    {
        unsigned int data_1 = arg_4;

        if (data_1 >= 0x61 && data_1 <= 0x7a)
            return data_1 - 0x20;

        return data_1;
    }

    unsigned int context_1 = arg_4;
    BOOL data_2;

    if (context_1 < 0x100)
    {
        if (g_54427c <= 1)
        {
            wchar16 (* data_3)[0x21] = g_544070;
            data_3 = (*data_3)[context_1];
            data_2 = data_3 & 2;
        }
        else
        {
            unsigned int item_14_1 = context_1;
            int data_val;
            int entry_data;
            data_2 = Camera_ApplyBandFilter_12(data_val, entry_data, context, 2);
        }
    }

    if (context_1 >= 0x100 || data_2)
    {
        char data_5 = context_1 >> 8;
        void* item_10_1;

        if (!(*(&(**&g_544070)[data_5] + 1) & 0x80))
        {
            *(&arg_4 + 1) = 0;
            arg_4 = context_1;
            item_10_1 = 1;
        }
        else
        {
            *(&arg_4 + 2) = 0;
            arg_4 = data_5;
            *(&arg_4 + 1) = context_1;
            item_10_1 = 2;
        }

        int data_7 = Camera_UpdateViewCone_27(g_9361e4, 0x200, &arg_4, item_10_1, &item_8, 3, 0, 1);

        if (data_7)
        {
            if (data_7 != 1)
                return *(&item_8 + 1) << 8 | item_8;

            return item_8;
        }
    }

    return context_1;
}
