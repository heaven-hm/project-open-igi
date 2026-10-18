// IGI native provenance: Display_SyncVSync_13 - 0x00497570 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00497570-Display_SyncVSync_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SyncVSync_13.h"
int* Display_SyncVSync_13(void* context, int arg2)

{
    *(context + 8) = 0xffffffff;
    int item_18 = 0;
    int item_14 = 0;
    int item_10 = 0;
    int item_c = 0;
    Display_CheckCapabilities_13(context, &item_18);
    *(context + 0x40) = arg2;
    *(context + 0x44) = Mem_Alloc(arg2 * 0xc, 4);
    void* data_1 = Mem_Alloc(0xc, 4);
    int* temp_val = *(context + 0x44);
    *(context + 0x48) = data_1;
    Display_SwapBuffers_14(temp_val, arg2);
    return Display_SwapBuffers_14(*(context + 0x48), 1);
}
