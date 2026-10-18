// IGI native provenance: Script_ParseEnumReal32_6 - 0x004F1240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1240-Script_ParseEnumReal32_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumReal32_6.h"
int Script_ParseEnumReal32_6()

{
    int result = 0;

    if (Script_ExecuteLevelScript_4(0))
    {
        int i;

        do
        {
            result += 1;
            i = Script_ExecuteLevelScript_4(result);
        } while (i);
    }

    return result;
}
