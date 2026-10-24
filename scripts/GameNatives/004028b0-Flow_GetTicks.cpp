// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Flow_GetTicks
// Retail VA: 0x004028b0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: uint32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_frames_ctx -> g_frames_context
// Readable signature: uint32_t Flow_GetTicks();

uint32_t Flow_GetTicks()

// Sig: int Frames_GetUpdateCounter(void)
// Note: Returns the update-loop counter at game-context field +0x34. Frames_Set
// resets it and the retail main loop increments it once per simulation/update
// pass; gameplay timeout code compares against this value.
{
    return g_frames_context->update_counter;
}
