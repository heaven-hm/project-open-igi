// IGI native provenance: Script_ExecuteLevelScript_12 - 0x004F2070 - CFG score 86.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F2070-Script_ExecuteLevelScript_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F2070-Script_ExecuteLevelScript_12.hpp"
void* Script_ExecuteLevelScript_12(int context, char* arg2, void* arg3)

{
    char* dest_val = arg2;
    void* result = Table_GetAt(context, dest_val);
    void* result_1 = result;

    if (result_1)
    {
        int data_val = 0;
        int entity_1 = 0;
        data_val = g_a76c03;
        entity_1 = *(result_1 + 0x1c);
        (*(((data_val * 0x180 + entity_1) << 2) + &g_a96ae0))(result_1, &arg2);
        *arg2 = 0;
        int data_5 = Script_SpawnNewTask_12();
        int temp_1 = 0;
        temp_1 = *(result_1 + 0x1c);
        result = *(((data_5 * 0x180 + temp_1) << 2) + &g_a96ae0);

        if (result)
            result = result(result_1);
    }

    if (arg3)
    {
        int data_6 = 0;
        int entity_5 = 0;
        data_6 = g_a76c03;
        entity_5 = *(arg3 + 0x1c);
        (*(((data_6 * 0x180 + entity_5) << 2) + &g_a96ae0))(arg3, &arg2);
        *arg2 = 1;
        int data_11 = Script_FormatString_11();
        int temp_2 = 0;
        temp_2 = *(arg3 + 0x1c);
        result = (*(((data_11 * 0x180 + temp_2) << 2) + &g_a96ae0))(arg3);
    }

    *(context + (dest_val << 2)) = arg3;
    return result;
}
