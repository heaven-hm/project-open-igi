// IGI native provenance: Display_SetDisplayMode_10 - 0x00495F40 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00495F40-Display_SetDisplayMode_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetDisplayMode_10.h"
unsigned int __fastcall Display_SetDisplayMode_10(int context)

{
    int dwParam2 = context;
    unsigned int mciId = g_5ca130;
    dwParam2 = 0;
    unsigned int result = mciSendCommandA(mciId, 0x808, 0, &dwParam2);
    g_543918 = 0xffffffff;
    return result;
}
