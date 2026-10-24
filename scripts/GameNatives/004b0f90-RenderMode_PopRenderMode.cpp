// Native: RenderMode_PopRenderMode
// Retail VA: 0x4b0f90
// Source: Binary Ninja MCP decompile
// Signature: int32_t()

  int32_t RenderMode_PopRenderMode()

{
    int32_t result = *((g_942304 << 2) + &g_942308);

    if (result == 0xffffffff)
    {
        g_a94a64 = 0xffffffff;
        return 0xffffffff;
    }

    g_942304 += 1;
    Sys_FreeMemory_3(result);
    return result;
}
