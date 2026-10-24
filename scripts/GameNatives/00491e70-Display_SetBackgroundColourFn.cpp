// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Display_SetBackgroundColourFn
// Retail VA: 0x00491e70
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: uint32_t(char arg1, char arg2, char arg3)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_bcadf0 -> g_background_red
//   g_bcadf4 -> g_background_green
//   g_bcadf8 -> g_background_blue
// Readable signature: uint32_t Display_SetBackgroundColourFn(char red, char green, char blue);

uint32_t Display_SetBackgroundColourFn(char red, char green, char blue)

// Sig: void Display_SetBackgroundColourFn(int8_t r, int8_t g, int8_t b)
// Note: Sets the 3D-view BACKGROUND COLOUR (clear colour) - masks each arg to a
// byte into globals 0xBCADF0/R,G,B. Params: r,g,b = 0-255 colour channels. Use
// for custom sky/clear colour effects.
{
    uint32_t result = red;
    g_background_red = result;
    g_background_green = green;
    g_background_blue = blue;
    return result;
}
