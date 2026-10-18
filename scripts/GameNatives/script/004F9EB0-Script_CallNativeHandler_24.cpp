// IGI native provenance: Script_CallNativeHandler_24 - 0x004F9EB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9EB0-Script_CallNativeHandler_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9EB0-Script_CallNativeHandler_24.hpp"
int Script_CallNativeHandler_24(void* context, char* arg2, int arg3, int* arg4, int arg5)

{
    int data_1;
    int entity_val;
    int temp_val;
    data_1 = Script_EvaluateCondition_25(context, Script_ExecuteLevelScript_24(arg2));
    int result;
    int entity_1;
    int temp_1;
    result = Script_ParseEnumReal32_25(data_1, temp_val, entity_val, context, arg3);

    if (arg3 > 0xa)
        return result;

    switch (arg3)
    {
        case 0:
        {
            temp_1 = *arg4;
            return Script_ParseEnumReal32_25(result, temp_1, arg4, context, temp_1);
            break;
        }
        case 1:
        {
            entity_1 = *arg4;
            return Script_ParseEnumString_25(arg4, temp_1, entity_1, context, entity_1);
            break;
        }
        case 2:
        {
            result = *arg4;
            return Script_ExecuteLevelScript_25(result, arg4, entity_1, context, result);
            break;
        }
        case 3:
        {
            temp_1 = *arg4;
            return Script_CallNativeHandler_25(result, temp_1, arg4, context, temp_1);
            break;
        }
        case 4:
        {
            return Script_EvaluateCondition_25(context, *arg4);
            break;
        }
        case 5:
        {
            return Script_FormatString_25(context, *arg4);
            break;
        }
        case 6:
        {
            return Script_SpawnNewTask_26(context, *arg4);
            break;
        }
        case 7:
        {
            return Script_ParseEnumInt32_26(context, *arg4, arg4[1]);
            break;
        }
        case 8:
        {
            return Script_ParseEnumReal32_26(context, arg4);
            break;
        }
        case 9:
        {
            return Script_ParseEnumString_26(context, arg4);
            break;
        }
        case 0xa:
        {
            return Script_ExecuteLevelScript_26(context, arg4, arg5);
            break;
        }
    }
}
