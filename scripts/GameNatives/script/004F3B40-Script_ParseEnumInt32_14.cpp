// IGI native provenance: Script_ParseEnumInt32_14 - 0x004F3B40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F3B40-Script_ParseEnumInt32_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3B40-Script_ParseEnumInt32_14.hpp"
char Script_ParseEnumInt32_14(long double context , int arg2)

{
    if (Script_FormatString_6(arg2 + 0x200f8))
    {
        Script_SpawnNewTask_7(arg2 + 0x200f8, arg2);
        *(arg2 + 0x200f4) = context;
    }

    char result = *(arg2 + 0x200f4);

    if (!result)
        return result;

    return Script_FormatString_14(arg2, *(arg2 + 0x20154) + 1);
}
