// IGI native provenance: Camera_SetFieldOfView_19 - 0x004A9FC1 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A9FC1-Camera_SetFieldOfView_19.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A9FC1-Camera_SetFieldOfView_19.hpp"
unsigned int __fastcall Camera_SetFieldOfView_19(int context)

{
    int item_8 = context;
    unsigned int arg_4;

    if (!g_9361e4)
    {
        unsigned int data_1 = arg_4;

        if (data_1 >= 0x41 && data_1 <= 0x5a)
            return data_1 + 0x20;

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
            data_2 = data_3 & 1;
        }
        else
        {
            unsigned int item_1c_1 = context_1;
            int data_val;
            int entry_data;
            data_2 = Camera_ApplyBandFilter_12(data_val, entry_data, context, 1);
        }
    }

    if (context_1 >= 0x100 || data_2)
    {
        char data_5 = context_1 >> 8;
        void* data_6;

        if (!(*(&(**&g_544070)[data_5] + 1) & 0x80))
        {
            *(&arg_4 + 1) = 0;
            arg_4 = context_1;
            data_6 = 1;
        }
        else
        {
            *(&arg_4 + 2) = 0;
            arg_4 = data_5;
            *(&arg_4 + 1) = context_1;
            data_6 = 2;
        }

        int data_7 = Camera_UpdateViewCone_27(g_9361e4, 0x100, &arg_4, data_6, &item_8, 3, 0, 1);

        if (data_7)
        {
            if (data_7 != 1)
                return *(&item_8 + 1) << 8 | item_8;

            return item_8;
        }
    }

    return context_1;
}
