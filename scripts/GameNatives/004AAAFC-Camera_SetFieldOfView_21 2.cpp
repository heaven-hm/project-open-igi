// IGI native provenance: Camera_SetFieldOfView_21 - 0x004AAAFC - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AAAFC-Camera_SetFieldOfView_21.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_SetFieldOfView_21.h"
int Camera_SetFieldOfView_21(char* context, void** arg2, int arg3, int arg4)

{
    int result = 0;
    char context_val = *context;
    char* source_val = &context[1];
    char* item_8 = source_val;

    while (true)
    {
        BOOL data_2;
        wchar16 (* entity_val)[0x21];

        if (g_54427c <= 1)
        {
            entity_val = g_544070;
            unsigned int data_3 = context_val;
            data_3 = (*entity_val)[data_3];
            data_2 = data_3 & 8;
        }
        else
        {
            unsigned int data_1 = context_val;
            unsigned int item_24_1 = data_1;
            int temp_val;
            data_2 = Camera_ApplyBandFilter_12(data_1, temp_val, entity_val, 8);
            entity_val = 8;
        }

        if (!data_2)
            break;

        context_val = *source_val;
        source_val = &source_val[1];
    }

    char* item_8_1 = source_val;

    if (context_val == 0x2d)
    {
        arg4 |= 2;
    label_4aab57:
        context_val = *source_val;
        source_val = &source_val[1];
        item_8_1 = source_val;
    }
    else if (context_val == 0x2b)
        goto label_4aab57;

    if (arg3 < 0 || arg3 == 1 || arg3 > 0x24)
    {
        char** data_20 = arg2;

        if (data_20)
            *data_20 = context;

        return 0;
    }

    int entity_1 = 0x10;

    if (arg3)
        goto label_4aaba5;

    if (context_val == 0x30)
    {
        char data_5 = *source_val;

        if (data_5 == 0x78 || data_5 == 0x58)
        {
            arg3 = 0x10;
        label_4aaba5:

            if (arg3 == 0x10 && context_val == 0x30)
            {
                data_5 = *source_val;

                if (data_5 == 0x78 || data_5 == 0x58)
                {
                    context_val = source_val[1];
                    item_8_1 = &source_val[2];
                }
            }
        }
        else
            arg3 = 8;
    }
    else
        arg3 = 0xa;

    int data_6 = 0xffffffff;
    int temp_1 = 0;
    char* data_7 = COMBINE(temp_1, data_6) / arg3;
    unsigned int temp_2 = COMBINE(temp_1, data_6) % arg3;
    char* item_10_1 = data_7;

    while (true)
    {
        unsigned int source_3 = context_val;
        BOOL data_8;

        if (g_54427c <= 1)
        {
            wchar16 (* data_9)[0x21] = g_544070;
            data_9 = (*data_9)[source_3];
            data_8 = data_9 & 4;
        }
        else
        {
            unsigned int item_24_2 = source_3;
            data_8 = Camera_ApplyBandFilter_12(data_7, temp_2, entity_1, 4);
            entity_1 = 4;
        }

        if (!data_8)
        {
            BOOL data_10;

            if (g_54427c <= 1)
            {
                wchar16 (* data_11)[0x21] = g_544070;
                data_11 = (*data_11)[source_3];
                data_10 = data_11 & 0x103;
            }
            else
            {
                unsigned int item_24_3 = source_3;
                data_10 = Camera_ApplyBandFilter_12(data_8, temp_2, entity_1, 0x103);
                entity_1 = 0x103;
            }

            if (!data_10)
                break;

            int item_20_1 = context_val;
            unsigned int data_13;
            data_13 = Camera_ApplyWideScreen_28(entity_1);
            entity_1 = data_13 - 0x37;
        }
        else
            entity_1 = context_val - 0x30;

        if (entity_1 >= arg3)
            break;

        arg4 |= 8;

        if (result >= item_10_1 && result == item_10_1)
            temp_2 = COMBINE(0, 0xffffffff) % arg3;

        if (result < item_10_1 || (result == item_10_1 && entity_1 <= temp_2))
            result = result * arg3 + entity_1;
        else
            arg4 |= 4;

        data_7 = item_8_1;
        item_8_1 = &item_8_1[1];
        context_val = *data_7;
    }

    char* item_8_2 = item_8_1 - 1;

    if (!(arg4 & 8))
    {
        if (arg2)
            item_8_2 = context;

        result = 0;
    }
    else if (arg4 & 4)
    {
    label_4aacb5:
        g_936064 = 0x22;

        if (!(arg4 & 1))
        {
            int entity_6 = arg4;
            entity_6 &= 2;
            char f4_1 = entity_6;
            entity_6 = -(entity_6);
            result = -((entity_6 - entity_6)) + 0x7fffffff;
        }
        else
            result = 0xffffffff;
    }
    else if (!(arg4 & 1))
    {
        int entity_5 = arg4 & 2;

        if (entity_5 && result > 0x80000000)
            goto label_4aacb5;

        if (!entity_5 && result > 0x7fffffff)
            goto label_4aacb5;
    }

    if (arg2)
        *arg2 = item_8_2;

    if (!(arg4 & 2))
        return result;

    return -(result);
}
