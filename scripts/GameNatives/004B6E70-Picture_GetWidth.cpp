// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Picture_GetWidth
// Retail VA: 0x004b6e70
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t Picture_GetWidth(int32_t* picture);

int32_t Picture_GetWidth(int32_t* picture)

// Sig: int Picture_GetWidth(void* picture)
// Note: RETURNS the WIDTH in pixels of a loaded picture/surface. Param: picture
// = pointer from a picture-load call. Thin thunk to fcn.004B7DF0
// (byte-verified).
{
    return Sys_CheckHeap_21(picture);
}
