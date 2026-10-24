// IGI native provenance: Siren_Init_2 - 0x00449220 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00449220-Siren_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00449220-Siren_Init_2.hpp"
int Siren_Init_2(long double self , long double args , int* flags )

{
    int dest_ptr = flags[2];
    char* base_ptr = *flags;
    Sys_ResolveSymbol_23(flags[1], dest_ptr, base_ptr, 0x20);
    Profile_ValueSet(flags[1], dest_ptr + 1);
    *(base_ptr + 0x20) = self;
    int result = Profile_ValueSet(flags[1], dest_ptr + 2);
    *(base_ptr + 0x24) = args;
    return result;
}
