// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: DebugText_Clear
// Retail VA: 0x004e7bb0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_a5ea75 -> g_debug_text_active
//   g_a5ebd0 -> g_debug_text_handle
// Readable signature: int32_t DebugText_Clear();

int32_t DebugText_Clear()

{
    char enabled = g_debug_text_active;

    if (enabled)
        enabled = SoundSys_EvaluateTrigger_9(g_debug_text_handle);

    return enabled;
}
