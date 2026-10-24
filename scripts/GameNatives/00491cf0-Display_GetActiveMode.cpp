// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Display_GetActiveMode
// Retail VA: 0x00491cf0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t() __pure
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t Display_GetActiveMode();

int32_t Display_GetActiveMode() __pure

// Sig: void* Display_GetActiveMode()
// Note: RETURNS pointer to the ACTIVE display-mode struct (global 0xC28B40):
// width/height/depth fields live at +0x4/+0x8/+0x10 per Display_SetMode
// comparisons. Takes no args. Read current resolution from here.
{
    return 0xc28b40;
}
