// IGI native provenance: AI_AlertAllies_8 - 0x004564B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004564B0-AI_AlertAllies_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_AlertAllies_8.h"
void* AI_AlertAllies_8(void* context, int arg2)

{
    void* result = Table_GetAt(arg2 + 0x4c4, 0);
    int entity_val = *(context + 8);

    if (!(entity_val & 4))
    {
        void* temp_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xb;
        int data_1 = *(temp_1 + 0x2c);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (entity_val & 0x10000)
    {
        void* entity_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xc;
        int data_4 = *(entity_2 + 0x30);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (entity_val & 0x20000)
    {
        void* data_6 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xd;
        int data_7 = *(data_6 + 0x34);
        *(context + 0x14) = data_7;
        return data_7(context, arg2);
    }

    if (!(entity_val & 1))
        return result;

    Script_SpawnNewTask_13(result, 1);
    void* temp_3 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 7;
    int data_9 = *(temp_3 + 0x1c);
    *(context + 0x14) = data_9;
    return data_9(context, arg2);
}
