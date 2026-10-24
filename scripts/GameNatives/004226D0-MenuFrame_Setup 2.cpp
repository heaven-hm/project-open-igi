// IGI native provenance: MenuFrame_Setup - 0x004226D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004226D0-MenuFrame_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuFrame_Setup.h"
int MenuFrame_Setup(void* context)

{
    int context_val = (g_c28b44 >> 1) + *(context + 0x20) - 0x140;
    void* entity_1 = *(context + 0x28);
    int dest_val = (g_c28b48 >> 1) + *(context + 0x24) - 0xf0;
    int temp_2 = *(context + 0x2c);
    (*(((Menu_PlayMusic_13() + StatusScreen_GetEvent() * 0x180) << 2) + &g_a96ae0))(context);
    int data_8 = Menu_PlayMusic_11(context, context_val, dest_val, entity_1, 0.0f, 1, 2, 0);
    int dest_1 = dest_val + data_8;
    int flagP5 = temp_2
        - (Menu_PlayMusic_11(context, context_val, dest_val + temp_2, entity_1, 8.40779079e-45.0f, 5, 4, 1) + data_8);
    Menu_DrawWidget_11(context, context_val, dest_1, flagP5, 7, 0);
    return Menu_DrawWidget_11(context, entity_1 + context_val, dest_1, flagP5, 3, 1);
}
