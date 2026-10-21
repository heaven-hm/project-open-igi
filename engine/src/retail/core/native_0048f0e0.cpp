#include "igi/retail/core/native_0048f0e0.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

void* g_window_handle = nullptr;

// Native: SetWindowHandle; retail VA 0x0048F0E0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x0048F0E0..0x0048F0EA.
// Behavior/CFG: one basic block loads the 32-bit argument, stores it at the window handle global 0x005C8BC4, and returns the same bits in EAX. The native at 0x0048F100 reads that global and calls SetWindowTextA with it, so the global is the window handle.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the store and the EAX return match the retail exactly.
void* IGI_RETAIL_CDECL SetWindowHandle(void* window)
{
    g_window_handle = window;
    return window;
}

} // extern "C"

#undef IGI_RETAIL_CDECL
