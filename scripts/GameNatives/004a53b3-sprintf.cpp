// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: sprintf
// Retail VA: 0x004a53b3
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(char* arg1, char* arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t sprintf(char* buffer, char* format);

int32_t sprintf(char* buffer, char* format)

// Sig: int Game_DataSymbolLoad(char* symBuf, char* symPath, char* symName)
// Note: Parses game-data SYMBOL DEFINITIONS from a buffer into the symbol
// table: tokenizes via 0x4A7720 with per-name limit 0x42 (66 bytes) and count
// cap 0x7FFFFFFF, null-terminates each parsed name. Params: symBuf = symbol
// buffer base, symPath = source path ref, symName = initial name entry. RETURNS
// parse status. Feeds the table that Symbol_Register*/GameData_SymbolRemove
// operate on.
{
    char* buffer_start = buffer;
    char* write_cursor = buffer;
    int32_t width_limit = 0x42;
    void va_storage;
    void* va_list_ptr = &va_storage;
    int32_t count_limit = 0x7fffffff;
    int32_t result = _output(&write_cursor, format);
    int32_t remaining_limit = count_limit - 1;

    if (count_limit - 1 < 0)
    {
        int32_t flush_result = 0;
        _flsbuf(&write_cursor);
    }
    else
        *write_cursor = 0;

    return result;
}
