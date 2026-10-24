// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: GameFunctions_IsDebugKeyPressed
// Retail VA: 0x00414fd0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(void* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_c28c7f -> g_debug_key_byte_1
//   g_c28c8c -> g_debug_key_byte_2
//   g_57b194 -> g_debug_key_gate
// Readable signature: int32_t GameFunctions_IsDebugKeyPressed(void* context);

int32_t GameFunctions_IsDebugKeyPressed(void* context)

{
    if (*(context + 0xc28c62) & 0x80 && g_debug_key_byte_1 & 0x80 && g_debug_key_byte_2 & 0x80
            && Config_GetActivePlayerProfile() && *(Config_GetActivePlayerProfile() + 0x560) >= 0xe
            && g_debug_key_gate)
        return 1;

    return 0;
}
