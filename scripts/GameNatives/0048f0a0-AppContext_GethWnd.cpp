// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AppContext_GethWnd
// Retail VA: 0x0048f0a0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8bc4 -> g_app_window_handle
// Readable signature: int32_t AppContext_GethWnd();

int32_t AppContext_GethWnd()

{
    return g_app_window_handle;
}
