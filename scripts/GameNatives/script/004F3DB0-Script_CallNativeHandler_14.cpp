// IGI native provenance: Script_CallNativeHandler_14 - 0x004F3DB0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F3DB0-Script_CallNativeHandler_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3DB0-Script_CallNativeHandler_14.hpp"
int* Script_CallNativeHandler_14(void* context, int* arg2)

{
    void* data_1 = Script_ExecuteLevelScript_4(*(context + 0x2014c));
    int* result = arg2;
    int source_val = *result;
    int item_4 = result[1];
    int item_8 = source_val;
    item_8 = source_val + 0xe8;
    memcpy(context + 0x6c, source_val, 0xe8);
    *(context + 0x20154) = result[1];

    if (data_1)
    {
        unsigned int data_3 = Script_SpawnNewTask_16();
        int entity_2 = 0;
        entity_2 = *(data_1 + 0x1c);
        result = *(((data_3 * 0x180 + entity_2) << 2) + &g_a96ae0);

        if (result)
            return result(data_1, &item_8);
    }

    return result;
}
