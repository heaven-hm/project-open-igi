// IGI native provenance: Grenade_Action - 0x004768A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004768A0-Grenade_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Grenade_Action.h"
int* Grenade_Action(int* context )

{
    void* data_1 = *(context[1] + 0x6e0);

    if (data_1)
    {
        Sys_ExecuteBytecode_2(data_1);
        *(context[1] + 0x6e0) = 0;
    }

    int* data_2 = Script_FormatString_31(*context, "Grenade - Explosion falloff (meters)");
    int* context_val = data_2;

    if (!context_val)
        context_val = Script_CallNativeHandler_31(*context, "Grenade - Explosion falloff (meters)", data_2);

    int* dest_val = Script_FormatString_31(*context, "Grenade - Explosion radius (meters)");

    if (!dest_val)
        dest_val = Script_CallNativeHandler_31(*context, "Grenade - Explosion radius (meters)", 0x40a00000);

    int* data_8 = Script_FormatString_31(*context, "Grenade - Explosion damage factor");

    if (!data_8)
        data_8 = Script_CallNativeHandler_31(*context, "Grenade - Explosion damage factor", 0x3f800000);

    int temp_2 = context_val[0x49];
    int entity_4 = dest_val[0x49];
    int temp_3 = data_8[0x49];
    *(context[1] + 0x6e0) = Mem_Alloc(0xc, 4);
    int* result = *(context[1] + 0x6e0);
    *result = temp_3;
    result[1] = entity_4;
    result[2] = temp_2;
    return result;
}
