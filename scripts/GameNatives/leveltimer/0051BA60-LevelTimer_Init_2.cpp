// IGI native provenance: LevelTimer_Init_2 - 0x0051BA60 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051BA60-LevelTimer_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LevelTimer_Init_2.hpp"

int LevelTimer_Init_2(long double self , int* args )

{
    float dest_ptr = args[2];
    float* base_ptr = *args;
    Profile_ValueSet(args[1], dest_ptr);
    *base_ptr = self;
    base_ptr[1] = Script_GetArg(args[1], dest_ptr + 1);
    int result = Script_GetArg(args[1], dest_ptr + 2);
    base_ptr[2] = result;
    return result;
}
