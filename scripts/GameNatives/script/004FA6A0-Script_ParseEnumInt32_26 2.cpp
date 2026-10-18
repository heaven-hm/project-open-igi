// IGI native provenance: Script_ParseEnumInt32_26 - 0x004FA6A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA6A0-Script_ParseEnumInt32_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumInt32_26.h"
int Script_ParseEnumInt32_26(void* context, int arg2, int arg3)

{
    char item_c = 7;
    int item_b = arg2;
    int item_7 = arg3;
    Sys_ResolveSymbol_4(*(context + 4));
    int result = Sys_CloseFileStream_4(*(context + 4), &item_c, 9);
    result = 1;
    return result;
}
