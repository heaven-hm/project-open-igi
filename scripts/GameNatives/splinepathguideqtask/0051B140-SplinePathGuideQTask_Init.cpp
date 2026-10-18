// IGI native provenance: SplinePathGuideQTask_Init - 0x0051B140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051B140-SplinePathGuideQTask_Init.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SplinePathGuideQTask_Init.hpp"
int* SplinePathGuideQTask_Init()

{
    int item_48 = 0;
    g_54ed50 = TaskType_Register("SplinePathGuideQTask", 0x44, QTask_OpenEvent_2(), 0);
    g_54ed52 = QTask_UnableToAllocateEvent(0);
    g_54ed53 = QTask_UnableToAllocateEvent(0);
    QTask_TriggerEvent(g_54ed50, 1, SplinePathGuideQTask_LoadParameters);
    QTask_RemoveEventCallback(g_54ed50, 0, sub_51ae50);
    int data_4 = Script_ParseEnumInt32_8();
    QTask_RemoveEventCallback(g_54ed50, data_4, sub_51b390);
    int data_5 = Script_ParseEnumReal32_8();
    int item_4c_1 = data_5;
    data_5 = g_54ed50;
    QTask_RemoveEventCallback(data_5, item_4c_1, SplinePathGuideQTask_Setup);
    QTask_TriggerEvent(g_54ed50, 2, sub_51ae90);
    g_a83754 = Script_EvaluateCondition_7(5);
    int data_7 = Script_ParseEnumString_3();
    void item_44;
    void item_24;
    Script_UndefinedParameterTy(g_a83754, &item_44, &item_24, data_7, 7, 0);
    int data_8 = Script_ParseEnumString_3();
    void item_20;
    Script_UndefinedParameterTy(g_a83754, &item_44, &item_20, data_8, 7, 0);
    int data_9 = Script_ParseEnumString_3();
    void item_14;
    Script_UndefinedParameterTy(g_a83754, &item_44, &item_14, data_9, 7, 0);
    int data_10 = Script_ParseEnumString_3();
    void item_10;
    Script_UndefinedParameterTy(g_a83754, &item_44, &item_10, data_10, 7, 0);
    int data_11 = Script_EvaluateCondition_2();
    void item_4;
    Script_UndefinedParameterTy(g_a83754, &item_44, &item_4, data_11, 7, 0);
    int* result = Mem_Alloc8(0x64);
    g_a83760 = result;
    return result;
}
