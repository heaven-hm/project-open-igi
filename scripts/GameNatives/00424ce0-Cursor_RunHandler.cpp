// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Cursor_RunHandler
// Retail VA: 0x00424ce0
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(void* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_bc20f8 -> g_mouse_delta_x
//   g_bc20f4 -> g_mouse_delta_y
//   g_c28b44 -> g_screen_width
//   g_c28b48 -> g_screen_height
//   g_57bc58 -> g_cursor_x
//   g_57bc5c -> g_cursor_y
//   g_c28f8c -> g_mouse_buttons
// Readable signature: int32_t Cursor_RunHandler(void* cursor);

int32_t Cursor_RunHandler(void* cursor)

{
    int32_t x_clamp;
    void* device;
    int32_t x_pos;

    if (FX_FragmentSimulate_23())
    {
        long double delta_x = g_mouse_delta_x;
        device = cursor;
        int32_t prev_x = *(device + 0x28);
        *(device + 0x24) += g_mouse_delta_y * 320f;
        bool has_y = *(device + 0x24) >= 0;
        *(device + 0x28) = prev_x + delta_x * 240f;
        int32_t x_clamp_tmp;

        if (has_y)
        {
            int32_t x_pos_tmp = *(device + 0x24);

            if (g_screen_width - 1 >= x_pos_tmp)
                x_clamp_tmp = x_pos_tmp;
            else
                x_clamp_tmp = g_screen_width - 1;
        }
        else
            x_clamp_tmp = 0;

        *(device + 0x24) = x_clamp_tmp;

        if (*(device + 0x28) >= 0)
        {
            x_pos = *(device + 0x28);

            if (g_screen_height - 1 >= x_pos)
                x_clamp = x_pos;
            else
                x_clamp = g_screen_height - 1;
        }
        else
            x_clamp = 0;
    }
    else
    {
        device = cursor;
        int32_t reset_x = g_cursor_x;
        *(device + 0x24) = reset_x;
        *(device + 0x28) = g_cursor_y;
        int32_t x_clamp_1;

        if (reset_x >= 0)
        {
            int32_t x_pos_1 = *(device + 0x24);

            if (g_screen_width - 1 >= x_pos_1)
                x_clamp_1 = x_pos_1;
            else
                x_clamp_1 = g_screen_width - 1;
        }
        else
            x_clamp_1 = 0;

        *(device + 0x24) = x_clamp_1;

        if (*(device + 0x28) >= 0)
        {
            x_pos = *(device + 0x28);

            if (g_screen_height - 1 >= x_pos)
                x_clamp = x_pos;
            else
                x_clamp = g_screen_height - 1;
        }
        else
            x_clamp = 0;
    }
    int32_t prev_button_state = *(device + 0x3c);
    *(device + 0x28) = x_clamp;
    *(device + 0x38) = prev_button_state;
    int32_t mouse_button_state = *g_mouse_buttons & 1;
    *(device + 0x3c) = mouse_button_state;
    return mouse_button_state;
}
