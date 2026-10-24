// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTaskList_Register
// Retail VA: 0x004c1790
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t*(int32_t* arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t* QTaskList_Register(int32_t* task_list, int32_t task);

int32_t* QTaskList_Register(int32_t* task_list, int32_t task)

{
    int32_t count = *task_list;

    if (count < task_list[1])
    {
        task_list[count + 2] = task;
        *task_list += 1;
        return task_list;
    }

    Debug_Error("QTaskList is full");

    while (true)
        /* nop */
}
