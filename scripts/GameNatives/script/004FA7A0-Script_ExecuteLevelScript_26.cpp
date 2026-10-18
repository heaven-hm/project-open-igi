// IGI native provenance: Script_ExecuteLevelScript_26 - 0x004FA7A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA7A0-Script_ExecuteLevelScript_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA7A0-Script_ExecuteLevelScript_26.hpp"
int Script_ExecuteLevelScript_26(void* context, int arg2, int arg3)

{
    int data_val;
    int entity_val;
    int temp_val;
    Script_ParseEnumReal32_25(data_val, temp_val, entity_val, context, 0xa);
    Script_FormatString_25(context, arg3);
    Sys_ResolveSymbol_4(*(context + 4));
    int result = Sys_CloseFileStream_4(*(context + 4), arg2, arg3);
    result = 1;
    return result;
}
