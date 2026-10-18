// IGI native provenance: AI_SelectTarget_13 - 0x0045B1C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045B1C0-AI_SelectTarget_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0045B1C0-AI_SelectTarget_13.hpp"
void* AI_SelectTarget_13(void* context, int arg2)

{
    void* result = Table_GetAt(arg2 + 0x4c4, 0);
    int context_val = *(context + 8);

    if (!(context_val & 4))
    {
        void* temp_val = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1d;
        int data_1 = *(temp_val + 0x74);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (context_val & 0x10000)
    {
        void* entity_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1e;
        int data_4 = *(entity_2 + 0x78);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (context_val & 0x20000)
    {
        void* data_6 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x20;
        int data_7 = *(data_6 + 0x80);
        *(context + 0x14) = data_7;
        return data_7(context, arg2);
    }

    if (!(context_val & 1))
        return result;

    Script_SpawnNewTask_13(result, 1);

    if (!(context_val & 0x2000000))
    {
        int* entity_4 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0;
        int data_12 = *entity_4;
        *(context + 0x14) = data_12;
        return data_12(context, arg2);
    }

    void* temp_2 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0xa;
    int data_9 = *(temp_2 + 0x28);
    *(context + 0x14) = data_9;
    return data_9(context, arg2);
}
