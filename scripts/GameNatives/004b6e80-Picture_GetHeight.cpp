// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Picture_GetHeight
// Retail VA: 0x004b6e80
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t Picture_GetHeight(int32_t* picture);

int32_t Picture_GetHeight(int32_t* picture)

// Sig: int Picture_GetHeight(void* picture)
// Note: RETURNS the HEIGHT in pixels of a loaded picture/surface. Param:
// picture = picture pointer. Thin thunk to fcn.004B7E00.
{
    return Sys_AllocateMemory_22(picture);
}
