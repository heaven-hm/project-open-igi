// IGI native provenance: Script_CallNativeHandler_25 - 0x004FA5A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA5A0-Script_CallNativeHandler_25.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004FA5A0-Script_CallNativeHandler_25.hpp"
int Script_CallNativeHandler_25(int context, int arg2, int arg3, void* arg4, short arg5)

{
    int item_4 = arg3;
    item_4 = 3;
    *(&item_4 + 1) = arg5;
    Sys_ResolveSymbol_4(*(arg4 + 4));
    int result = Sys_CloseFileStream_4(*(arg4 + 4), &item_4, 3);
    result = 1;
    return result;
}
