// IGI native provenance: Script_ParseEnumReal32_25 - 0x004FA4E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA4E0-Script_ParseEnumReal32_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumReal32_25.h"
int Script_ParseEnumReal32_25(int context, int arg2, int arg3, void* arg4, char arg5)

{
    int item_4 = arg3;
    *(&item_4 + 2) = 0;
    *(&item_4 + 3) = arg5;
    Sys_ResolveSymbol_4(*(arg4 + 4));
    int result = Sys_CloseFileStream_4(*(arg4 + 4), (&item_4 + 2), 2);
    result = 1;
    return result;
}
