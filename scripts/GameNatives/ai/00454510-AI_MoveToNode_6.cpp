// IGI native provenance: AI_MoveToNode_6 - 0x00454510 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00454510-AI_MoveToNode_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00454510-AI_MoveToNode_6.hpp"
int AI_MoveToNode_6(void* context, void* arg2)

{
    AI_FireWeapon_21(arg2);
    int result = *(context + 8);

    if (result & 0x10000)
    {
        void* entity_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 9;
        int data_1 = *(entity_1 + 0x24);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (result & 0x20000)
    {
        void* data_3 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xa;
        int data_4 = *(data_3 + 0x28);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (!(result & 4))
        return result;

    void* data_6 = *(arg2 + 0x6ec);

    if (data_6)
    {
        struct status_msg* data_9 = SoundSys_SetMasterVolume_7(Script_FormatString(), data_6 + 0x114);

        if (data_9)
            SoundSys_StopSoundSample_7(data_9, arg2 + 0x6a8);
    }

    *(arg2 + 0x664) = 0;
    *(arg2 + 0x668) = 0;
    *(arg2 + 0x66c) = 0;
    int* data_10 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_11 = *data_10;
    *(context + 0x14) = data_11;
    return data_11(context, arg2);
}
