// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Mouse_Update
// Retail VA: 0x0048fc20
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t()
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5c8e1c -> g_mouse_device
//   g_c28f84 -> g_mouse_x
//   g_c28f88 -> g_mouse_y
//   g_c28f8c -> g_mouse_buttons
//   g_5c8e10 -> g_mouse_button_1
//   g_5c8e0c -> g_mouse_button_2
//   g_5c8e18 -> g_mouse_button_3
//   g_5c8e08 -> g_mouse_wheel

// Readability note: mouse_x_raw / mouse_y_raw are converted with 1/255
// (0.0039215686274509803). button_0..button_7 are the eight per-button bytes
// packed into the low byte of button_state; wheel bits 0x100/0x200 are set
// separately.
// Readable signature: int32_t Mouse_Update();

int32_t Mouse_Update()

{
    int32_t* mouse_device = g_mouse_device;
    int32_t button_state = 0;
    int32_t mouse_x_raw;
    int32_t wheel_delta;

    if (!(*(*mouse_device + 0x24))(mouse_device, 0x14, &mouse_x_raw))
    {
    read_buttons:
        g_mouse_x = mouse_x_raw * 0.0039215686274509803;
        int32_t mouse_y_raw;
        g_mouse_y = mouse_y_raw * 0.0039215686274509803;

        if (!FX_SmokeEmit_25())
        {
            char button_0;

            if (button_0)
                button_state = 1;

            char button_1;

            if (button_1)
                button_state |= 2;

            char button_2;

            if (button_2)
                button_state |= 4;

            char button_3;

            if (button_3)
                button_state |= 8;

            char button_4;

            if (button_4)
                button_state |= 0x10;

            char button_5;

            if (button_5)
                button_state |= 0x20;

            char button_6;

            if (button_6)
                button_state |= 0x40;

            char button_7;

            if (button_7)
                button_state |= 0x80;

            int32_t wheel_tmp;
            wheel_delta = wheel_tmp;

            if (wheel_delta > 0)
            {
                *g_mouse_buttons = button_state | 0x100;
                return wheel_delta;
            }

            if (wheel_delta < 0)
                button_state |= 0x200;
        }
        else
        {
            if (g_mouse_button_1)
                button_state = 1;

            if (g_mouse_button_2)
                button_state |= 2;

            if (g_mouse_button_3)
                button_state |= 4;

            wheel_delta = g_mouse_wheel;
            bool wheel_positive = wheel_delta >= 0;

            if (wheel_delta > 0)
            {
                button_state |= 0x100;
                wheel_delta -= 1;
                g_mouse_wheel = wheel_delta;
                wheel_positive = wheel_delta >= 0;
            }

            if (!wheel_positive)
            {
                g_mouse_wheel = wheel_delta + 1;
                *g_mouse_buttons = button_state | 0x200;
                return wheel_delta + 1;
            }
        }

        *g_mouse_buttons = button_state;
    }
    else
    {
        int32_t* mouse_device_1 = g_mouse_device;
        wheel_delta = (*(*mouse_device_1 + 0x1c))(mouse_device_1);

        if (!wheel_delta)
        {
            int32_t* mouse_device_2 = g_mouse_device;
            int32_t state_result = (*(*mouse_device_2 + 0x24))(mouse_device_2, 0x14, &mouse_x_raw);

            if (state_result)
            {
                int32_t error_code = DIERR_GENERIC(state_result);
                Debug_Error("Failed get mouse state: %s");

                while (true)
                    /* nop */
            }

            goto read_buttons;
        }
    }

    return wheel_delta;
}
