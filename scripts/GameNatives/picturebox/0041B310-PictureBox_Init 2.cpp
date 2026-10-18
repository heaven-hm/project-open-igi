// IGI native provenance: PictureBox_Init - 0x0041B310 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041B310-PictureBox_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PictureBox_Init.h"
int* PictureBox_Init()

{
    int item_a00 = 0;
    short data_1 = TaskType_Register("PictureBox", 0x9fc, Menu_PlayMusic_13(), 0);
    g_53b58c = data_1;
    QTask_TriggerEvent(data_1, 1, PictureBox_LoadParameters);
    int data_2 = StatusScreen_GetEvent();
    int item_a04 = data_2;
    data_2 = g_53b58c;
    QTask_TriggerEvent(data_2, item_a04, PictureBox_Setup);
    QTask_TriggerEvent(g_53b58c, 2, PictureBox_Process);
    QTask_RemoveEventCallback(g_53b58c, 0, sub_41b490);
    int data_3 = Script_ParseEnumInt32_8();
    int item_a04_1 = data_3;
    data_3 = g_53b58c;
    QTask_RemoveEventCallback(data_3, item_a04_1, sub_41b830);
    int data_4 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_53b58c, data_4, PictureBox_Update);
    int data_5 = Menu_HandleInput_13();
    QTask_TriggerEvent(g_53b58c, data_5, PictureBox_OnEvent);
    int data_6 = Menu_RequestScreen_14();
    int item_a04_4 = data_6;
    data_6 = g_53b58c;
    QTask_TriggerEvent(data_6, item_a04_4, sub_41a120);
    g_57bba8 = Script_EvaluateCondition_7(4);
    int data_8 = Script_ParseEnumString_3();
    void item_9fc;
    void item_9d4;
    Script_UndefinedParameterTy(g_57bba8, &item_9fc, &item_9d4, data_8, 7, 0);
    int data_10 = Script_ParseEnumString_3();
    void item_9d0;
    Script_UndefinedParameterTy(g_57bba8, &item_9fc, &item_9d0, data_10, 7, 0);
    int data_12 = Script_CallNativeHandler_3();
    void item_a0;
    Script_UndefinedParameterTy(g_57bba8, &item_9fc, &item_a0, data_12, 7, 0);
    int data_14 = Script_CallNativeHandler_3();
    void item_50;
    return Script_UndefinedParameterTy(g_57bba8, &item_9fc, &item_50, data_14, 7, 0);
}
