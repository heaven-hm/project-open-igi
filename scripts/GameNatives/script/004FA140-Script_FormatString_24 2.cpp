// IGI native provenance: Script_FormatString_24 - 0x004FA140 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA140-Script_FormatString_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_FormatString_24.h"
int* Script_FormatString_24(int context)

{
    int* result = g_a774a0;

    for (int* i = *result; i; i = *i)
    {
        if (result[2] == context)
            return result;

        result = i;
    }

    return nullptr;
}
