// IGI native provenance: Display_SetBackgroundColourFn - 0x00491E70 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00491E70-Display_SetBackgroundColourFn.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetBackgroundColourFn.h"
unsigned int Display_SetBackgroundColourFn(char context, char arg2, char arg3)

// Sig: void Display_SetBackgroundColourFn(char r, char g, char b)
// Note: Sets the 3D-view BACKGROUND COLOUR (clear colour) - masks each arg to a
// byte into globals 0xBCADF0/R,G,B. Params: r,g,b = 0-255 colour channels. Use
// for custom sky/clear colour effects.
{
    unsigned int result = context;
    g_bcadf0 = result;
    g_bcadf4 = arg2;
    g_bcadf8 = arg3;
    return result;
}
