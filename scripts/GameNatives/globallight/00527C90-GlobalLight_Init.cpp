// IGI native provenance: GlobalLight_Init - 0x00527C90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00527C90-GlobalLight_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00527C90-GlobalLight_Init.hpp"
int* GlobalLight_Init()

{
    int item_1e4 = 0;
    g_a8408c = TaskType_Register("GlobalLight", 0x1b8, QTask_OpenEvent_2(), 0);
    int item_1e4_1 = 0;
    g_a84374 = TaskType_Register("GlobalLightKeyframe", 0x1a4, QTask_OpenEvent_2(), 0);
    QTask_RemoveEventCallback(g_a8408c, 0, sub_5280e0);
    QTask_TriggerEvent(g_a8408c, 1, AIGraph_LoadParameters);
    QTask_TriggerEvent(g_a8408c, 2, sub_528160);
    int data_4 = Script_ParseEnumInt32_8();
    int item_1e8 = data_4;
    data_4 = g_a8408c;
    QTask_RemoveEventCallback(data_4, item_1e8, sub_528170);
    int data_5 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_a8408c, data_5, GlobalLight_Setup);
    QTask_RemoveEventCallback(g_a84374, 0, sub_528260);
    QTask_TriggerEvent(g_a84374, 2, sub_528400);
    int data_7 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_a84374, data_7, sub_5283f0);
    int data_8 = Script_ParseEnumReal32_8();
    QTask_RemoveEventCallback(g_a84374, data_8, GlobalLight_Process);
    g_a84088 = Script_EvaluateCondition_7(6);
    int data_10 = Script_EvaluateCondition_2();
    void item_1b8;
    void item_170;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_170, data_10, 7, 0);
    int data_11 = Script_CallNativeHandler_2();
    void item_20;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_20, data_11, 7, 0);
    int data_12 = Script_CallNativeHandler_2();
    void item_14;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_14, data_12, 7, 0);
    int data_13 = Script_EvaluateCondition_2();
    void item_8;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_8, data_13, 7, 0x534ca0);
    int data_14 = Script_SpawnNewTask_2();
    void item_3;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_3, data_14, 6, 0);
    int data_15 = Script_SpawnNewTask_2();
    void item_4;
    Script_UndefinedParameterTy(g_a84088, &item_1b8, &item_4, data_15, 6, 0);
    g_a83f44 = Script_EvaluateCondition_7(0x28);
    int data_17 = Script_SpawnNewTask_2();
    Script_UndefinedParameterTy(g_a83f44, 0xa84130, 0xa842d0, data_17, 7, 0);

    for (int i = 0; i < 5; i += 1)
    {
        int item_1d8;

        if (i)
        {
            int i_1 = i;
            Game_DataSymbolLoad(&item_1d8, "object category %d");
        }
        else
        {
            item_1d8 = g_54f170[0];
            int item_1d4_1 = g_54f170[4][0];
        }

        int source_2 = i << 5;
        int* item_1e4_2 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa83e00, "Ambient color %s");
        int* item_1f0_6 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa842d4, "Fog color %s");
        int* item_1fc_1 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa83fe8, "Fog density %s");
        int* item_208_1 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa83ea4, "Scale R %s");
        int* item_214_1 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa83f48, "Scale G %s");
        int* item_220_1 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa84090, "Scale B %s");
        int* item_1e4_3 = &item_1d8;
        Game_DataSymbolLoad(source_2 + 0xa83d60, "Link setting %s");
        int source_6 = i * 0x44;
        int data_21 = Script_CallNativeHandler_2();
        Script_UndefinedParameterTy(g_a83f44, 0xa84130, source_6 + 0xa8417c, data_21, 7, 0);
        int data_23 = Script_CallNativeHandler_2();
        Script_UndefinedParameterTy(g_a83f44, 0xa84130, source_6 + 0xa84188, data_23, 7, 0);
        int data_25 = Script_EvaluateCondition_2();
        Script_UndefinedParameterTy(g_a83f44, 0xa84130, source_6 + 0xa84194, data_25, 7, 0);
        int data_26 = Script_ParseEnumString_3();
        Script_UndefinedParameterTy(g_a83f44, 0xa84130, source_6 + 0xa841bc, data_26, 7, 0);
    }

    int data_28 = Script_CallNativeHandler_2();
    Script_UndefinedParameterTy(g_a83f44, 0xa84130, 0xa84150, data_28, 7, 0);
    int data_29 = Script_CallNativeHandler_2();
    Script_UndefinedParameterTy(g_a83f44, 0xa84130, 0xa8415c, data_29, 7, 0);
    int data_31 = Script_CallNativeHandler_2();
    Script_UndefinedParameterTy(g_a83f44, 0xa84130, 0xa84168, data_31, 7, 0);
    int data_32 = Script_EvaluateCondition_2();
    Script_UndefinedParameterTy(g_a83f44, 0xa84130, 0xa84174, data_32, 7, 0);
    return SoundSys_InitSoundSys_13(AIGraph_LoadParameters);
}
