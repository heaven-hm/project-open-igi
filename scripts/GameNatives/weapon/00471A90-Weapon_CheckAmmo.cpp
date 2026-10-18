// IGI native provenance: Weapon_CheckAmmo - 0x00471A90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00471A90-Weapon_CheckAmmo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00471A90-Weapon_CheckAmmo.hpp"
int Weapon_CheckAmmo(float context, float arg2, float arg3, float arg4, float arg5, float arg6, int arg7)

{
    int entity_val;
    int item_4_1 = entity_val;
    float item_4;
    float* data_2;
    int entity_2;

    if (!arg6)
    {
        data_2 = context;
        item_4 = arg4;
        entity_2 = arg7;
        context = arg5 * *data_2;

        if (!entity_2)
            arg6 = (1.0f - *data_2) * arg5 + arg3;
        else
            arg6 = arg3;
    }
    else
    {
        arg6 = arg3;
        data_2 = context;
        context = arg5;
        entity_2 = arg7;
        item_4 = arg4 * *data_2;

        if (entity_2)
            arg2 = (1.0f - *data_2) * arg4 + arg3;
    }

    int item_1c = entity_2;
    return Sys_ExecuteBytecode_14(arg2, arg6, item_4, context, *data_2 * 64.0f, 64.0f, 0.0f, 0x3f000000, 0x80000, 0);
}
