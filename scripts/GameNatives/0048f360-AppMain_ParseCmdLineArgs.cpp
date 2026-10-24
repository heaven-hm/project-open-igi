// Verified: 100% human-verified by BlankNameES (EU native set, scripts/GameNatives/EU_NATIVES_BN_COPY.md).
// Native: AppMain_ParseCmdLineArgs
// Retail VA: 0x0048f360
// Source: Binary Ninja MCP decompile (bn_function_decompile, Pseudo C)
// BN signature: void*(char* arg1, int32_t arg2)
// Readability pass: identifier names only; control flow, operators, constants, call order and memory offsets are unchanged from BN Pseudo C.

// Readability note: callback is the original variadic-style handler pointed to by
// arg 2 (BN shows `arg2(~i_5 - 1 + result_2)`); prefix is matched
// case-insensitively against each whitespace-separated token before invoking it.
// Readable signature: void* AppMain_ParseCmdLineArgs(char* prefix, int32_t callback);

void* AppMain_ParseCmdLineArgs(char* prefix, int32_t callback)

{
    int32_t string_ptr = FX_DebrisSpawn_21();
    int32_t remaining_len = 0xffffffff;

    while (remaining_len)
    {
        bool has_char_0 = 0 != *string_ptr;
        string_ptr += 1;
        remaining_len -= 1;

        if (!has_char_0)
            break;
    }

    int32_t copy_len = ~remaining_len;
    void token_buffer;
    int32_t dest_ptr;
    int32_t copy_end;
    copy_end = __builtin_memcpy(&token_buffer, string_ptr - copy_len, copy_len & 0xfffffffc);
    __builtin_memcpy(copy_end, dest_ptr, copy_len & 3);
    void* first_token = Camera_SetFieldOfView_6(&token_buffer, " \t");
    void* current_token = first_token;
    void* first_token_copy = current_token;

    if (current_token)
    {
        while (true)
        {
            void* scan_cursor = current_token;
            int32_t token_len = 0xffffffff;

            while (token_len)
            {
                bool has_char_2 = 0 != *scan_cursor;
                scan_cursor += 1;
                token_len -= 1;

                if (!has_char_2)
                    break;
            }

            char* arg_cursor_1 = prefix;
            int32_t arg_len = 0xffffffff;

            while (arg_len)
            {
                bool has_char_3 = 0 != *arg_cursor_1;
                arg_cursor_1 = &arg_cursor_1[1];
                arg_len -= 1;

                if (!has_char_3)
                    break;
            }

            if (~token_len - 1 >= ~arg_len - 1)
            {
                char* arg_cursor_2 = prefix;
                int32_t arg_len_2 = 0xffffffff;
                int32_t char_index = 0;
                char matches = 1;

                while (arg_len_2)
                {
                    bool has_char_4 = 0 != *arg_cursor_2;
                    arg_cursor_2 = &arg_cursor_2[1];
                    arg_len_2 -= 1;

                    if (!has_char_4)
                        break;
                }

                if (~arg_len_2 - 1 <= 0)
                    goto invoke_callback;

                char* cmp_cursor = prefix;
                int32_t len_probe;

                do
                {
                    if (!matches)
                        goto next_token;

                    char token_ch = *(current_token - prefix + cmp_cursor);
                    char arg_ch = *cmp_cursor;

                    if (token_ch >= 0x61 && token_ch <= 0x7a)
                        token_ch -= 0x20;

                    if (arg_ch >= 0x61 && arg_ch <= 0x7a)
                        arg_ch -= 0x20;

                    if (token_ch != arg_ch)
                        matches = 0;

                    char_index += 1;
                    char* arg_cursor_3 = prefix;
                    len_probe = 0xffffffff;
                    cmp_cursor = &cmp_cursor[1];

                    while (len_probe)
                    {
                        bool has_char_6 = 0 != *arg_cursor_3;
                        arg_cursor_3 = &arg_cursor_3[1];
                        len_probe -= 1;

                        if (!has_char_6)
                            break;
                    }
                } while (char_index < ~len_probe - 1);

                if (matches)
                {
                    current_token = first_token_copy;
                invoke_callback:
                    char* arg_cursor_4 = prefix;
                    int32_t arg_len_3 = 0xffffffff;

                    while (arg_len_3)
                    {
                        bool has_char_5 = 0 != *arg_cursor_4;
                        arg_cursor_4 = &arg_cursor_4[1];
                        arg_len_3 -= 1;

                        if (!has_char_5)
                            break;
                    }

                    callback(~arg_len_3 - 1 + current_token);
                }
            }

        next_token:
            first_token = Camera_SetFieldOfView_6(nullptr, " \t");
            first_token_copy = first_token;

            if (!first_token)
                break;

            current_token = first_token_copy;
        }
    }

    return first_token;
}
