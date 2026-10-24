// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: SoundSys_EvaluateTrigger_11
// Retail VA: 0x004e8100
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_54d98c -> g_sound_evaluate_trigger_type
// Readable signature: int32_t SoundSys_EvaluateTrigger_11();

int32_t SoundSys_EvaluateTrigger_11()

{
    int32_t result;
    result = g_sound_evaluate_trigger_type;
    return result;
}
