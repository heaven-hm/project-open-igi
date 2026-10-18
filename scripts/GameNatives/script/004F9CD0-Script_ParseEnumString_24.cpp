// IGI native provenance: Script_ParseEnumString_24 - 0x004F9CD0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9CD0-Script_ParseEnumString_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9CD0-Script_ParseEnumString_24.hpp"
char* Script_ParseEnumString_24(int* context, double* arg2, int arg3)

{
    Script_CallNativeHandler_27(*context);
    int result = context[4];

    if (result > 0xa)
        return result;

    switch (result)
    {
        case 0:
        {
            return Script_CallNativeHandler_26(*context, arg2);
            break;
        }
        case 1:
        {
            return Script_CallNativeHandler_26(*context, arg2);
            break;
        }
        case 2:
        {
            return Script_EvaluateCondition_26(*context, arg2);
            break;
        }
        case 3:
        {
            return Script_EvaluateCondition_26(*context, arg2);
            break;
        }
        case 4:
        {
            return Script_FormatString_26(*context, arg2);
            break;
        }
        case 5:
        {
            return Script_FormatString_26(*context, arg2);
            break;
        }
        case 6:
        {
            return Script_SpawnNewTask_27(*context, arg2);
            break;
        }
        case 7:
        {
            return Script_ParseEnumInt32_27(*context, arg2);
            break;
        }
        case 8:
        {
            return Script_ParseEnumReal32_27(*context, arg2);
            break;
        }
        case 9:
        {
            int item_8_10 = arg3;
            return Script_ParseEnumString_27(*context, arg2);
            break;
        }
        case 0xa:
        {
            return Script_ExecuteLevelScript_27(*context, arg2, arg3);
            break;
        }
    }
}
