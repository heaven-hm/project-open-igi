// IGI native provenance: Menu_PlayMusic_12 - 0x00423810 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00423810-Menu_PlayMusic_12.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Menu_PlayMusic_12.h"
int Menu_PlayMusic_12(int context, int arg2, int arg3, int arg4)

{
    int item_4 = arg3;
    int data_val = Config_SetResolution();
    int* source_val = Config_SetGamma();
    Config_Moveleft(arg4, &item_4, nullptr);
    int result = 0;

    if (data_val > 0)
    {
        do
        {
            if (*source_val == item_4)
                return result;

            source_val = &source_val[3];
            result += 1;
        } while (result < data_val);
    }

    return 0xffffffff;
}
