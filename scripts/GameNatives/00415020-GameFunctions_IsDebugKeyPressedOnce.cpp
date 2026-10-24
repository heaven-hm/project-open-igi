// Native: GameFunctions_IsDebugKeyPressedOnce
// Retail VA: 0x415020
// Source: Binary Ninja MCP decompile
// Signature: int32_t(void* arg1)

  int32_t GameFunctions_IsDebugKeyPressedOnce(void* arg1)

// Sig: void DEBUG_KEYSENABLEINVALID()
// Note: TOMBSTONE - DO NOT INVOKE. The retail debug-keys native address is
// UNKNOWN. The old value 0x0041502 lies outside .text (0x401000-0x532510) and
// would crash on invoke; no verified replacement found in igi.exe (unknown >
// fabricated per the local reverse-engineering acceptance policy). This entry
// exists only so JSON consumers see the warning instead of silently missing the
// key.
{
    if (*(arg1 + 0xc28c62) & 0x80 && !(*(arg1 + 0xc28d62) & 0x80) && g_c28c7f & 0x80
            && g_c28c8c & 0x80 && Config_GetActivePlayerProfile()
            && *(Config_GetActivePlayerProfile() + 0x560) >= 0xe && g_57b194)
        return 1;

    return 0;
}
