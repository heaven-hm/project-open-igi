// IGI native provenance: ListBox_Update - 0x0041EA70 - CFG score 92.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041EA70-ListBox_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0041EA70-ListBox_Update.hpp"
int ListBox_Update(void* context, int arg2)

{
    void* data_val = *(context + 0xd8);
    void* flagP = *(context + 0xd4);
    void* data_1 = Menu_HandleInput_3(*(context + 0x44), context + 0x94);
    int* data_2 = *(context + 0xbf4);

    if (data_2)
    {
        Sys_ReadFileBlock_18(data_2);
        *(context + 0xbf4) = 0;
    }

    if (data_1)
    {
        int** data_3 = Sys_OpenFileStream_18(data_1, 0, 0, &g_567c74);
        *(context + 0xbf4) = data_3;
        data_3[8] = 0;
    }

    Script_Isexists(context + 0xbf8, context);
    Script_Isexists(context + 0xc48, context);
    Script_Isexists(context + 0xc98, context);
    Script_Isexists(context + 0xce8, context);

    if (*(context + 0x44))
    {
        void* item_54_3 = context + 0x54;
        void item_40;
        Game_DataSymbolLoad(&item_40, "%s_1.spr");
        void* dest_1 = &item_40;
        int i = 0xffffffff;

        while (i)
        {
            bool flag_0_1 = 0 != *dest_1;
            dest_1 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int entity_2 = ~i;
        int source_2;
        int dest_4;
        dest_4 = memcpy(*(context + 0xd4) + 0x54, dest_1 - entity_2, entity_2 & 0xfffffffc);
        void* item_60_3 = context + 0x54;
        memcpy(dest_4, source_2, entity_2 & 3);
        Game_DataSymbolLoad(&item_40, "%s_2.spr");
        void* dest_5 = &item_40;
        int i_1 = 0xffffffff;

        while (i_1)
        {
            bool flag_1_1 = 0 != *dest_5;
            dest_5 += 1;
            i_1 -= 1;

            if (!flag_1_1)
                break;
        }

        int entity_6 = ~i_1;
        int source_4;
        int dest_8;
        dest_8 = memcpy(*(context + 0xd8) + 0x54, dest_5 - entity_6, entity_6 & 0xfffffffc);
        memcpy(dest_8, source_4, entity_6 & 3);
    }

    *(*(context + 0xd4) + 0x44) = *(context + 0x44);
    *(*(context + 0xd8) + 0x44) = *(context + 0x44);
    void* source_5 = *(context + 0xd4);
    unsigned int data_13 = Script_ParseEnumReal32_8();
    int data_14 = 0;
    data_14 = *(source_5 + 0x1c);
    (*(((data_13 * 0x180 + data_14) << 2) + &g_a96ae0))(source_5, arg2);
    void* source_6 = *(context + 0xd8);
    unsigned int data_17 = Script_ParseEnumReal32_8();
    int temp_10 = 0;
    temp_10 = *(source_6 + 0x1c);
    (*(((data_17 * 0x180 + temp_10) << 2) + &g_a96ae0))(source_6, arg2);
    int entity_16 = *(data_val + 0x2c);
    int data_21 = *(flagP + 0x2c);

    if (data_21 <= entity_16)
        data_21 = entity_16;

    *(context + 0x28) = *(context + 0xbec) + *(data_val + 0x28) + *(flagP + 0x28) + 6;
    *(context + 0x2c) = data_21 + 4;
    return (*(((Script_ParseEnumReal32_8() * 0x180 + Menu_PlayMusic_13()) << 2) + &g_a96ae0))(context, arg2);
}
