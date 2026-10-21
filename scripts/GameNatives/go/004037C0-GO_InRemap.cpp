// IGI native provenance: GO_InRemap - 0x004037C0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004037C0-GO_InRemap.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GO_InRemap.h"
int GO_InRemap(int context, float arg2 )

// Sig: int GO_InRemap(int key1, int key2, int action)
// Note: Adds one input binding (key remap) to the current profile. Params: key1
// = primary key code, key2 = secondary key code, action = in-game action id
// being bound. Use in input/config scripts, repeat once per binding. Returns 1.
{
    int data_val = g_bc2380;
    int entity_4 = data_val * 0x159;
    int data_2 = data_val * 0x73 + *((entity_4 << 2) + &g_bc2358) - 0x73;
    int data_4 = Script_GetArg(arg2, 0);
    int item_30 = 1;
    *(data_2 * 0xc + &g_bc25bc) = data_4;
    *(data_2 * 0xc + 0xbc25c0) = Script_GetArg(arg2, item_30);
    *(data_2 * 0xc + 0xbc25c4) = Script_GetArg(arg2, 2) & 0x3fffffff;
    *((entity_4 << 2) + &g_bc2358) += 1;
    int item_18 = 0;
    int item_10 = 0;
    int item_c = 0;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
