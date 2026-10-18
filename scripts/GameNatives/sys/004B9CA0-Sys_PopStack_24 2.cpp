// IGI native provenance: Sys_PopStack_24 - 0x004B9CA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B9CA0-Sys_PopStack_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_PopStack_24.h"
int Sys_PopStack_24(int* context, char** arg2)

{
    if (!Sys_CloseFileStream_31(context, &context[2]))
        return 0;

    return Sys_ResolveSymbol_24(context, arg2, 0, 0);
}
