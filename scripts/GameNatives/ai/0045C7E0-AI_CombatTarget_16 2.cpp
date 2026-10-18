// IGI native provenance: AI_CombatTarget_16 - 0x0045C7E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045C7E0-AI_CombatTarget_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_CombatTarget_16.h"
int AI_CombatTarget_16(void* context, void* arg2)

{
    int result = *(context + 8);

    if (!(result & 4))
    {
        void* entity_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1d;
        int data_1 = *(entity_1 + 0x74);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (result & 0x10000)
    {
        void* entity_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1e;
        int data_4 = *(entity_2 + 0x78);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (result & 0x20000)
    {
        void* entity_3 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x20;
        int data_7 = *(entity_3 + 0x80);
        *(context + 0x14) = data_7;
        return data_7(context, arg2);
    }

    if (!(result & 0xfd81fe1) && *(arg2 + 0xd0c) != 0xffffffff)
        return result;

    FX_DebrisSpawn_15(arg2);
    int* entity_4 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0;
    int data_10 = *entity_4;
    *(context + 0x14) = data_10;
    return data_10(context, arg2);
}
