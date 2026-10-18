// IGI native provenance: Script_ParseEnumInt32_10 - 0x004F1D80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1D80-Script_ParseEnumInt32_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumInt32_10.h"
int* Script_ParseEnumInt32_10(int* context)

{
    int* result = context;
    int* i_1 = *result;

    for (int* i = *i_1; i; i = *i)
    {
        HeapList_Remove(i_1);
        result = Sys_ExecuteBytecode_2(i_1);
        i_1 = i;
    }

    return result;
}
