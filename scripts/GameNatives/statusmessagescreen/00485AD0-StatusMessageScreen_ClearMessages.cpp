// IGI native provenance: StatusMessageScreen_ClearMessages - 0x00485AD0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 00485AD0-StatusMessageScreen_ClearMessages.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "StatusMessageScreen_ClearMessages.hpp"
void StatusMessageScreen_ClearMessages()

// Sig: void StatusMessageScreen_ClearMessages(void)
// Note: Clears all on-screen status messages. Takes no args.
{
    StatusList_Clear(*(StatusScreen_Get() + 0x4e4));
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    int arg_4;
    memcpy(arg_4, &item_18, 0x18);
}
