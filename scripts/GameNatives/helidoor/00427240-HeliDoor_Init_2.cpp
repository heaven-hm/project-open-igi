// IGI native provenance: HeliDoor_Init_2 - 0x00427240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00427240-HeliDoor_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HeliDoor_Init_2.hpp"

int HeliDoor_Init_2(long double self , long double args , int* flags )

{
    float dest_ptr = flags[2];
    float* base_ptr = *flags;
    Profile_ValueSet(flags[1], dest_ptr);
    base_ptr[1] = self * 4096.0f;
    int result = Profile_ValueSet(flags[1], dest_ptr + 1);
    *base_ptr = args * 4096.0f;
    return result;
}
