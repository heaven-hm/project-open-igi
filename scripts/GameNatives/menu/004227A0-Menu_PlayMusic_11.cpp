// IGI native provenance: Menu_PlayMusic_11 - 0x004227A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004227A0-Menu_PlayMusic_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_11.hpp"
int Menu_PlayMusic_11(void* context, int arg2, int arg3, int arg4, float arg5, int arg6, int arg7, int arg8)

{
    int dest_val = 0;
    int flagP = 0;
    int item_4 = 0;
    int result = 0;
    int result_4;

    if (*(context + 0x94))
    {
        dest_val = Sys_CloseFileStream_17(*(context + (arg5 << 2) + 0x94));
        int result_1;
        result_1 = Sys_ExecuteBytecode_17(*(context + (arg5 << 2) + 0x94));
        result = result_1;
    }

    if (*(context + 0x98))
    {
        flagP = Sys_CloseFileStream_17(*(context + (arg6 << 2) + 0x94));
        result_4 = result;

        if (result_4 <= Sys_ExecuteBytecode_17(*(context + (arg6 << 2) + 0x94)))
        {
            int result_2;
            result_2 = Sys_ExecuteBytecode_17(*(context + (arg6 << 2) + 0x94));
            result = result_2;
        }
    }

    int context_3 = arg7;

    if (*(context + 0x9c))
    {
        item_4 = Sys_CloseFileStream_17(*(context + (context_3 << 2) + 0x94));
        result_4 = result;

        if (result_4 <= Sys_ExecuteBytecode_17(*(context + (context_3 << 2) + 0x94)))
        {
            int result_3;
            result_3 = Sys_ExecuteBytecode_17(*(context + (context_3 << 2) + 0x94));
            result = result_3;
        }
    }

    if (arg8)
        arg3 -= result;

    int data_16;

    if (!dest_val)
        data_16 = arg4;
    else
    {
        int result_8 = result_4;
        int result_5 = result_4;
        result_4 = Sys_ResolveSymbol_14(*(context + (arg5 << 2) + 0x94), arg2, arg3, 128.0f, 128.0f, 128.0f, 0, 0, 0);
        arg2 += dest_val;
        data_16 = arg4 - dest_val;
    }

    if (!flagP)
    {
        result_4 = arg2 + data_16 - item_4;
        arg2 = result_4;
    }
    else
    {
        int i_2 = (data_16 - item_4) / flagP;

        if (i_2 > 0)
        {
            int i_1 = i_2;
            int i;

            do
            {
                int result_6 = result_4;
                Sys_ResolveSymbol_14(*(context + (arg6 << 2) + 0x94), arg2, arg3, 128.0f, 128.0f, 128.0f, 0, 0, 0);
                result_4 = arg2 + flagP;
                i = i_1;
                i_1 -= 1;
                arg2 = result_4;
            } while (i != 1);
            context_3 = arg7;
        }
    }

    if (item_4)
    {
        int result_9 = result_4;
        int result_7 = result_4;
        Sys_ResolveSymbol_14(*(context + (context_3 << 2) + 0x94), arg2, arg3, 128.0f, 128.0f, 128.0f, 0, 0, 0);
    }

    return result;
}
