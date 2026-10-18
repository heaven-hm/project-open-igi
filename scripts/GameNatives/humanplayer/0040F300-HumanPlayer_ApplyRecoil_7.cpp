// IGI native provenance: HumanPlayer_ApplyRecoil_7 - 0x0040F300 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0040F300-HumanPlayer_ApplyRecoil_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_ApplyRecoil_7.hpp"
int HumanPlayer_ApplyRecoil_7(void* context, void* arg2)

{
    if (*(arg2 + 0xd4c) != 0xffffffff)
    {
        void* temp_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 4;
        int entity_2 = *(temp_1 + 0x10);
        *(context + 0x14) = entity_2;
        return entity_2(context, arg2);
    }

    char result = AI_CheckSector_22(*(arg2 + 0x4ec));

    if (!result)
        return result;

    *(arg2 + 0x4d0) = 1;
    int temp_2 = *(context + 0x10);
    *(context + 0x10) = 1;
    void* entity_3 = *(context + 4);
    *(context + 0xc) = temp_2;
    int entity_4 = *(entity_3 + 4);
    *(context + 0x14) = entity_4;
    return entity_4(context, arg2);
}
