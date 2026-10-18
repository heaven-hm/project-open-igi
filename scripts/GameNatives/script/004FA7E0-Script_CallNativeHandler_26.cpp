// IGI native provenance: Script_CallNativeHandler_26 - 0x004FA7E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA7E0-Script_CallNativeHandler_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA7E0-Script_CallNativeHandler_26.hpp"
char* Script_CallNativeHandler_26(void* context, char* arg2)

{
    void* source_val = context;
    Sys_ResolveSymbol_4(*(source_val + 4));
    Resource_Unpack(*(source_val + 4), &context, 2);
    char* result = arg2;
    *result = *(&context + 1);
    result = 1;
    return result;
}
