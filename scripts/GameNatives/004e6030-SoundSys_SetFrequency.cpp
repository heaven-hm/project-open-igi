// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: SoundSys_SetFrequency
// Retail VA: 0x004e6030
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_limit_a -> g_sound_frequency_limit
// Readable signature: int32_t SoundSys_SetFrequency(int32_t frequency);

int32_t SoundSys_SetFrequency(int32_t frequency)

{
    g_sound_frequency_limit = frequency;
    return Timer_Commit(frequency);
}
