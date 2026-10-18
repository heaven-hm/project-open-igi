// IGI native provenance: Camera_User32Dll - 0x004ABBBD - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004ABBBD-Camera_User32Dll.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Camera_User32Dll.h"
int Camera_User32Dll(int context, int arg2, int arg3)

{
    int context_val = 0;

    if (!g_936204)
    {
        HMODULE hModule = LoadLibraryA("user32.dll");
        int data_1;

        if (hModule)
        {
            data_1 = GetProcAddress(hModule, "MessageBoxA");
            g_936204 = data_1;
        }

        if (!hModule || !data_1)
            return 0;

        g_936208 = GetProcAddress(hModule, "GetActiveWindow");
        g_93620c = GetProcAddress(hModule, "GetLastActivePopup");
    }

    int data_4 = g_936208;

    if (data_4)
    {
        context_val = data_4();

        if (context_val)
        {
            int data_6 = g_93620c;

            if (data_6)
                context_val = data_6(context_val);
        }
    }

    return g_936204(context_val, context, arg2, arg3);
}
