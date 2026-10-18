// IGI native provenance: Script_CallNativeHandler_12 - 0x004F2140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F2140-Script_CallNativeHandler_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_CallNativeHandler_12.h"
int** Script_CallNativeHandler_12(int context , char* arg2)

{
    int** data_val = Table_GetAt(context, arg2);
    int** dest_val = data_val;
    int** arg_4;

    if (dest_val)
    {
        int* source_val = *dest_val;
        int** flagP1 = arg_4;

        if (!source_val || !*source_val || !source_val)
        {
            data_val = flagP1[2];
            int* source_1 = *data_val;
            int source_2 = -(source_1);
            source_val = (source_2 - source_2) & data_val;
        }

        while (source_val != dest_val)
        {
            int data_4 = 0;
            int entity_2 = 0;
            data_4 = g_a76c03;
            arg_4 = nullptr;
            entity_2 = source_val[7];
            data_val = *(((data_4 * 0x180 + entity_2) << 2) + &g_a96ae0);

            if (data_val)
            {
                data_val(source_val, &arg_4);
                data_val = arg_4;

                if (data_val && data_val[1] == arg2 && !data_val[2])
                {
                    void* data_9 = Script_ExecuteLevelScript_12(context, arg2, source_val);
                    data_9 = 1;
                    return data_9;
                }
            }

            source_val = *source_val;

            if (!source_val || !*source_val || !source_val)
            {
                data_val = flagP1[2];
                int* source_4 = *data_val;
                int source_5 = -(source_4);
                source_val = (source_5 - source_5) & data_val;
            }
        }
    }
    else
    {
        int* i = arg_4[2];

        if (*i && i)
        {
            do
            {
                int data_1 = 0;
                arg_4 = nullptr;
                data_1 = g_a76c03;
                int data_2 = 0;
                data_2 = i[7];
                data_val = *(((data_1 * 0x180 + data_2) << 2) + &g_a96ae0);

                if (data_val)
                {
                    data_val(i, &arg_4);
                    data_val = arg_4;

                    if (data_val && data_val[1] == arg2 && !data_val[2])
                    {
                        void* data_3 = Script_ExecuteLevelScript_12(context, arg2, i);
                        data_3 = 1;
                        return data_3;
                    }
                }

                i = *i;

                if (!i)
                    break;

                if (!*i)
                    break;
            } while (i);
        }
    }
    data_val = 0;
    return data_val;
}
