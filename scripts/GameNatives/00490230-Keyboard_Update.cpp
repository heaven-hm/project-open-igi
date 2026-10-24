// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Keyboard_Update
// Retail VA: 0x00490230
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8e30 -> g_keyboard_initialized
//   g_5c8e24 -> g_input_device
//   g_c28c60 -> g_keyboard_no_input
// Readable signature: int32_t Keyboard_Update();

int32_t Keyboard_Update()

{
    if (g_keyboard_initialized)
        goto read_input;

    int32_t* input_device = g_input_device;
    int32_t status = (*(*input_device + 0x1c))(input_device);

    if (!status)
    {
        g_keyboard_initialized = 1;
    read_input:
        // Keyboard state buffers at 0xc28c62 (current), 0xc28d62 (previous)
        // and 0xc28e80 (alternate/repeat state).
        __builtin_memcpy(0xc28d62, 0xc28c62, 0x100);

        if (g_keyboard_no_input)
        {
            status = 0;
            __builtin_memset(0xc28c62, 0, 0x100);
        }
        else
        {
            int32_t smoke_result = FX_SmokeEmit_25();

            if (smoke_result)
            {
                __builtin_memcpy(0xc28c62, 0xc28e80, 0x100);
                return smoke_result;
            }

            int32_t* input_device_2 = g_input_device;
            status = (*(*input_device_2 + 0x24))(input_device_2, 0x100, 0xc28c62);

            if (status)
            {
                int32_t* input_device_3 = g_input_device;

                if ((*(*input_device_3 + 0x1c))(input_device_3))
                {
                    status = 0;
                    __builtin_memset(0xc28c62, 0, 0x100);
                }
                else
                {
                    int32_t* input_device_4 = g_input_device;
                    status = (*(*input_device_4 + 0x24))(input_device_4, 0x100, 0xc28c62);

                    if (status)
                    {
                        status = 0;
                        __builtin_memset(0xc28c62, 0, 0x100);
                    }
                }
            }
        }
    }

    return status;
}
