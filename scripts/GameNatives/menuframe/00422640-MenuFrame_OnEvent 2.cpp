// IGI native provenance: MenuFrame_OnEvent - 0x00422640 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00422640-MenuFrame_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuFrame_OnEvent.h"
int MenuFrame_OnEvent(void* context, int arg2)

{
    if (*(context + 0x44))
    {
        int i = 0;
        int* dest_1 = context + 0x94;

        do
        {
            int item_54_1 = i + 1;
            void* item_58_1 = context + 0x54;
            char item_40[0x40];
            sprintf(&item_40, "%s_%d.spr");
            *dest_1 = Menu_DrawWidget_3(*(context + 0x44), &item_40);
            i += 1;
            dest_1 = &dest_1[1];
        } while (i < 8);
    }

    return (*(((Script_ParseEnumReal32_8() * 0x180 + Menu_PlayMusic_13()) << 2) + &g_a96ae0))(context, arg2);
}
