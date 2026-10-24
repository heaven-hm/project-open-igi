// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: TextWindow_vprintf
// Retail VA: 0x004e76e0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t* arg1, char* arg2, int32_t arg3)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t TextWindow_vprintf(int32_t* text_window, char* format, int32_t args);

int32_t TextWindow_vprintf(int32_t* text_window, char* format, int32_t args)

{
    // Shared format buffer at 0xa5ea78.
    String_VFormat(0xa5ea78, format, args);
    return Console_AppendText(text_window, 0xa5ea78);
}
