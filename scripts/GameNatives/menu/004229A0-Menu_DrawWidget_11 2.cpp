// IGI native provenance: Menu_DrawWidget_11 - 0x004229A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004229A0-Menu_DrawWidget_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_DrawWidget_11.h"
int Menu_DrawWidget_11(int context, int arg2, int arg3, int arg4, int arg5, int arg6)

{
    int result = *(context + (arg5 << 2) + 0x94);

    if (result)
    {
        int data_val;
        int entity_1;
        data_val = Sys_ExecuteBytecode_17(result);
        int i_1 = arg4 / data_val;
        result = arg6;

        if (result)
        {
            result = Sys_CloseFileStream_17(*(context + (arg5 << 2) + 0x94));
            entity_1 = arg2 - result;
            arg2 = entity_1;
        }

        if (i_1 > 0)
        {
            int i;

            do
            {
                int item_2c_1 = entity_1;
                result = Sys_ResolveSymbol_14(*(context + (arg5 << 2) + 0x94), arg2, arg3, 128.0f, 128.0f, 128.0f, 0,
                    0, 0xffffffff);
                entity_1 = arg3 + data_val;
                i = i_1;
                i_1 -= 1;
                arg3 = entity_1;
            } while (i != 1);
        }
    }

    return result;
}
