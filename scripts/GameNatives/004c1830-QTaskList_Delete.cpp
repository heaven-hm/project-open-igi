// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTaskList_Delete
// Retail VA: 0x004c1830
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(void* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t QTaskList_Delete(void* task_list);

int32_t QTaskList_Delete(void* task_list)

{
    return Mem_DeAllocFn(task_list);
}
