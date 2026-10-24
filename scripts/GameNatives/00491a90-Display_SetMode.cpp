// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: Display_SetMode
// Retail VA: 0x00491a90
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: int32_t(void* arg1)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.
// Global identifier map (BN -> readable):
//   g_5ca02d -> g_device_open
//   g_bcadec -> g_active_window
//   g_5c9fa8 -> g_window_title
//   g_c28b5c -> g_screen_title
//   g_c28b44 -> g_screen_width
//   g_c28b48 -> g_screen_height
//   g_c28b50 -> g_screen_bpp
//   g_c28b58 -> g_screen_refresh
//   g_c28b56 -> g_screen_aspect
// Readable signature: int32_t Display_SetMode(void* mode_struct);

int32_t Display_SetMode(void* mode_struct)

// Sig: int Display_SetMode(void* modeStruct)
// Note: Switches the DISPLAY MODE at runtime: validates mode fields vs current
// (0xC28B44...), repositions/resizes the window (GetSystemMetrics), copies the
// mode into the active-mode struct. Param: modeStruct = pointer to a
// display-mode struct (same layout Display_GetActiveMode returns). RETURNS 0 on
// failure, non-zero on success.
{
    int32_t result;

    if (!g_device_open)
    {
    do_open:
        Display_CloseDevice_4(g_active_window);

        if (g_device_open)
            Display_OpenDevice_4(mode_struct);

        RECT window_rect;

        if (!FX_FragmentSimulate_23())
        {
            int32_t screen_w = GetSystemMetrics(SM_CXSCREEN);
            int32_t mode_h = *(mode_struct + 8);
            int32_t high_d;
            int32_t low_d;
            high_d = HIGHD(screen_w - *(mode_struct + 4));
            low_d = LOWD(screen_w - *(mode_struct + 4));
            window_rect.left = (low_d - high_d) >> 1;
            int32_t high_d2;
            int32_t low_d2;
            high_d2 = HIGHD(GetSystemMetrics(SM_CYSCREEN) - mode_h);
            low_d2 = LOWD(GetSystemMetrics(SM_CYSCREEN) - mode_h);
            int32_t centered_top = (low_d2 - high_d2) >> 1;
            int32_t right_edge = window_rect.left + *(mode_struct + 4);
            window_rect.top = centered_top;
            window_rect.right = right_edge;
            window_rect.bottom = mode_h + centered_top;
        }
        else
        {
            window_rect.left = 0;
            window_rect.top = 0;
            window_rect.right = GetSystemMetrics(SM_CXSCREEN);
            window_rect.bottom = GetSystemMetrics(SM_CYSCREEN);
        }

        AdjustWindowRect(&window_rect, GetWindowLongA(AppContext_GethWnd(), 0xfffffff0), 0);
        LONG win_top = window_rect.top;
        LONG win_left = window_rect.left;
        SetWindowPos(AppContext_GethWnd(), nullptr, win_left, win_top, window_rect.right - win_left,
            window_rect.bottom - win_top, 0x44);
        UpdateWindow(AppContext_GethWnd());
        SetFocus(AppContext_GethWnd());
        Sys_FreeMemory_3(*(mode_struct + 0x18));
        (*((Display_SwapBuffers_5() << 2) + &g_a94e84))(mode_struct);
        g_device_open = 1;
        int32_t i = 0xffffffff;
        int32_t title_cursor = 0xc28b5c;
        result = 0;

        while (i)
        {
            bool cond_4 = 0 != *title_cursor;
            title_cursor += 1;
            i -= 1;

            if (!cond_4)
                break;
        }

        int32_t title_len = ~i;
        int32_t copy_src;
        int32_t copy_dst;
        copy_dst = __builtin_memcpy(&g_window_title, title_cursor - title_len, title_len & 0xfffffffc);
        __builtin_memcpy(copy_dst, copy_src, title_len & 3);
    }
    else
    {
        int32_t i_1 = 0xffffffff;
        char* name_cursor = mode_struct + 0x1c;

        while (i_1)
        {
            bool cond_1 = 0 != *name_cursor;
            name_cursor = &name_cursor[1];
            i_1 -= 1;

            if (!cond_1)
                break;
        }

        if (~i_1 == 1)
        {
            char* stored_cursor = &g_window_title;
            int32_t i_2 = 0xffffffff;

            while (i_2)
            {
                bool cond_3 = 0 != *stored_cursor;
                stored_cursor = &stored_cursor[1];
                i_2 -= 1;

                if (!cond_3)
                    break;
            }

            int32_t stored_len = ~i_2;
            int32_t copy_src_2;
            int32_t copy_dst_2;
            copy_dst_2 = __builtin_memcpy(mode_struct + 0x1c, stored_cursor - stored_len, stored_len & 0xfffffffc);
            __builtin_memcpy(copy_dst_2, copy_src_2, stored_len & 3);
        }

        if (*(mode_struct + 4) != g_screen_width || *(mode_struct + 8) != g_screen_height || *(mode_struct + 0x10) != g_screen_bpp
                || *(mode_struct + 0x18) != g_screen_refresh || *(mode_struct + 0x16) != g_screen_aspect)
            goto do_open;

        char* stored_cursor = &g_screen_title;
        char* name_cursor_2 = mode_struct + 0x1c;

        while (true)
        {
            char name_ch = *name_cursor_2;
            char stored_ch = *stored_cursor;
            bool less = name_ch < stored_ch;

            if (name_ch == stored_ch)
            {
                if (!name_ch)
                {
                    result = 0;
                    break;
                }

                name_ch = name_cursor_2[1];
                char stored_ch2 = stored_cursor[1];
                less = name_ch < stored_ch2;

                if (name_ch == stored_ch2)
                {
                    name_cursor_2 = &name_cursor_2[2];
                    stored_cursor = &stored_cursor[2];

                    if (name_ch)
                        continue;

                    result = 0;
                    break;
                }
            }

            result = name_cursor_2 - name_cursor_2 + 1;
            break;
        }

        if (result)
            goto do_open;
    }

    return result;
}
