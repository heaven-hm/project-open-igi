// IGI native provenance: Script_FormatString_25 - 0x004FA620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA620-Script_FormatString_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA620-Script_FormatString_25.hpp"
int Script_FormatString_25(void* context, int arg2)

{
    char item_8 = 5;
    int item_7 = arg2;
    Sys_ResolveSymbol_4(*(context + 4));
    int result = Sys_CloseFileStream_4(*(context + 4), &item_8, 5);
    result = 1;
    return result;
}
