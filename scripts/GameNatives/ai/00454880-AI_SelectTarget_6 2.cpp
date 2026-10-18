// IGI native provenance: AI_SelectTarget_6 - 0x00454880 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00454880-AI_SelectTarget_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_SelectTarget_6.h"
int AI_SelectTarget_6(void* context, void* arg2)

{
    int result = *(context + 8);

    if (!(result & 4))
    {
        void* entity_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 8;
        int data_1 = *(entity_1 + 0x20);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (result & 0x10000)
    {
        void* entity_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 9;
        int data_4 = *(entity_2 + 0x24);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (result & 0x20000)
    {
        void* entity_3 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xa;
        int data_7 = *(entity_3 + 0x28);
        *(context + 0x14) = data_7;
        return data_7(context, arg2);
    }

    if (!(result & 0x301fe1) && *(arg2 + 0xd0c) != 0xffffffff)
        return result;

    FX_DebrisSpawn_15(arg2);
    int* entity_4 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_10 = *entity_4;
    *(context + 0x14) = data_10;
    return data_10(context, arg2);
}
