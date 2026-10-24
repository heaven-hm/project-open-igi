// Native: BackgroundFX_New
// Retail VA: 0x4199d0
// Source: Binary Ninja MCP decompile
// Signature: struct status_msg*(void* arg1)

  struct status_msg* BackgroundFX_New(void* arg1)

{
    int32_t kind;
    kind = g_539af0;
    return QTask_HandleInput(arg1, kind, 0);
}
