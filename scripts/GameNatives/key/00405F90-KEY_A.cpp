// IGI native provenance: KEY_A - 0x00405F90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00405F90-KEY_A.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00405F90-KEY_A.hpp"
int KEY_A(int* context, int arg2)

{
    int saved_obj = (&g_bc28c0)[arg2 * 0x159];
    char const* const saved_index_1 = "GOInMouInv";
    char* j_2 = "%s(%d);\n";
    void item_100;
    Sys_CloseFileStream_4(context, &item_100, Game_DataSymbolLoad(&item_100, j_2));
    saved_index_1 = (&g_bc28c4)[arg2 * 0x159];
    j_2 = "GOInMouSens";
    Sys_CloseFileStream_4(context, &item_100, Game_DataSymbolLoad(&item_100, "%s(%f);\n"));
    int i = (&g_bc28bc)[arg2 * 0x159];
    int i_1 = 0;

    if (i > 0)
    {
        int* context_1 = arg2 * 0x564 + &g_bc25bc;

        do
        {
            j_2 = "GOInRemap";
            int data_7 = Game_DataSymbolLoad(&item_100, "%s(");
            char* j = *context_1;
            int entity_1 = 0;
            int* data_8 = &g_536578;

            while (*data_8 != j)
            {
                data_8 = &data_8[9];
                entity_1 += 1;

                if (data_8 >= 0x536944)
                    break;
            }

            char* item_120_3;
            char* item_11c_1;
            int saved_target;

            if (entity_1 == 0x1b)
            {
                j_2 = j;
                item_11c_1 = &g_5383cc;
                item_120_3 = &j_2 + data_7 + 0x18;
            }
            else
            {
                j_2 = &"MoveLeft"[entity_1 * 4];
                item_11c_1 = &g_5383d0;
                item_120_3 = &saved_target + data_7 + 0x14;
            }

            int data_11 = Game_DataSymbolLoad(item_120_3, item_11c_1);
            char* j_1 = context_1[1];
            int source_2 = data_7 + data_11;
            int entity_3 = 0;
            int* data_12 = &g_536948;

            while (*data_12 != j_1)
            {
                data_12 = &data_12[9];
                entity_3 += 1;

                if (data_12 >= 0x5369b4)
                    break;
            }

            char* item_120_4;
            char* item_11c_2;

            if (entity_3 == 3)
            {
                j_2 = j_1;
                item_11c_2 = &g_5383cc;
                item_120_4 = &j_2 + source_2 + 0x18;
            }
            else
            {
                j_2 = &g_536928[entity_3 * 0x24];
                item_11c_2 = &g_5383d0;
                item_120_4 = &saved_target + source_2 + 0x14;
            }

            int data_14 = Game_DataSymbolLoad(item_120_4, item_11c_2);
            char* j_3 = context_1[2];
            int source_3 = source_2 + data_14;
            int temp_3 = 0;
            int* entity_6 = &g_5369b8;

            while (*entity_6 != (context_1[1] << 0x1e | j_3))
            {
                entity_6 = &entity_6[9];
                temp_3 += 1;

                if (entity_6 >= "DefineGameOptions")
                    break;
            }

            char* item_120_5;
            char* item_11c_3;

            if (temp_3 == 0x7f)
            {
                j_2 = j_3;
                item_11c_3 = "%d);\n";
                item_120_5 = &j_2 + source_3 + 0x18;
            }
            else
            {
                j_2 = &g_536998[temp_3 * 0x24];
                item_11c_3 = "%s);\n";
                item_120_5 = &saved_target + source_3 + 0x14;
            }

            j_2 = source_3 + Game_DataSymbolLoad(item_120_5, item_11c_3);
            Sys_CloseFileStream_4(context, &item_100, j_2);
            i = i_1 + 1;
            context_1 = &context_1[3];
            i_1 = i;
        } while (i < (&g_bc28bc)[arg2 * 0x159]);
    }

    return i;
}
