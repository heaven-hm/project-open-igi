// IGI native provenance: Sys_ReadFileBlock_17 - 0x004B66A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B66A0-Sys_ReadFileBlock_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ReadFileBlock_17.h"
int Sys_ReadFileBlock_17(int* context)

{
    int* data_val = *context;

    if (data_val)
    {
        Texture_UploadPalettePacket(data_val);
        Sys_ExecuteBytecode_2(*context);
        *context = 0;
    }

    void* data_2 = context[3];

    if (data_2)
        Sys_PushStack_20(data_2);

    int data_3 = context[4];

    if (data_3)
    {
        int i = 0;

        if (data_3 > 0)
        {
            int context_1 = 0;

            do
            {
                Struct_Clear36(context[1] + context_1);
                i += 1;
                context_1 += 0x24;
            } while (i < context[4]);
        }
    }

    int result = Sys_ExecuteBytecode_2(context[2]);
    context[2] = 0;
    return result;
}
