// IGI native provenance: AI_CheckSector_8 - 0x004566A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004566A0-AI_CheckSector_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CheckSector_8.hpp"
int AI_CheckSector_8(void* context, void* arg2)

{
    AI_FireWeapon_21(arg2);
    int data_val = *(context + 8);

    if (!(data_val & 4))
    {
        void* entity_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xb;
        int data_2 = *(entity_1 + 0x2c);
        *(context + 0x14) = data_2;
        return data_2(context, arg2);
    }

    if (data_val & 0x20000)
    {
        void* data_4 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xd;
        int data_5 = *(data_4 + 0x34);
        *(context + 0x14) = data_5;
        return data_5(context, arg2);
    }

    void* data_7 = *(arg2 + 0x6ec);

    if (data_7)
    {
        struct status_msg* data_10 = SoundSys_SetMasterVolume_7(Script_FormatString(), data_7 + 0x114);

        if (data_10)
            SoundSys_StopSoundSample_7(data_10, arg2 + 0x6a8);
    }

    *(arg2 + 0x664) = 0;
    *(arg2 + 0x668) = 0;
    *(arg2 + 0x66c) = 0;
    int* data_11 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_12 = *data_11;
    *(context + 0x14) = data_12;
    return data_12(context, arg2);
}
