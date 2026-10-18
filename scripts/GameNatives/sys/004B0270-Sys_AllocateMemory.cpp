// IGI native provenance: Sys_AllocateMemory - 0x004B0270 - CFG score 97.8% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B0270-Sys_AllocateMemory.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B0270-Sys_AllocateMemory.hpp"
int Sys_AllocateMemory(int* context, int* arg2, int arg3, int* arg4, int arg5, int arg6)

{
    int dest_val = arg3;
    int i = 0xffffffff;
    g_9422e0 = 0;
    g_9422e4 = 0;
    void* temp_val = nullptr;
    int source_val = 0;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    void* item_24;
    memset(&item_24, 0, 0x18);
    void* item_8 = nullptr;

    if (arg2)
        *arg2 = 0;

    if (context)
        *context = 0xffffffff;

    int* dest_1 = arg4;
    int item_1c;

    while (true)
    {
        int item_20;

        if (item_20 < 0x200)
        {
            if (source_val != 0xffffffff)
                goto label_4b034a;

            int item_18;
            int item_4;

            if (Sys_ReadFileBlock(0x93dae0, &item_20, &item_4, &item_24, &item_1c, &item_18))
            {
                int item_38_2 = item_20;
                void* item_3c_2 = item_24;
                int data_4 = Sys_CloseFileStream(dest_1, item_4, 0);
                int item_10_1 = item_1c;
                temp_val = item_24;
                source_val = data_4;
                int item_14_1 = 0;
            label_4b034a:
                int* entity_3 = &dest_1[source_val * 5];
                arg4 = entity_3;
                int data_6 = entity_3[2];

                if (data_6 == 0x100)
                    break;

                if (data_6 != 0x800)
                {
                    int item_40_2;

                    if (data_6 != 0x2000)
                    {
                        if (data_6 == 0x4000)
                        {
                            if (temp_val < ~i - 1)
                            {
                            label_4b0506:
                                int item_38_12 = item_20;
                                goto label_4b0507;
                            }

                            int item_38_6 = item_20;
                            void* item_3c_5 = temp_val;
                            item_40_2 = 1;
                            goto label_4b050b;
                        }

                        if (*(&data_6 + 1) & 0x80)
                        {
                            item_14_1 = 0;
                            *((entity_3[1] << 2) + &g_93d2e0) = 0;

                            if (entity_3[2] & 0x20000)
                            {
                                Sys_OpenFileStream(0x93dae0, &item_20, source_val, temp_val, item_1c, item_18);
                                temp_val = item_24;
                            }

                            int item_38_8 = item_20;
                            void* item_3c_7 = temp_val;
                            int data_17 = Sys_CloseFileStream(dest_1, source_val, 1);
                            temp_val = item_24;
                            source_val = data_17;
                            item_18 += 1;
                            continue;
                        }
                        else if (data_6 & 0x10000)
                        {
                            int data_20 = *entity_3;
                            int flagP1 = entity_3[1];

                            if (data_20 >= 0)
                                *((data_20 << 2) + &g_93d2e0) += item_14_1;

                            int item_40_4;

                            if (item_14_1)
                            {
                            label_4b0489:
                                int item_38_10 = item_20;
                                void* item_3c_9 = temp_val;
                                item_40_4 = 1;
                            }
                            else
                            {
                                int data_21 = *((flagP1 << 2) + &g_93d2e0);
                                item_14_1 = data_21;

                                if (data_21)
                                    goto label_4b0489;

                                int item_38_9 = item_20;
                                void* item_3c_8 = temp_val;
                                item_40_4 = 0;
                            }

                            int data_23 = Sys_CloseFileStream(dest_1, source_val, item_40_4);
                            temp_val = item_24;
                            source_val = data_23;
                            item_18 -= 1;
                            continue;
                        }
                        else if (data_6 != 0x400)
                        {
                            if (temp_val < ~i - 1)
                            {
                                int flagP5 = *(temp_val + arg3);
                                int context_1;

                                if (*(&data_6 + 1) & 0x10)
                                {
                                    int data_33 = Sys_FreeMemory(flagP5, arg5, data_6 & 0xff);
                                    temp_val = item_24;
                                    entity_3 = arg4;
                                    context_1 = data_33;
                                }
                                else if (!arg6)
                                {
                                    context_1 = 0;
                                    context_1 = (data_6 & 0xff) == flagP5;
                                }
                                else
                                {
                                    int item_38_14 = data_6 & 0xff;
                                    unsigned int data_35;
                                    int entity_13;
                                    data_35 = Camera_ApplyWideScreen_28(entity_3);
                                    int item_3c_13 = flagP5;
                                    unsigned int data_36 = Camera_ApplyWideScreen_28(entity_13);
                                    temp_val = item_24;
                                    int entity_14 = 0;
                                    entity_14 = data_35 == data_36;
                                    context_1 = entity_14;
                                    entity_3 = arg4;
                                }

                                if (*(&entity_3[2] + 1) & 2 && context_1)
                                {
                                    Sys_OpenFileStream(0x93dae0, &item_20, source_val, temp_val, item_1c, item_18);
                                    temp_val = item_24;
                                    entity_3 = arg4;
                                }

                                if (entity_3[2] & 0x40000 && context_1)
                                {
                                    Sys_OpenFileStream(0x93dae0, &item_20, source_val, temp_val, item_1c, item_18);
                                    temp_val = item_24;
                                }

                                int item_38_17 = item_20;
                                void* item_3c_16 = temp_val;
                                source_val = Sys_CloseFileStream(dest_1, source_val, context_1);

                                if (!context_1)
                                {
                                    temp_val = item_24;
                                    continue;
                                }
                                else
                                {
                                    temp_val = item_24 + 1;
                                    item_14_1 += 1;
                                    item_24 = temp_val;
                                    continue;
                                }
                            }

                            goto label_4b0506;
                        }
                    }
                    else
                    {
                        if (temp_val)
                        {
                            int item_38_4 = item_20;
                        label_4b0507:
                            void* item_3c_11 = temp_val;
                            item_40_2 = 0;
                            goto label_4b050b;
                        }

                        int item_38_5 = item_20;
                        int item_3c_4 = 0;
                        item_40_2 = 1;
                    label_4b050b:
                        int data_31 = Sys_CloseFileStream(dest_1, source_val, item_40_2);
                        temp_val = item_24;
                        source_val = data_31;
                        continue;
                    }
                    int data_26 = item_1c;
                    *((data_26 << 3) + &g_93aa88) = temp_val;
                    *((data_26 << 3) + &g_93aa8c) = 0;
                    int item_38_11 = item_20;
                    void* item_3c_10 = temp_val;
                    source_val = Sys_CloseFileStream(dest_1, source_val, 1);
                    bool flag_4_1 = item_1c + 1 >= 0xa;
                    item_10_1 += 1;
                    item_1c += 1;

                    if (!flag_4_1)
                    {
                        temp_val = item_24;
                        continue;
                    }
                }
                else
                {
                    int item_38_3 = item_20;
                    void* item_3c_3 = temp_val;
                    source_val = Sys_CloseFileStream(dest_1, source_val, 1);
                    bool flag_2_1 = item_10_1 - 1 < 0;
                    item_10_1 -= 1;

                    if (!flag_2_1)
                    {
                        int data_10 = item_1c;
                        temp_val = item_24;
                        *((data_10 << 3) + &g_93aa84) = temp_val - *((data_10 << 3) + &g_93aa80) + 1;
                        continue;
                    }
                }
            }
        }

        return 0;
    }

    if (temp_val >= 0)
        item_8 = temp_val;

    if (arg2)
        *arg2 = item_1c;

    int temp_5 = item_1c;
    g_9422e0 = &g_93aa88;
    g_9422e4 = temp_5;

    if (context)
        *context = item_8;

    return 1;
}
