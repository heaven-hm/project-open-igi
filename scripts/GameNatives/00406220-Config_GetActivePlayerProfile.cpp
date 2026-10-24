// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Config_GetActivePlayerProfile
// Retail VA: 0x00406220
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: void*()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_bc2384 -> g_active_profile_index
//   g_bc2388 -> g_player_profile_base
// Readable signature: void* Config_GetActivePlayerProfile();

void* Config_GetActivePlayerProfile()

{
// Sig: ProfileRecord* Config_GetActivePlayerProfileRecord()
// Note: RETURNS pointer to the ACTIVE PROFILE's graphic-options/config record:
// computed as 0xBC2388 + 0xD14*[0xBC2384] (verified against retail
// defaults-reset fcn.00403B70 which walks the same layout). Takes no args.
// Despite the historical name this returns a record pointer, not a bool. Old
// analyst name IsPlayerProfileActive was wrong - it returns the record pointer,
// not a bool.
    return &(&g_player_profile_base)[g_active_profile_index * 0x159];
}
