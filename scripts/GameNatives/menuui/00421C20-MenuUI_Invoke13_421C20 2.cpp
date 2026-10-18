// IGI native provenance: MenuUI_Invoke13_421C20 - 0x00421C20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00421C20-MenuUI_Invoke13_421C20.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuUI_Invoke13_421C20.h"

int MenuUI_Invoke13_421C20(void* arg1)

{
    (*(((Menu_PlayMusic_13() + StatusScreen_GetEvent() * 0x180) << 2) + &g_a96ae0))(arg1);
    Menu_UpdateLanguage_11(arg1);
    int* eax_5 = *(arg1 + 0x98);

    if (eax_5)
        Sys_AllocateMemory_18(eax_5);

    int result = Menu_PlayMusic_4(*(arg1 + 0x44));

    if (result)
    {
        int ecx_1 = 0;
        ecx_1 = *(Menu_PlayMusic_4(*(arg1 + 0x44)) + 0x1c);
        int eax_7 = StatusScreen_GetEvent();
        result = (*(((ecx_1 + eax_7 * 0x180) << 2) + &g_a96ae0))(Menu_PlayMusic_4(*(arg1 + 0x44)));
    }

    int* esi_4 = *(arg1 + 0x94);

    if (!esi_4)
        return result;

    return Sys_AllocateMemory_18(esi_4);
}
