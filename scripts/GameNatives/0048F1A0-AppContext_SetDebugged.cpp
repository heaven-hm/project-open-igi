// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AppContext_SetDebugged
// Retail VA: 0x0048f1a0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(char arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8bf8 -> g_debugged_flag
// Readable signature: int32_t AppContext_SetDebugged(char state);

int32_t AppContext_SetDebugged(char state)

// Sig: void App_ContextSetDebugged(int8_t state)
// Note: Sets the app-level DEBUGGED flag (global 0x5C8BF8). Param: state = 1
// mark debugger attached/debug mode, 0 clear. BlankName's IGIPatch calls this
// by name. Use to toggle engine debug paths.
{
    int32_t value = state;
    g_debugged_flag = value;
    return value;
}
