// IGI native provenance: Script_FormatString_12 - 0x004F2310 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004F2310-Script_FormatString_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_FormatString_12.h"
int Script_FormatString_12(void* context, int arg2, int arg3)

{
    int item_8 = arg2;
    int data_1 = 0;
    data_1 = g_a76c09;
    int entity_1 = 0;
    entity_1 = *(context + 0x1c);
    return (*(((data_1 * 0x180 + entity_1) << 2) + &g_a96ae0))(context, &item_8, item_8, arg3);
}
