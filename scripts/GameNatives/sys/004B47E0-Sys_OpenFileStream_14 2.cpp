// IGI native provenance: Sys_OpenFileStream_14 - 0x004B47E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B47E0-Sys_OpenFileStream_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_OpenFileStream_14.h"
int Sys_OpenFileStream_14(int* context, int arg2)

{
    Sys_ResolveSymbol_13(context);
    int temp_val = context[1];
    *context ^= arg2;
    int result = arg2 << 9;
    int entity_5 = context[2] - result;
    context[1] = temp_val + (arg2 << 0xf);
    context[2] = entity_5;
    return result;
}
