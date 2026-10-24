// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTask_LinkQTaskEventHandlerNoContextFn
// Retail VA: 0x00401530
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(uint16_t event_id, int32_t context, uint32_t flags)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_567c70 -> g_active_link_depth
//   g_536148 -> g_link_call_count
//   g_a96ae0 -> g_event_handler_table
//   g_af5ee8 -> g_task_type_table
// Readable signature: int32_t QTask_LinkQTaskEventHandlerNoContextFn(uint16_t event_id, int32_t context, uint32_t flags);

int32_t QTask_LinkQTaskEventHandlerNoContextFn(uint16_t event_id, int32_t context, uint32_t flags)

{
    int32_t new_depth = g_active_link_depth + 1;
    g_link_call_count += 1;
    g_active_link_depth = new_depth;
    int32_t context_offset = context * 0x180;
    int32_t handler_slot = event_id + context_offset;
    *((handler_slot << 2) + &g_event_handler_table) = flags;
    *(handler_slot + 0x54ff34) = new_depth == 1;
    int32_t context_index = 0;

    for (int16_t* task_type = &g_task_type_table; task_type < 0xafa6e8; )
    {
        if (*(task_type + 0x1d) && *task_type == event_id && !*(context_offset + context_index + 0x54ff34))
            QTask_LinkQTaskEventHandlerNoContextFn(context_index, context, flags);

        task_type = &task_type[0x18];
        context_index += 1;
    }

    QTask_DeleteTask();
    int32_t previous_depth = g_active_link_depth - 1;
    g_active_link_depth = previous_depth;
    return previous_depth;
}
