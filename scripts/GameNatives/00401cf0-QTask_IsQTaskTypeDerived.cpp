// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: QTask_IsQTaskTypeDerived
// Retail VA: 0x00401cf0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(int32_t arg1, int16_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_af5ee8 -> g_task_type_table
// Readable signature: int32_t QTask_IsQTaskTypeDerived(int32_t type_id, int16_t base_type_id);

int32_t QTask_IsQTaskTypeDerived(int32_t type_id, int16_t base_type_id)

// Sig: bool TaskType_IsDerivedFrom(uint16_t typeId, uint16_t baseTypeId)
// Note: Checks equality and walks the task-type parent chain until the sentinel
// type. Ghidra/r2 CFG agreement: 5 basic blocks and 6 edges; see
// assets/IGI1-Native-Name-Evidence.json.
{
    int32_t current_type = type_id;

    while (current_type != base_type_id)
    {
        current_type = (current_type & 0xffff) * 0x30;
        current_type = *(current_type + &g_task_type_table);

        if (current_type == 0x181)
        {
            current_type = 0;
            return current_type;
        }
    }

    current_type = 1;
    return current_type;
}
