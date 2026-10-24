// Verified: 100% manually verified by HeavenHM (project-igi-natives/IGI_Natives/IGI-Natives.json).
// Native: Debug_Error
// Retail VA: 0x4af7b0
// Source: Binary Ninja MCP decompile
// Signature: int32_t(char* arg1)

  int32_t Debug_Error(char* arg1)

// Sig: void Error_Show(char* format, ...)
// Note: Shows a fatal-error dialog: printf-style FORMAT STRING + variadic args
// (e.g. push "Level_Load(): Couldn't load script: %s"; push scriptPath; call).
// Param: format = printf-style string, followed by substitution args. Variadic
// - proven by retail call sites (add esp,8 cleanup) and vsprintf helper
// fcn.004A5702.
{
    if (g_936268)
    {
        sprintf(0xa84640, "Fatal error:\n\n");
        int32_t edi_1 = 0xa84640;
        int32_t i = 0xffffffff;

        while (i)
        {
            bool cond:0_1 = 0 != *edi_1;
            edi_1 += 1;
            i -= 1;

            if (!cond:0_1)
                break;
        }

        void arg_8;
        String_VFormat(~i + 0xa8463f, arg1, &arg_8);
        g_936268(0xa84640);
    }

    int32_t 93626c_1 = g_93626c;

    if (!93626c_1)
        return 93626c_1;

    /* tailcall */
    return 93626c_1();
}
