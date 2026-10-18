// IGI native provenance: Mem_GrowCustomPool_2 - 0x004ABA99 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004ABA99-Mem_GrowCustomPool_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Mem_GrowCustomPool_2.h"
int Mem_GrowCustomPool_2(int* context, int arg2, int arg3)

{
    int entity_val;
    int item_8 = entity_val;
    void* source_val = context[1];
    char* dest_val = *context;
    char* item_8_1 = dest_val;
    char* data_val = dest_val;
    void* data_6;

    if (source_val < arg3)
    {
        char* source_1 = source_val + dest_val;

        if (*source_1)
            data_val = source_1;

        if (&data_val[arg3] >= &context[0x3e])
        {
        label_4abb31:
            char* source_6 = &context[2];
            int data_5;

            while (true)
            {
                if (source_6 >= dest_val)
                    return 0;

                if (&source_6[arg3] >= &context[0x3e])
                    return 0;

                char data_4 = *source_6;

                if (data_4)
                    source_6 = &source_6[data_4];
                else
                {
                    char* context_3 = &source_6[1];
                    data_5 = 1;

                    while (!*context_3)
                    {
                        context_3 = &context_3[1];
                        data_5 += 1;
                    }

                    if (data_5 >= arg3)
                        break;

                    arg2 -= data_5;

                    if (arg2 < arg3)
                        return 0;

                    source_6 = context_3;
                }
            }

            void* context_5 = &source_6[arg3];

            if (context_5 >= &context[0x3e])
            {
                context[1] = 0;
                *context = &context[2];
            }
            else
            {
                *context = context_5;
                context[1] = data_5 - arg3;
            }

            *source_6 = arg3;
            data_6 = &source_6[8];
        }
        else
        {
            while (true)
            {
                char context_1 = *data_val;

                if (context_1)
                    data_val = &data_val[context_1];
                else
                {
                    char* context_2 = &data_val[1];
                    int source_3 = 1;

                    while (!*context_2)
                    {
                        context_2 = &context_2[1];
                        source_3 += 1;
                    }

                    if (source_3 >= arg3)
                    {
                        void* context_4 = &data_val[arg3];

                        if (context_4 >= &context[0x3e])
                        {
                            context[1] = 0;
                            *context = &context[2];
                        }
                        else
                        {
                            *context = context_4;
                            context[1] = source_3 - arg3;
                        }

                        *data_val = arg3;
                        data_6 = &data_val[8];
                        break;
                    }

                    if (data_val != item_8_1)
                    {
                        arg2 -= source_3;

                        if (arg2 < arg3)
                            return 0;
                    }
                    else
                        context[1] = source_3;

                    dest_val = item_8_1;
                    data_val = context_2;
                }

                if (&data_val[arg3] >= &context[0x3e])
                    goto label_4abb31;
            }
        }
    }
    else
    {
        *dest_val = arg3;

        if (&dest_val[arg3] >= &context[0x3e])
        {
            context[1] = 0;
            *context = &context[2];
        }
        else
        {
            *context += arg3;
            context[1] -= arg3;
        }

        data_6 = &dest_val[8];
    }

    return (data_6 << 4) - context * 0xf;
}
