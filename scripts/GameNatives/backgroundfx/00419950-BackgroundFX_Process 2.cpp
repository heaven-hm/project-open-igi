// IGI native provenance: BackgroundFX_Process - 0x00419950 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00419950-BackgroundFX_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "BackgroundFX_Process.h"
int BackgroundFX_Process(int* context, float* arg2)

{
    int* data_val = *context;

    if (data_val)
        Menu_HandleInput_5(data_val, arg2);

    int result = context[1];
    int dest_val = 0;

    if (result > 0)
    {
        void* context_1 = nullptr;

        do
        {
            Menu_RequestScreen_6(context_1 + context[2], arg2);
            result = context[1];
            dest_val += 1;
            context_1 += 0xc;
        } while (dest_val < result);
    }

    int* source_1 = context[3];

    if (!source_1)
        return result;

    return Menu_DrawWidget_5(source_1, arg2);
}
