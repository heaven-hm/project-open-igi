// IGI native provenance: FX_FireBallUpdate_20 - 0x0048E300 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048E300-FX_FireBallUpdate_20.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FireBallUpdate_20.h"
❓  int* __stdcall FX_FireBallUpdate_20(void* context , int* arg2, int arg3, int arg4, int arg5)
// This function has unresolved stack usage. View graph of stack usage to resolve.

{
    Camera_UpdateTransform_5(0x1404);
    int* i = arg2;
    int arg_404;
    int* flagP = &arg_404;
    return_addr = nullptr;

    if (i && arg4)
    {
        int temp_1 = *i;
        i = nullptr;
        int context_1 = 1;
        char arg_4[0x400];
        memset(&arg_4, 0, 0x400);
        arg_404 = temp_1;
        int arg_408 = 0xffffffff;
        arg_4[temp_1] = 1;

        if (temp_1 != 0xffffffff)
        {
            do
            {
                void* data_1 = Script_ExecuteLevelScript_23(*(context + 0x3b6c), *flagP);
                int data_2 = arg4(context, data_1, arg5);

                if (data_2)
                {
                    data_2 = 1;
                    return data_2;
                }

                int dest_2 = 0;
                int saved_target;
                int* data_3 = &(&saved_target)[context_1 + 0x105];
                int* source_2 = data_1 + 0x13c;

                while (return_addr < arg3)
                {
                    void* temp_4 = *source_2;

                    if (temp_4)
                    {
                        int entity_4 = *(temp_4 + 8);

                        if (*flagP == entity_4)
                            entity_4 = *(temp_4 + 0xc);

                        if (!arg_4[entity_4])
                        {
                            arg_4[entity_4] = 1;
                            *data_3 = entity_4;
                            context_1 += 1;
                            data_3 = &data_3[1];
                            return_addr += 1;
                            *data_3 = 0xffffffff;
                        }
                    }

                    dest_2 += 1;
                    source_2 = &source_2[1];

                    if (dest_2 >= 0x40)
                        break;
                }

                i = flagP[1];
                flagP = &flagP[1];
            } while (i != 0xffffffff);
        }
    }

    i = 0;
    return i;
}
