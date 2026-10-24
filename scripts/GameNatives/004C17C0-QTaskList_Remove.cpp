// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTaskList_Remove
// Retail VA: 0x004c17c0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t* arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t QTaskList_Remove(int32_t* task_list, int32_t task);

int32_t QTaskList_Remove(int32_t* task_list, int32_t task)

{
    int32_t index = 0;
    int32_t count = *task_list;

    if (count > 0)
    {
        int32_t* entry = &task_list[2];

        do
        {
            if (*entry == task)
            {
                *task_list = count - 1;
                task_list[index + 2] = task_list[count + 1];
                return index;
            }

            index += 1;
            entry = &entry[1];
        } while (index < count);
    }

    Debug_Error("Trying to remove unrigistered QTask from QTaskList");

    while (true)
        /* nop */
}
