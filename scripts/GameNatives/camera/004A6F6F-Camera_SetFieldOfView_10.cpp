// IGI native provenance: Camera_SetFieldOfView_10 - 0x004A6F6F - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A6F6F-Camera_SetFieldOfView_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A6F6F-Camera_SetFieldOfView_10.hpp"
int* Camera_SetFieldOfView_10(PSTR context, char* arg2, int arg3, int* arg4)

{
    int entity_val;
    int item_8 = entity_val;
    int item_c = entity_val;
    int source_val = g_936214;
    char* dest_val = arg2;
    int item_c_1 = 0;
    int item_8_1 = 0;
    char data_val = *dest_val;
    int entity_1;
    int source_1;

    if (data_val == 0x61)
    {
        entity_1 = 0x109;
    label_4a6fad:
        source_1 = source_val | 2;
    label_4a6fb2:
        int temp_1 = 1;

        while (true)
        {
            data_val = dest_val[1];
            dest_val = &dest_val[1];

            if (!data_val)
                break;

            if (!temp_1)
                break;

            int data_1 = data_val;

            if (data_1 > 0x54)
            {
                if (data_1 == 0x62)
                {
                    if (!(*(&entity_1 + 1) & 0xc0))
                    {
                        *(&entity_1 + 1) |= 0x80;
                        continue;
                    }
                }
                else if (data_1 == 0x63)
                {
                    if (!item_c_1)
                    {
                        item_c_1 = 1;
                        source_1 |= 0x4000;
                        continue;
                    }
                }
                else if (data_1 == 0x6e)
                {
                    if (!item_c_1)
                    {
                        item_c_1 = 1;
                        source_1 &= 0xffffbfff;
                        continue;
                    }
                }
                else if (data_1 == 0x74 && !(*(&entity_1 + 1) & 0xc0))
                {
                    *(&entity_1 + 1) |= 0x40;
                    continue;
                }
            }
            else if (data_1 == 0x54)
            {
                if (!(0x1000 & entity_1))
                {
                    entity_1 |= 0x1000;
                    continue;
                }
            }
            else if (data_1 == 0x2b)
            {
                if (!(entity_1 & 2))
                {
                    entity_1 = (entity_1 & 0xfffffffe) | 2;
                    source_1 = (source_1 & 0xfffffffc) | 0x80;
                    continue;
                }
            }
            else if (data_1 == 0x44)
            {
                if (!(entity_1 & 0x40))
                {
                    entity_1 |= 0x40;
                    continue;
                }
            }
            else if (data_1 == 0x52)
            {
                if (!item_8_1)
                {
                    item_8_1 = 1;
                    entity_1 |= 0x10;
                    continue;
                }
            }
            else if (data_1 == 0x53 && !item_8_1)
            {
                item_8_1 = 1;
                entity_1 |= 0x20;
                continue;
            }

            temp_1 = 0;
        }

        int item_20_1 = arg3;
        int data_3 = Camera_UpdateTransform_24(context, entity_1, 0xa4);

        if (data_3 >= 0)
        {
            g_936210 += 1;
            arg4[3] = source_1;
            arg4[1] = 0;
            *arg4 = 0;
            arg4[2] = 0;
            arg4[7] = 0;
            arg4[4] = data_3;
            return arg4;
        }
    }
    else
    {
        if (data_val == 0x72)
        {
            entity_1 = 0;
            source_1 = source_val | 1;
            goto label_4a6fb2;
        }

        if (data_val == 0x77)
        {
            entity_1 = 0x301;
            goto label_4a6fad;
        }
    }
    return nullptr;
}
