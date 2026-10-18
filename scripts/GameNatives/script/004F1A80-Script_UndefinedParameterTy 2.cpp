// IGI native provenance: Script_UndefinedParameterTy - 0x004F1A80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1A80-Script_UndefinedParameterTy.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_UndefinedParameterTy.h"
int* Script_UndefinedParameterTy(int* context, int arg2, int arg3, int arg4, char arg5, int arg6)

{
    if (arg4 != 0xffffffff)
    {
        *((*context << 4) + context[2]) = arg3 - arg2;
        *((*context << 4) + context[2] + 4) = arg4;
        *((*context << 4) + context[2] + 8) = arg6;
        *((*context << 4) + context[2] + 0xc) = arg5;
        *context += 1;
        return context;
    }

    Error_Show("Undefined parameter type.");

    while (true)
        /* nop */
}
