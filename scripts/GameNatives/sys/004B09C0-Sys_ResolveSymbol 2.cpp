// IGI native provenance: Sys_ResolveSymbol - 0x004B09C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B09C0-Sys_ResolveSymbol.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ResolveSymbol.h"
int Sys_ResolveSymbol(int* context, int* arg2, int* arg3, char* arg4, int arg5, int arg6)

{
    void item_200;
    Sys_PopStack(&item_200, arg4);
    return Sys_DispatchOpcode(context, arg2, arg3, &item_200, arg5, arg6);
}
