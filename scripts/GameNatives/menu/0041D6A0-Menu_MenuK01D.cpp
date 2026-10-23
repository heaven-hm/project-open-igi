// IGI native provenance: Menu_MenuK01D - 0x0041D6A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041D6A0-Menu_MenuK01D.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_MenuK01D.hpp"
int Menu_MenuK01D(void* context, int arg2, char arg3)

{
    int i = 0xffffffff;
    void* dest_val = context + 0x54;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    for (int i_1 = ~i - 1; i_1 >= arg2; i_1 -= 1)
        *(context + i_1 + 0x55) = *(context + i_1 + 0x54);

    *(context + arg2 + 0x54) = arg3;
    *(context + 0x1a0) = arg2 + 1;
    int data_4;
    int temp_val;
    temp_val = HIGHD(arg3);
    data_4 = LOWD(arg3);
    int item_4c = ((data_4 ^ temp_val) - temp_val) % 7 + 1;
    void item_40;
    sprintf(&item_40, "menu_k0%.1d");
    return SoundSys_AttachLipsync_8(&item_40, 0, 0);
}
