// IGI native provenance: Display_SwapBuffers_7 - 0x00494F50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00494F50-Display_SwapBuffers_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SwapBuffers_7.hpp"
int Display_SwapBuffers_7(HWND context, int arg2, int arg3, void* arg4)

{
    if (Sys_ReadFileBlock_3() == g_5ca0e0 && g_5ca10c)
    {
        if (arg2 == 6)
        {
            if ((!arg3 || arg3 >> 0x10) && FX_DebrisSpawn_23())
            {
                InvalidateRect(context, nullptr, 0);
                FX_SmokeEmit_23(0);
                return 1;
            }

            if (!FX_DebrisSpawn_23())
                FX_SmokeEmit_23(1);
        }
        else if (arg2 == 0x24)
        {
            RECT rect;
            rect.left = 0;
            rect.idx = 0;
            rect.right = g_c28b44;
            rect.bottom = g_c28b48;
            int data_4 = FX_FragmentSimulate_23();
            int data_5 = -(data_4);
            AdjustWindowRect(&rect, ((data_5 - data_5) & 0x7f310000) + 0xcf0000, 0);
            LONG left = rect.left;
            *(arg4 + 0x18) = 0x64;
            *(arg4 + 0x1c) = 0x64;
            LONG bottom = rect.bottom;
            *(arg4 + 0x20) = rect.right - left;
            *(arg4 + 0x24) = bottom - rect.idx;
        }
    }

    return 1;
}
