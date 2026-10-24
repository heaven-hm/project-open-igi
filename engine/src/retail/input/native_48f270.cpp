// Native: Input_GetInstance_48F270 (placeholder until VERIFIED); retail VA 0x0048F270-0x0048F275 (6 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes a1 e0 8b 5c 00 c3; Binary Ninja MCP decompile; DirectInput layer packet DINPUT-LAYER.md.
// Behavior/CFG: mov eax,[0x005C8BE0]; ret - returns the app instance handle kept in the engine global g_5c8be0.
// Status/limits: TRANSCRIBED; g_5c8be0 (engine app_startup_globals.cpp, image 0) is written by the transcribed StoreInstanceHandle 0x48F0D0. Runtime UNOBSERVED.
#include "igi/retail/core/app_startup_globals.hpp"
#include "igi/retail/input/native_48f270.hpp"

extern "C" {

std::int32_t Input_GetInstance_48F270()
{
    return g_5c8be0; // 0x0048F270: a1 e0 8b 5c 00 (mov eax,[0x5C8BE0]); c3
}

} // extern "C"
