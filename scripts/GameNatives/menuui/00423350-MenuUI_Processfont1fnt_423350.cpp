// IGI native provenance: MenuUI_Processfont1fnt_423350 - 0x00423350 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00423350-MenuUI_Processfont1fnt_423350.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00423350-MenuUI_Processfont1fnt_423350.hpp"
int MenuUI_Processfont1fnt_423350(void* arg1)

{
    int* esi_1 = arg1 + 0x154;
    int i_1 = 0x1b;
    int i;

    do
    {
        int* eax_1 = esi_1[-0x40];

        if (eax_1)
        {
            Sys_ReadFileBlock_18(eax_1);
            esi_1[-0x40] = 0;
        }

        int* eax_2 = *esi_1;

        if (eax_2)
        {
            Sys_ReadFileBlock_18(eax_2);
            *esi_1 = 0;
        }

        esi_1 = &esi_1[1];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    int eax_3 = Script_SpawnNewTask_6();
    int context_val = 0;
    context_val = *(arg1 + 0x1c);
    return (*(((eax_3 * 0x180 + context_val) << 2) + &g_a96ae0))(arg1);
}
