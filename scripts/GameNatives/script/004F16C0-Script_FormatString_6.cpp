// IGI native provenance: Script_FormatString_6 - 0x004F16C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F16C0-Script_FormatString_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F16C0-Script_FormatString_6.hpp"
int* Script_FormatString_6(int* context)

{
    int* result = context;

    if (!*result)
    {
        result = 0;
        return result;
    }

    if (*result[1])
    {
        result = result[2];
        return result;
    }

    result = 0;
    return result;
}
