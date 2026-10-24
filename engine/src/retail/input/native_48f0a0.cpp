// Native: GetWindowHandle_48F0A0 (placeholder until VERIFIED); retail VA 0x0048F0A0-0x0048F0A5 (6 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes a1 c4 8b 5c 00 c3; Binary Ninja MCP decompile; native_0048f970.cpp notes this getter had no implementation.
// Behavior/CFG: mov eax,[0x005C8BC4]; ret - returns the game window handle.
// Status/limits: TRANSCRIBED; reads the existing port global g_window_handle (native_0048f0e0.hpp), which transcribed SetWindowHandle 0x48F0E0 writes. Runtime UNOBSERVED.
#include "igi/retail/core/native_0048f0e0.hpp"

extern "C" {

void* GetWindowHandle_48F0A0()
{
    return g_window_handle; // 0x0048F0A0: a1 c4 8b 5c 00 (mov eax,[0x5C8BC4]); c3
}

} // extern "C"
