// IGI native provenance: AIStationaryGun_Process - 0x0046FA50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046FA50-AIStationaryGun_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIStationaryGun_Process.hpp"
int AIStationaryGun_Process(long double context , long double arg2 , long double arg3 , long double arg4 , long double arg5 , long double arg6 , long double arg7 , int* arg8 )

{
    int source_val = arg8[2];
    char* context_val = *arg8;
    Sys_ResolveSymbol_23(arg8[1], source_val, context_val, 0x20);
    Profile_ValueSet(arg8[1], source_val + 1);
    *(context_val + 0x28) = context * 0.0174532924f;
    Profile_ValueSet(arg8[1], source_val + 2);
    *(context_val + 0x20) = arg2 * 0.0174532924f;
    Profile_ValueSet(arg8[1], source_val + 3);
    *(context_val + 0x24) = arg3 * 0.0174532924f;
    *(context_val + 0x2c) = Script_GetArg(arg8[1], source_val + 4);
    Profile_ValueSet(arg8[1], source_val + 5);
    *(context_val + 0x30) = arg4 * 0.0174532924f * 0.0333333351f;
    Profile_ValueSet(arg8[1], source_val + 6);
    *(context_val + 0x34) = arg5 * 0.0174532924f * 0.0333333351f;
    Sys_ResolveSymbol_23(arg8[1], source_val + 7, &context_val[0x38], 0x20);
    Profile_ValueSet(arg8[1], source_val + 8);
    *(context_val + 0x58) = arg6 * 0.0174532924f * 0.0333333351f;
    int result = Profile_ValueSet(arg8[1], source_val + 9);
    *(context_val + 0x5c) = arg7 * 4096.0f;
    return result;
}
