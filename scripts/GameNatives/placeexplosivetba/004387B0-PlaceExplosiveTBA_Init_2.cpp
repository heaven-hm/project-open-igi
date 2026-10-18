// IGI native provenance: PlaceExplosiveTBA_Init_2 - 0x004387B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004387B0-PlaceExplosiveTBA_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "PlaceExplosiveTBA_Init_2.hpp"

int* PlaceExplosiveTBA_Init_2(long double self , int* args )

{
    int source_ptr = args[2];
    char* base_ptr = *args;
    float dest_ptr = args[1];
    *base_ptr = Script_GetArg(dest_ptr, source_ptr);
    base_ptr[1] = Script_GetArg(dest_ptr, source_ptr + 1);
    Sys_ResolveSymbol_23(dest_ptr, source_ptr + 2, &base_ptr[2], 0x18);
    Sys_ResolveSymbol_23(dest_ptr, source_ptr + 3, &base_ptr[0x1a], 0x18);
    Profile_ValueSet(dest_ptr, source_ptr + 4);
    *(base_ptr + 0x54) = self * 4096.0;
    Sys_ResolveSymbol_23(dest_ptr, source_ptr + 5, &base_ptr[0x32], 0x20);
    char* dest_ptr_1 = &base_ptr[2];
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *dest_ptr_1;
        dest_ptr_1 = &dest_ptr_1[1];
        i -= 1;

        if (!cond:0_1)
            break;
    }

    if (~i == 1)
        *(base_ptr + 0x58) = 0;
    else
        *(base_ptr + 0x58) = Level_LoadLevel_5(&base_ptr[2]);

    int i_1 = 0xffffffff;
    char* dest_ptr_2 = &base_ptr[0x1a];

    while (i_1)
    {
        bool cond:1_1 = 0 != *dest_ptr_2;
        dest_ptr_2 = &dest_ptr_2[1];
        i_1 -= 1;

        if (!cond:1_1)
            break;
    }

    if (~i_1 == 1)
    {
        *(base_ptr + 0x5c) = 0;
        args[3] = source_ptr + 6 - args[2];
        return args;
    }

    *(base_ptr + 0x5c) = Level_LoadLevel_5(&base_ptr[0x1a]);
    args[3] = source_ptr + 6 - args[2];
    return args;
}
