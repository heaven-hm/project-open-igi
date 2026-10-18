// IGI native provenance: Sys_CloseFileStream_30 - 0x004BE9A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BE9A0-Sys_CloseFileStream_30.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BE9A0-Sys_CloseFileStream_30.hpp"
int Sys_CloseFileStream_30(int* context)

{
    Sys_PopStack_29(context, *(*context + context[5] + 1));
    int result = context[5] + 5;
    context[5] = result;
    return result;
}
