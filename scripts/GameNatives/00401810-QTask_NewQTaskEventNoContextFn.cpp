// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTask_NewQTaskEventNoContextFn
// Retail VA: 0x00401810
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int16_t arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_ai_anim_names -> g_event_slot_names
//   g_ai_anim_table -> g_event_slot_table
//   g_54ff24 -> g_event_slot_scan_limit
// Readable signature: int32_t QTask_NewQTaskEventNoContextFn(int16_t context);

int32_t QTask_NewQTaskEventNoContextFn(int16_t context)

{
    int32_t event_slot = 0x20;
    char* name_entry = &g_event_slot_names;

    do
    {
        if (!*name_entry)
        {
            int32_t slot_offset = event_slot * 6;
            *(slot_offset + &g_event_slot_table) = 1;
            *(slot_offset + 0x54f930) = context;
            *(slot_offset + 0x54f933) = 0;
            return event_slot;
        }

        name_entry = &name_entry[6];
        event_slot += 1;
    } while (name_entry < &*(g_event_slot_scan_limit + 2));

    Debug_Error("Unable to allocate event");

    while (true)
        /* nop */
}
