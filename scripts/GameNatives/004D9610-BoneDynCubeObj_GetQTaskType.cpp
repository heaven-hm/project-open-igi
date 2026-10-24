// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: BoneDynCubeObj_GetQTaskType
// Retail VA: 0x004d9610
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_548618 -> g_bone_dyncube_qtask_type
// Readable signature: int32_t BoneDynCubeObj_GetQTaskType();

int32_t BoneDynCubeObj_GetQTaskType()

{
    int32_t result;
    result = g_bone_dyncube_qtask_type;
    return result;
}
