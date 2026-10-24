// Native: AppContext_SetLightmapsUsed
// Retail VA: 0x48f240
// Source: Binary Ninja MCP decompile
// Signature: int32_t(char arg1)

  int32_t AppContext_SetLightmapsUsed(char arg1)

// Sig: void App_ContextSetLightmapsUsed(int8_t used)
// Note: Enables/disables OBJECT lightmaps globally (global 0x5C8BE4) - the
// verified retail lightmap setter. Param: used = 1 use object lightmaps, 0
// flat-lit. Use for graphics enhancers (this is what IGIPatch's lightmap
// toggles call).
{
    int32_t result = arg1;
    g_5c8be4 = result;
    return result;
}
