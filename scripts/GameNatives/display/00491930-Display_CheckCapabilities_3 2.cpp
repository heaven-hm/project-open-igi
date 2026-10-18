// IGI native provenance: Display_CheckCapabilities_3 - 0x00491930 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491930-Display_CheckCapabilities_3.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CheckCapabilities_3.h"
int Display_CheckCapabilities_3()

{
    if (g_5ca02d)
        Display_OpenDevice_4(0);

    int item_8 = Display_SyncVSync_5();
    int item_4 = 0;
    Sys_AllocateMemory_3(sub_491910, &item_8);
    Display_RestoreDevice_7();
    int result = Display_OpenDevice_16();
    g_5ca02c = 0;
    return result;
}
