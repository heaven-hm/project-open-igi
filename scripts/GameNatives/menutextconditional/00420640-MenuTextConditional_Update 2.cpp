// IGI native provenance: MenuTextConditional_Update - 0x00420640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00420640-MenuTextConditional_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuTextConditional_Update.h"
int MenuTextConditional_Update(long double context , int arg2, int arg3)

{
    void* data_val = *(arg2 + 0x44);
    *(arg2 + 0x28) = 0;
    *(arg2 + 0x2c) = 0;

    if (data_val)
    {
        void* data_1 = Menu_HandleInput_3(data_val, arg2 + 0x94);
        *(arg2 + 0x20c) = Menu_DrawWidget_3(*(arg2 + 0x44), "glow.spr");
        int* data_3 = *(arg2 + 0x114);

        if (data_3)
        {
            Sys_ReadFileBlock_18(data_3);
            *(arg2 + 0x114) = 0;
        }

        if (data_1)
        {
            int temp_2 = (g_c28b44 >> 1) + *(arg2 + 0x20) - 0x140;
            int temp_3 = (g_c28b48 >> 1) + *(arg2 + 0x24) - 0xf0;
            char* data_9;
            int entity_5;
            data_9 = Menu_LocalSS(*(arg2 + 0x44), arg2 + 0x54);
            int item_20_3 = entity_5;
            int item_24_1 = entity_5;
            int** data_10 = Sys_OpenFileStream_18(data_1, temp_2, temp_3, data_9);
            *(arg2 + 0x114) = data_10;
            data_10[8] = 0;
            *(arg2 + 0x28) = Sys_CheckHeap_21(*(arg2 + 0x114));
            *(arg2 + 0x2c) = Sys_AllocateMemory_22(*(arg2 + 0x114));
        }
    }

    Script_Isexists(arg2 + 0x11c, arg2);
    Script_Isexists(arg2 + 0x16c, arg2);
    Script_Isexists(arg2 + 0x1bc, arg2);

    if (*(arg2 + 0x11c))
        *(arg2 + 0x4c) = 1;

    if (Script_FormatString_6(arg2 + 0x16c))
    {
        Script_SpawnNewTask_7(arg2 + 0x16c, arg2);
        int item_8_1 = context;
        *(arg2 + 0x210) = item_8_1;
        *(arg2 + 0x4c) = item_8_1;
    }

    if (Script_FormatString_6(arg2 + 0x1bc))
    {
        Script_SpawnNewTask_7(arg2 + 0x1bc, arg2);
🚫🚫🚫        int item_8_2 = /* int item_8_2 = (unimplemented  {fistp qword [stack_ptr+0x18], fp0}).d */;
        /* unimplemented  {fistp qword [stack_ptr+0x18], fp0} */
        *(arg2 + 0x214) = item_8_2;
    }

    return (*(((Script_ParseEnumReal32_8() * 0x180 + Menu_PlayMusic_13()) << 2) + &g_a96ae0))(arg2, arg3);
}
