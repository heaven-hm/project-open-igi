// IGI native provenance: Display_CheckCapabilities_4 - 0x00491D80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491D80-Display_CheckCapabilities_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_CheckCapabilities_4.hpp"
int Display_CheckCapabilities_4(int context)

{
    int item_c = Display_OpenDevice_5();
    int item_4 = context;
    void item_10;
    return Display_SyncVSync_14(&item_10);
}
