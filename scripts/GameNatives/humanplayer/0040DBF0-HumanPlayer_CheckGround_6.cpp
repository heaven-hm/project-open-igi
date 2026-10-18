// IGI native provenance: HumanPlayer_CheckGround_6 - 0x0040DBF0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0040DBF0-HumanPlayer_CheckGround_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_CheckGround_6.hpp"
void HumanPlayer_CheckGround_6(void* context, void* arg2)

{
    *(arg2 + 0x664) = *(arg2 + 0x664) * 0.99000001f;
    *(arg2 + 0x668) = *(arg2 + 0x668) * 0.99000001f;
    *(arg2 + 0x66c) = (*(arg2 + 0x66c) - 44.6008873f) * 0.99000001f;

    if (!(*(context + 8) & 4))
        return;

    void* data_1 = *(arg2 + 0xdb4);
    *(arg2 + 0xd7a) = 1;
    memset(arg2 + 0x664, 0, 0x18);

    if (data_1)
    {
        SoundSys_SetFrequency_7(data_1);
        *(arg2 + 0xdb4) = 0;
    }

    int* entity_1 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_3 = *entity_1;
    *(context + 0x14) = data_3;
    data_3(context, arg2);
}
