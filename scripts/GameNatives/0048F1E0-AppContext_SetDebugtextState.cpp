// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AppContext_SetDebugtextState
// Retail VA: 0x0048f1e0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(char arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8bf4 -> g_debug_text_enabled
// Readable signature: int32_t AppContext_SetDebugtextState(char state);

int32_t AppContext_SetDebugtextState(char state)

// Sig: void App_ContextSetDebugtextState(int8_t state)
// Note: Toggles on-screen DEBUG TEXT output (global 0x5C8BF4). Param: state = 1
// show engine debug text, 0 hide. Handy while reverse-engineering other
// systems.
{
    int32_t value = state;
    g_debug_text_enabled = value;
    return value;
}
