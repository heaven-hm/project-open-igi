// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AppContext_SetTerrainLightmapsUsed
// Retail VA: 0x0048f260
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(char arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8be8 -> g_terrain_lightmaps_enabled
// Readable signature: int32_t AppContext_SetTerrainLightmapsUsed(char used);

int32_t AppContext_SetTerrainLightmapsUsed(char used)

// Sig: void App_ContextSetTerrainLightmapsUsed(int8_t used)
// Note: Enables/disables TERRAIN lightmaps globally (global 0x5C8BE8). Param:
// used = 1 lit terrain, 0 unlit. Pair with AppContext_SetLightmapsUsed for full
// lighting control.
{
    int32_t value = used;
    g_terrain_lightmaps_enabled = value;
    return value;
}
