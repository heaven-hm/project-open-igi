// IGI native FX_SmokeEmit_23 - 0x0048F140 - transcribed from retail igi.exe.
// Retail: eax=[esp+4]; [0x5c8bfc]=movsx(al); ecx=[0x5c8c18]; if ecx: call ecx(eax,[0x5c8c1c]) cdecl; ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bfc;
extern std::int32_t g_5c8c18;
extern std::int32_t g_5c8c1c;

int __cdecl FX_SmokeEmit_23(int context)
{
    typedef int (__cdecl *Callback)(int, int);
    g_5c8bfc = (signed char)context;
    Callback callback = (Callback)(intptr_t)g_5c8c18;

    if (!callback)
        return context;

    return callback(context, g_5c8c1c);
}
