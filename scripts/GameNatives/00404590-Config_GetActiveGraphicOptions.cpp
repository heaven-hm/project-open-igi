// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Config_GetActiveGraphicOptions
// Retail VA: 0x00404590
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: void*()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_bc2384 -> g_active_profile_index
//   g_bc2394 -> g_graphic_options_base
// Readable signature: void* Config_GetActiveGraphicOptions();

void* Config_GetActiveGraphicOptions()

{
// Sig: GraphicOptions* Config_GetActiveGraphicOptions()
// Note: Returns a pointer to the ACTIVE PROFILE's graphic-options record inside
// the global config array. The record base is 0xBC2394 + 0xD14 *
// activeProfileIndex (verified in igi.exe). Useful for reading/writing raw
// graphic fields directly (gamma sits at record+0x220).
    return g_active_profile_index * 0x564 + &g_graphic_options_base;
}
