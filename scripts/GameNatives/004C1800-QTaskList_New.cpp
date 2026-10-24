// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTaskList_New
// Retail VA: 0x004c1800
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t*(int32_t arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t* QTaskList_New(int32_t capacity);

int32_t* QTaskList_New(int32_t capacity)

{
    int32_t* task_list = Mem_AllocFn((capacity << 2) + 8, 4);
    task_list[1] = capacity;
    *task_list = 0;
    return task_list;
}
