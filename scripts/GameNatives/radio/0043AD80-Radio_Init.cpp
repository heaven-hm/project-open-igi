// IGI native provenance: Radio_Init - 0x0043AD80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043AD80-Radio_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043AD80-Radio_Init.hpp"
int* Radio_Init()

{
    int item_2f8 = 0;
    short data_1 = TaskType_Register("Radio", 0x2e8, FX_TrailUpdate(), 0);
    g_53c2e0 = data_1;
    QTask_RemoveEventCallback(data_1, 0, sub_43af20);
    QTask_TriggerEvent(g_53c2e0, 1, Radio_LoadParameters);
    QTask_TriggerEvent(g_53c2e0, 2, sub_43b110);
    int data_3 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_53c2e0, data_3, sub_43b160);
    int data_4 = Script_ParseEnumReal32_8();
    int item_2fc_1 = data_4;
    data_4 = g_53c2e0;
    QTask_RemoveEventCallback(data_4, item_2fc_1, sub_43b170);
    int data_5 = Script_FormatString_5();
    QTask_TriggerEvent(g_53c2e0, data_5, Radio_Setup);
    int data_6 = Script_SpawnNewTask_6();
    QTask_TriggerEvent(g_53c2e0, data_6, Radio_Process);
    int data_7 = AI_CheckHearing_25();
    int item_2fc_4 = data_7;
    data_7 = g_53c2e0;
    QTask_RemoveEventCallback(data_7, item_2fc_4, sub_443dc0);
    g_57bef8 = Script_FormatString_7(FX_ExplosionSpawn_2(), 6);
    int dest_val = 0;
    void item_54;
    void* context_val = &item_54;
    void item_2e8;

    for (char* i = &g_57befc; i < 0x57bf38; )
    {
        int item_2f8_1 = dest_val;
        sprintf(i, "Radio sound %d");
        int data_10 = Script_EvaluateCondition_3();
        Script_UndefinedParameterTy(g_57bef8, &item_2e8, context_val, data_10, 7, 0);
        i = &i[0xf];
        dest_val += 1;
        context_val += 0x10;
    }

    int data_11 = Script_EvaluateCondition_3();
    void item_14;
    Script_UndefinedParameterTy(g_57bef8, &item_2e8, &item_14, data_11, 7, 0);
    int data_12 = Script_CallNativeHandler_3();
    void item_a4;
    return Script_UndefinedParameterTy(g_57bef8, &item_2e8, &item_a4, data_12, 7, 0);
}
