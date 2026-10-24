// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTask_DeleteQTaskEventNoContext
// Retail VA: 0x004018e0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: uint32_t(int32_t arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: uint32_t QTask_DeleteQTaskEventNoContext(int32_t context);

uint32_t QTask_DeleteQTaskEventNoContext(int32_t context)

{
    return QTask_InitContext(context);
}
