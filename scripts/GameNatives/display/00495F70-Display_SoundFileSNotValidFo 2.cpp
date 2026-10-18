// IGI native provenance: Display_SoundFileSNotValidFo - 0x00495F70 - CFG score 94.6% (PASS; structural only); runtime behavior not verified.
// Evidence: 00495F70-Display_SoundFileSNotValidFo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SoundFileSNotValidFo.h"
int Display_SoundFileSNotValidFo(int* context)

{
    char result = g_6840e9;

    if (!result)
        return result;

    int* item_88_1 = context;
    void item_68;
    void* entity_val;
    int temp_val;
    int* data_1 = Resource_Load(&item_68, temp_val, entity_val, &item_68);

    if (*data_1 != 0x46534c49)
    {
        Warning_Show("SOUND - File %s not valid format!");
        return Resource_Unload(context);
    }

    void item_44;
    Display_SoundInvalidFilename(&item_44, context);
    unsigned int data_4 = Display_CheckCapabilities_12(&item_44);
    int data_5 = Display_RestoreDevice_12(&item_44);

    if (Display_RestoreDevice_10(data_4, data_5))
    {
        int* item_84_3 = context;
        Warning_Show("SOUND - File %s already loaded!");
        return Resource_Unload(context);
    }

    void** flagP1 = Mem_Alloc(0x30, 4);
    memset(flagP1, 0, 0x30);
    memcpy(&flagP1[4], data_1, 0x14);

    switch (flagP1[5])
    {
        case 0:
        case 2:
        {
            int* dest_4 = context;
            int i = 0xffffffff;

            while (i)
            {
                bool flag_1_1 = 0 != *dest_4;
                dest_4 += 1;
                i -= 1;

                if (!flag_1_1)
                    break;
            }

            void* data_10 = Mem_Alloc(~i, 4);
            flagP1[0xa] = data_10;

            if (!data_10)
            {
                Sys_ExecuteBytecode_2(flagP1);
                flagP1 = nullptr;
                Resource_Unload(context);
            }

            int* dest_5 = context;
            int i_1 = 0xffffffff;

            while (i_1)
            {
                bool flag_2_1 = 0 != *dest_5;
                dest_5 += 1;
                i_1 -= 1;

                if (!flag_2_1)
                    break;
            }

            int entity_2 = ~i_1;
            int source_3;
            int dest_8;
            dest_8 = memcpy(flagP1[0xa], dest_5 - entity_2, entity_2 & 0xfffffffc);
            memcpy(dest_8, source_3, entity_2 & 3);
            break;
        }
        case 1:
        {
            flagP1[0xb] = &data_1[5];
            break;
        }
        case 3:
        {
            int source_4 = flagP1[8];
            void* data_12 = Mem_Alloc((flagP1[6] * *(flagP1 + 0x16) * source_4) >> 3, 4);
            flagP1[0xb] = data_12;

            if (!data_12)
            {
                Sys_ExecuteBytecode_2(flagP1);
                flagP1 = nullptr;
                Resource_Unload(context);
            }

            void item_64;
            Display_ClearSurfaces_10(&item_64, flagP1[6]);
            Display_SwapBuffers_10(&data_1[5], flagP1[0xb], source_4, &item_64);
            break;
        }
    }

    if (flagP1[0xb])
    {
        short item_48 = 0x12;
        short item_58 = 1;
        short item_56_1 = flagP1[6];
        int entity_12 = flagP1[7];
        int item_54_1 = entity_12;
        short item_4a_1 = *(flagP1 + 0x16);
        int data_19;
        int temp_5;
        temp_5 = HIGHD(flagP1[6] * *(flagP1 + 0x16));
        data_19 = LOWD(flagP1[6] * *(flagP1 + 0x16));
        unsigned int temp_8 = (data_19 + (temp_5 & 7)) >> 3;
        int item_50_1 = temp_8 * entity_12;
        int item_24;
        memset(&item_24, 0, 0x24);
        item_24 = 0x24;
        int item_20_1 = 0x100e8;
        int* data_24 = g_67a498;
        short* item_14_1 = &item_58;
        int item_1c_1 = flagP1[8] * temp_8;

        if ((*(*data_24 + 0xc))(data_24, &item_24, &flagP1[9], 0))
        {
            Sys_ExecuteBytecode_2(flagP1);
            return Resource_Unload(context);
        }

        if (!Display_SetGammaRamp_10(nullptr, flagP1, 0))
        {
            int* source_6 = flagP1[9];
            (*(*source_6 + 8))(source_6);
            Sys_ExecuteBytecode_2(flagP1);
            return Resource_Unload(context);
        }

        if (flagP1[5] == 3)
        {
            Sys_ExecuteBytecode_2(flagP1[0xb]);
            flagP1[0xb] = 0;
        }

        flagP1[0xb] = 0;
    }

    Display_SyncVSync_10(0x5ca1b8, data_4, data_5, flagP1);
    return Resource_Unload(context);
}
