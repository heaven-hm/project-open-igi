// IGI native provenance: WeaponType_InvalidIntroscreenMo - 0x004154B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004154B0-WeaponType_InvalidIntroscreenMo.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "WeaponType_InvalidIntroscreenMo.h"
struct status_msg* WeaponType_InvalidIntroscreenMo(void* context, int arg2, char arg3)

{
    int flags;
    int kind;

    if (!arg2)
    {
        int data_1;
        data_1 = FX_SmokeEmit_26();
        char (** item_8_1)[0x16] = &g_5393f8;

        if (!data_1)
            char (** item_8_2)[0x16] = &g_5393e8;

        flags = 4;
    }
    else if (arg2 == 1)
    {
        char (** item_8)[0x16] = &g_539408;
        flags = 1;
    }
    else
    {
        Error_Show("Invalid introscreen mode");

        while (true)
            /* nop */
    }

    kind = g_57b198;
    char item_4 = arg3;
    return QTask_HandleInput(context, kind, &flags);
}
