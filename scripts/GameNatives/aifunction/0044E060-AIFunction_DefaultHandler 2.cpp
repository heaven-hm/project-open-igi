// IGI native provenance: AIFunction_DefaultHandler - 0x0044E060 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0044E060-AIFunction_DefaultHandler.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AIFunction_DefaultHandler.h"
int AIFunction_DefaultHandler(int context)

// Sig: void AIFunction_DefaultHandler()
// Note: The default AI event handler: call this for any AIEVENT_* your script
// does not handle explicitly (retail template shows it as the else-branch
// body). Takes no args. Put inside every event if-else chain as fallback.
{
    Qvm_Execute(*((*((*(g_5bda58 + 0x3d38) << 2) + &g_53c94c) << 2) + &g_58a9f4));
    int item_18 = 1;
    int item_10 = 0;
    int item_c = 0x3ff00000;
    char* item_8 = &g_567c74;
    memcpy(context, &item_18, 0x18);
    return context;
}
