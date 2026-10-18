// IGI native provenance: MenuTextConditional_LoadParameters - 0x004207E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004207E0-MenuTextConditional_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuTextConditional_LoadParameters.hpp"
int MenuTextConditional_LoadParameters(long double context , void* arg2)

{
    if (Script_FormatString_6(arg2 + 0x16c))
    {
        Script_SpawnNewTask_7(arg2 + 0x16c, arg2);
        int item_8_1 = context;
        *(arg2 + 0x210) = item_8_1;
        *(arg2 + 0x4c) = item_8_1;
    }

    if (*(arg2 + 0x11c) && *(arg2 + 0x210))
    {
        Menu_UpdateLanguage_14(arg2, *(arg2 + 0x20), *(arg2 + 0x24), *(arg2 + 0x28),
            *(arg2 + 0x2c), nullptr, nullptr);

        if (*(arg2 + 0x49) && !*(arg2 + 0x48))
            menu_high(*(arg2 + 0x44), arg2);

        if ((*(arg2 + 0x4b) || *(arg2 + 0x50) == 0xfffffff9) && Script_FormatString_6(arg2 + 0x11c))
        {
            Script_SpawnNewTask_7(arg2 + 0x11c, arg2);
🚫🚫🚫            /* unimplemented  {fstp fp0, fp0} */
            /* unimplemented  {fstp fp0, fp0} */
            char* item_1c_2;

            if (!*(arg2 + 0xd4))
                item_1c_2 = "menu_sel";
            else
                item_1c_2 = arg2 + 0xd4;

            SoundSys_AttachLipsync_8(item_1c_2, 0, 0);
        }
    }

    *(arg2 + 0x50) = 0;
    return (*((Menu_PlayMusic_13() << 2) + &g_a970e0))(arg2);
}
