// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: DebugText_SetCursor
// Retail VA: 0x004e7b80
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_a5ea75 -> g_debug_text_active
//   g_a5ebd0 -> g_debug_text_handle
// Readable signature: int32_t DebugText_SetCursor(int32_t x, int32_t y);

int32_t DebugText_SetCursor(int32_t x, int32_t y)

{
    char enabled = g_debug_text_active;

    if (!enabled)
        return enabled;

    return SoundSys_LoadSoundDef_9(g_debug_text_handle, x, y);
}
