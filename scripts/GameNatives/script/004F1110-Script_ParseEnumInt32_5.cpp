// IGI native provenance: Script_ParseEnumInt32_5 - 0x004F1110 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1110-Script_ParseEnumInt32_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F1110-Script_ParseEnumInt32_5.hpp"
int Script_ParseEnumInt32_5(void* context)

{
    int result = Script_EvaluateCondition_4(context);

    if (result >= 0)
        result = Script_SpawnNewTask_5(result);

    int* i = *(context + 8);

    if (*i && i)
    {
        do
        {
            result = Script_ParseEnumInt32_5(i);
            i = *i;

            if (!i)
                break;

            if (!*i)
                break;
        } while (i);
    }

    return result;
}
