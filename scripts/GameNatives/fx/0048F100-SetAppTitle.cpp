// IGI native FX_ExplosionSpawn_23 - 0x0048F100 - transcribed from retail igi.exe.
// Retail: ecx=[0x5c8bc4]; eax=[esp+4]; [0x5c8bd4]=eax; if ecx: SetWindowTextA(ecx,eax); ret
#include <cstdint>
#include <windows.h>
extern std::int32_t g_5c8bc4;
extern std::int32_t g_5c8bd4;

int __cdecl FX_ExplosionSpawn_23(int context)
{
    HWND hWnd = (HWND)(intptr_t)g_5c8bc4;
    g_5c8bd4 = context;

    if (!hWnd)
        return context;

    return SetWindowTextA(hWnd, (LPCSTR)(intptr_t)context);
}
