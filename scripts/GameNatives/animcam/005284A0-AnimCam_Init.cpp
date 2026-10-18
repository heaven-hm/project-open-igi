// IGI native provenance: AnimCam_Init - 0x005284A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005284A0-AnimCam_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AnimCam_Init.hpp"
int* AnimCam_Init()

{
    int item_224 = 0;
    short data_1 = TaskType_Register("AnimCam", 0x220, CamTask_OnEvent(), 0);
    g_54f198 = data_1;
    QTask_TriggerEvent(data_1, 1, sub_52a180);
    QTask_TriggerEvent(g_54f198, 2, AIGraph_LoadParameters);
    QTask_RemoveEventCallback(g_54f198, 0, sub_528760);
    int data_3 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_54f198, data_3, sub_528cf0);
    int data_4 = Script_ParseEnumReal32_8();
    int item_228_1 = data_4;
    data_4 = g_54f198;
    QTask_RemoveEventCallback(data_4, item_228_1, AIGraph_LoadParameters);
    int data_5 = Script_ParseEnumReal32_15();
    QTask_RemoveEventCallback(g_54f198, data_5, AnimCam_Process);
    int data_6 = Script_ParseEnumString_15();
    QTask_RemoveEventCallback(g_54f198, data_6, sub_529a70);
    int data_7 = Script_EvaluateCondition_15();
    int item_228_4 = data_7;
    data_7 = g_54f198;
    QTask_RemoveEventCallback(data_7, item_228_4, sub_52a1c0);
    int data_8 = Script_FormatString_15();
    QTask_RemoveEventCallback(g_54f198, data_8, sub_52a1d0);
    int data_9 = Script_SpawnNewTask_16();
    QTask_RemoveEventCallback(g_54f198, data_9, sub_52a1f0);
    int data_10 = Script_ParseEnumInt32_16();
    int item_228_7 = data_10;
    data_10 = g_54f198;
    QTask_RemoveEventCallback(data_10, item_228_7, AnimCam_Update);
    g_a84388 = Script_EvaluateCondition_7(0xa);
    int data_12 = Script_ParseEnumString_3();
    void item_220;
    void item_148;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_148, data_12, 7, 0);
    int data_14 = Script_ParseEnumString_3();
    void item_144;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_144, data_14, 7, 0);
    int data_16 = Script_ParseEnumString_3();
    void item_140;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_140, data_16, 7, 0);
    int data_18 = Script_ParseEnumString_3();
    void item_13c;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_13c, data_18, 7, 0);
    int data_20 = Script_ParseEnumString_3();
    void item_138;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_138, data_20, 7, 0);
    int data_22 = Script_ParseEnumString_3();
    void item_134;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_134, data_22, 7, 0);
    int data_24 = Script_ParseEnumString_3();
    void item_130;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_130, data_24, 7, 0);
    int data_26 = Script_ParseEnumString_3();
    void item_12c;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_12c, data_26, 7, 0);
    int data_28 = Script_ParseEnumString_3();
    void item_128;
    Script_UndefinedParameterTy(g_a84388, &item_220, &item_128, data_28, 7, 0);
    int data_30 = Script_ParseEnumString_3();
    void item_124;
    return Script_UndefinedParameterTy(g_a84388, &item_220, &item_124, data_30, 7, 0);
}
