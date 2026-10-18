// IGI native provenance: FX_FindfreepositionStac - 0x0048B050 - CFG score 96.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048B050-FX_FindfreepositionStac.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "FX_FindfreepositionStac.h"
❓  void* __stdcall FX_FindfreepositionStac(void** context , int* arg2, char arg3, void* arg4)
// This function has unresolved stack usage. View graph of stack usage to resolve.

{
    Camera_UpdateTransform_5(0x142c);
    int* dest_val = context;
    int arg_42c;
    int* context_val = &arg_42c;
    return_addr = nullptr;
    *dest_val = 0;
    int* arg_10 = &arg_42c;

    if (*(arg2[0xedb] + 0x5c) >= 0x400)
        return Warning_Show("FindFreePosition stack not large enough");

    int* data_2 = AI_SelectTarget_4(arg2);

    if (!data_2)
        return Warning_Show("No nodes near target");

    void arg_14;
    AI_UpdatePatrol_3(&arg_14, arg2);
    char item_1c = 1;
    g_5c8bb4 = *data_2;
    g_5c8bac = arg2[0xf8b];
    void* data_5 = FX_SmokeEmit_18(arg2, data_2, arg3, item_1c);

    if (data_5)
    {
        *dest_val = data_2;
        return data_5;
    }

    int source_1 = *data_2;
    void* data_6 = nullptr;
    char arg_2c[0x400];
    memset(&arg_2c, 0, 0x400);
    arg_42c = source_1;
    int arg_4 = 1;
    int arg_430 = 0xffffffff;
    arg_2c[source_1] = 1;

    if (source_1 != 0xffffffff)
    {
    label_48b151:
        data_6 = arg4;

        if (return_addr < data_6)
        {
            int* data_9 = Script_ExecuteLevelScript_23(arg2[0xedb], *context_val) + 0x13c;
            int arg_c = 0;
            int saved_target;
            int* dest_1 = &(&saved_target)[arg_4 + 0x10f];
            int* arg_8 = data_9;

            while (true)
            {
                if (return_addr < arg4)
                {
                    void* data_10 = *data_9;

                    if (data_10)
                    {
                        int source_2 = *(data_10 + 8);

                        if (*context_val == source_2)
                            source_2 = *(data_10 + 0xc);

                        if (!arg_2c[source_2])
                        {
                            int* data_12 = Script_ExecuteLevelScript_23(arg2[0xedb], source_2);
                            data_6 = FX_SmokeEmit_18(arg2, data_12, arg3, 1);

                            if (data_6)
                            {
                                *context = data_12;
                                break;
                            }

                            context_val = arg_10;
                            *dest_1 = source_2;
                            dest_1 = &dest_1[1];
                            arg_2c[source_2] = 1;
                            arg_4 += 1;
                            *dest_1 = 0xffffffff;
                            return_addr += 1;
                        }
                    }

                    data_9 = &arg_8[1];
                    bool flag_2_1 = arg_c + 1 < 0x40;
                    arg_c += 1;
                    arg_8 = data_9;

                    if (flag_2_1)
                        continue;
                }

                int data_17 = context_val[1];
                context_val = &context_val[1];
                arg_10 = context_val;

                if (data_17 != 0xffffffff)
                    goto label_48b151;

                return data_17;
            }
        }
    }

    return data_6;
}
