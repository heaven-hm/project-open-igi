// Retail runtime observation: DefineMagicObj - 0x004C4520 - 381 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: QVM native callback observed; registry definition is distinct from placed MagicObj instances.
// IGI native provenance: Define_MagicObj - 0x004C4520 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C4520-Define_MagicObj.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Define_MagicObj.h"
int Define_MagicObj(int context, float arg2 )

// Sig: void Define_MagicObj(...)
// Note: Defines a MAGIC OBJECT type during magicobj definition parsing (pairs
// with the MAGICOBJ\ folder data). Parser-context only.
{
    int data_1 = g_a4433c * 0x28;
    float dest_val = arg2;
    *(data_1 + &g_b97e24) = 0;
    char* item_24 = 0x10;
    int item_2c = 0;
    float item_30 = dest_val;
    Sys_ResolveSymbol_23(item_30, item_2c, data_1 + &g_b97e00, item_24);
    Sys_ResolveSymbol_23(dest_val, 1, g_a4433c * 0x28 + &g_b97e10, 0x10);
    int source_1 = g_a4433c * 0x28;
    *(source_1 + 0xb97e20) = Script_GetArg(dest_val, 2);
    int data_6 = 0;
    data_6 = g_547f3d;
    int entity_1 = 0;
    *(source_1 + &g_b97e24) = 0;
    entity_1 = *(source_1 + 0xb97e20);
    int data_10 = *(((data_6 * 0x180 + entity_1) << 2) + &g_a96ae0);
    void* item_18;

    if (data_10)
    {
        data_10(0, &arg2);
        int item_2c_1 = 4;
        item_30 = arg2;
        void* data_12 = Mem_Alloc(item_30, item_2c_1);
        int entity_2 = g_a4433c;
        item_18 = data_12;
        int temp_1 = 0;
        float item_14_1 = dest_val;
        int item_10_1 = 3;
        int entity_4 = entity_2 * 0x28;
        *(entity_4 + &g_b97e24) = data_12;
        temp_1 = *(entity_4 + 0xb97e20);
        (*(((temp_1 + *g_547f3c * 0x180) << 2) + &g_a96ae0))(0, &item_18, item_30);
    }

    g_a4433c += 1;
    item_18 = 1;
    int item_10_2 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
