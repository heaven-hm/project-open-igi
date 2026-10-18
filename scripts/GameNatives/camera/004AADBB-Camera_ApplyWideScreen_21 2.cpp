// IGI native provenance: Mem_AllocateCustomPool_1 - 0x004AADBB - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004AADBB-Mem_AllocateCustomPool_1.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mem_AllocateCustomPool_1.h"
int* Mem_AllocateCustomPool_1(int* context, int* arg2)

{
    int* result = context[4];
    int* source_1 = &arg2[-1];
    unsigned int dest_2 = (arg2 - context[3]) >> 0xf;
    int* entity_5 = *source_1 - 1;
    int* item_8 = entity_5;

    if (!(entity_5 & 1))
    {
        int temp_1 = *(entity_5 + source_1);
        int* context_1 = entity_5 + source_1;
        int temp_2 = source_1[-1];
        arg2 = context_1;

        if (!(temp_1 & 1))
        {
            int temp_5 = (temp_1 >> 4) - 1;

            if (temp_5 > 0x3f)
                temp_5 = 0x3f;

            int* entity_13;

            if (context_1[1] != context_1[2])
                entity_13 = item_8;
            else
            {
                if (temp_5 >= 0x20)
                {
                    unsigned int context_5 = ~(0x80000000 >> ((temp_5 - 0x20) & 0x1f));
                    result[dest_2 + 0x31] &= context_5;
                    char f0_1 = *(temp_5 + result + 4);
                    *(temp_5 + result + 4) -= 1;

                    if (f0_1 == 1)
                        context[1] &= context_5;
                }
                else
                {
                    unsigned int context_3 = ~(0x80000000 >> (temp_5 & 0x1f));
                    result[dest_2 + 0x11] &= context_3;
                    char f1_1 = *(temp_5 + result + 4);
                    *(temp_5 + result + 4) -= 1;

                    if (f1_1 == 1)
                        *context &= context_3;
                }

                entity_13 = item_8;
                context_1 = arg2;
            }

            entity_5 = entity_13 + temp_1;
            *(context_1[2] + 4) = context_1[1];
            item_8 = entity_5;
            *(arg2[1] + 8) = arg2[2];
        }

        int temp_11 = (entity_5 >> 4) - 1;

        if (temp_11 > 0x3f)
            temp_11 = 0x3f;

        int context_9 = temp_2 & 1;
        int* context_12;

        if (context_9)
            context_12 = context;
        else
        {
            arg2 = source_1 - temp_2;
            context_12 = (temp_2 >> 4) - 1;

            if (context_12 > 0x3f)
                context_12 = 0x3f;

            int* entity_14 = entity_5 + temp_2;
            item_8 = entity_14;
            temp_11 = (entity_14 >> 4) - 1;

            if (temp_11 > 0x3f)
                temp_11 = 0x3f;

            if (context_12 != temp_11)
            {
                if (arg2[1] == arg2[2])
                {
                    if (context_12 >= 0x20)
                    {
                        unsigned int source_7 = ~(0x80000000 >> ((context_12 - 0x20) & 0x1f));
                        result[dest_2 + 0x31] &= source_7;
                        char f3_1 = *(context_12 + result + 4);
                        *(context_12 + result + 4) -= 1;

                        if (f3_1 == 1)
                            context[1] &= source_7;
                    }
                    else
                    {
                        unsigned int source_5 = ~(0x80000000 >> (context_12 & 0x1f));
                        result[dest_2 + 0x11] &= source_5;
                        char f4_1 = *(context_12 + result + 4);
                        *(context_12 + result + 4) -= 1;

                        if (f4_1 == 1)
                            *context &= source_5;
                    }
                }

                *(arg2[2] + 4) = arg2[1];
                *(arg2[1] + 8) = arg2[2];
            }

            source_1 = arg2;
        }

        if (context_9 || context_12 != temp_11)
        {
            void* entity_25 = &result[dest_2 * 0x81 + 0x51 + temp_11 * 2];
            source_1[1] = result[dest_2 * 0x81 + 0x51 + temp_11 * 2 + 1];
            source_1[2] = entity_25;
            *(entity_25 + 4) = source_1;
            *(source_1[1] + 8) = source_1;

            if (source_1[1] == source_1[2])
            {
                char entity_28 = *(temp_11 + result + 4);
                *(&arg2 + 3) = entity_28;
                *(temp_11 + result + 4) = entity_28 + 1;

                if (temp_11 >= 0x20)
                {
                    if (!*(&arg2 + 3))
                        context[1] |= 0x80000000 >> ((temp_11 - 0x20) & 0x1f);

                    result[dest_2 + 0x31] |= 0x80000000 >> ((temp_11 - 0x20) & 0x1f);
                }
                else
                {
                    if (!*(&arg2 + 3))
                        *context |= 0x80000000 >> (temp_11 & 0x1f);

                    result[dest_2 + 0x11] |= 0x80000000 >> (temp_11 & 0x1f);
                }
            }
        }

        *source_1 = item_8;
        *(item_8 + source_1 - 4) = item_8;
        result = &result[dest_2 * 0x81 + 0x51];
        int f2_1 = *result;
        *result -= 1;

        if (f2_1 == 1)
        {
            int data_3 = g_c321f0;

            if (data_3)
            {
                VirtualFree((g_c321e8 << 0xf) + *(data_3 + 0xc), 0x8000, MEM_DECOMMIT);
                void* data_4 = g_c321f0;
                *(data_4 + 8) |= 0x80000000 >> (g_c321e8 & 0x1f);
                *(*(g_c321f0 + 0x10) + (g_c321e8 << 2) + 0xc4) = 0;
                void* data_8 = *(g_c321f0 + 0x10);
                *(data_8 + 0x43) -= 1;
                void* data_9 = g_c321f0;

                if (!*(*(data_9 + 0x10) + 0x43))
                {
                    *(data_9 + 4) &= 0xfffffffe;
                    data_9 = g_c321f0;
                }

                if (*(data_9 + 8) == 0xffffffff)
                {
                    VirtualFree(*(data_9 + 0xc), 0, MEM_RELEASE);
                    HeapFree(g_c32428, HEAP_NONE, *(g_c321f0 + 0x10));
                    char* data_14 = g_c321f0;
                    Camera_ApplyHandiCam_26(data_14, &data_14[0x14],
                        g_c321f4 * 0x14 - data_14 + g_c321f8 - 0x14);
                    g_c321f4 -= 1;

                    if (context > g_c321f0)
                        context -= 0x14;

                    g_c321ec = g_c321f8;
                }
            }

            result = context;
            g_c321e8 = dest_2;
            g_c321f0 = result;
        }
    }

    return result;
}
