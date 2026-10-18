// IGI native provenance: ExplodeMagicObj_LoadParameters - 0x0043C1F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043C1F0-ExplodeMagicObj_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043C1F0-ExplodeMagicObj_LoadParameters.hpp"
int ExplodeMagicObj_LoadParameters(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , int* arg6 )

{
    int source_val = arg6[2];
    char* context_val = *arg6;
    Sys_ResolveSymbol_23(arg6[1], source_val, context_val, 0x10);
    Sys_ResolveSymbol_23(arg6[1], source_val + 1, &context_val[0x2c], 0x10);
    Profile_ValueSet(arg6[1], source_val + 2);
    *(context_val + 0x10) = context;
    *(context_val + 0x28) = Script_GetArg(arg6[1], source_val + 3);
    *(context_val + 0x24) = Script_GetArg(arg6[1], source_val + 4);
    Profile_ValueSet(arg6[1], source_val + 5);
    *(context_val + 0x20) = arg2;
    Profile_ValueSet(arg6[1], source_val + 6);
    *(context_val + 0x18) = arg3;
    Profile_ValueSet(arg6[1], source_val + 7);
    *(context_val + 0x1c) = arg4;
    int result = Profile_ValueSet(arg6[1], source_val + 8);
    *(context_val + 0x14) = arg5;
    return result;
}
