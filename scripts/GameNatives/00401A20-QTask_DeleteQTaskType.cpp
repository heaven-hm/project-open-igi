// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTask_DeleteQTaskType
// Retail VA: 0x00401a20
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: uint32_t(int16_t arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_af5f05 -> g_task_type_state_table
// Readable signature: uint32_t QTask_DeleteQTaskType(int16_t type_id);

uint32_t QTask_DeleteQTaskType(int16_t type_id)

{
    uint32_t stream = type_id;
    uint32_t type_offset = stream * 0x30;
    stream = *(type_offset + 0xaf5f04);

    if (stream)
        stream = Sys_OpenFileStream_2(type_offset + 0xaf5eec);

    *(type_offset + &g_task_type_state_table) = 0;
    return stream;
}
