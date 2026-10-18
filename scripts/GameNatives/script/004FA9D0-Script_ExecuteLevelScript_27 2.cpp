// IGI native provenance: Script_ExecuteLevelScript_27 - 0x004FA9D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA9D0-Script_ExecuteLevelScript_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ExecuteLevelScript_27.h"
int Script_ExecuteLevelScript_27(void* context, int arg2, int arg3)

{
    void* source_val = context;
    Script_CallNativeHandler_26(source_val, &context);
    int saved_ctx;
    Script_FormatString_26(source_val, &saved_ctx);
    Sys_ResolveSymbol_4(*(source_val + 4));
    int result = Resource_Unpack(*(source_val + 4), arg2, arg3);
    result = 1;
    return result;
}
