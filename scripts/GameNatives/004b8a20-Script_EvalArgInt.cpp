// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Script_EvalArgInt
// Retail VA: 0x004b8a20
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(float arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Readable signature: int32_t Script_EvalArgInt(float script_arg, int32_t arg_index);

int32_t Script_EvalArgInt(float script_arg, int32_t arg_index)

{
    if (!*(script_arg + 4))
        return Sys_ReadFileBlock_24(*(*(script_arg + 8) + (arg_index << 2)));

    int32_t unused_value;
    return Sys_AllocateMemory_28(script_arg, unused_value, arg_index, script_arg, arg_index);
}
