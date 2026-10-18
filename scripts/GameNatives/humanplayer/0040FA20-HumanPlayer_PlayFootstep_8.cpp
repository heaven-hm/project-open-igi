// IGI native provenance: HumanPlayer_PlayFootstep_8 - 0x0040FA20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040FA20-HumanPlayer_PlayFootstep_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_PlayFootstep_8.hpp"
int HumanPlayer_PlayFootstep_8(void* context, void* arg2)

{
    char result = AI_CheckSector_22(*(arg2 + 0x4ec));

    if (!result)
        return result;

    void* temp_1 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 1;
    int entity_2 = *(temp_1 + 4);
    *(context + 0x14) = entity_2;
    return entity_2(context, arg2);
}
