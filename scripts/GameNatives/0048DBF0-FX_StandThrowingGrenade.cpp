// IGI native provenance: FX_StandThrowingGrenade - 0x0048DBF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0048DBF0-FX_StandThrowingGrenade.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0048DBF0-FX_StandThrowingGrenade.hpp"
int FX_StandThrowingGrenade(void* context)

{
    char* item_8 = "Stand - Throwing grenade";
    int item_c = 0xffffffff;
    void* item_10 = context;

    if (!Locomotion_GetState(context, 0))
        return 1;

    void* result = AI_WriteChannel(context, 8, 1.0f);
    result = 0;
    return result;
}
