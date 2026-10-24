// Native: GetInstanceHandle_48F090 (placeholder until VERIFIED); retail VA 0x0048F090-0x0048F095 (6 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes a1 c0 8b 5c 00 c3; Binary Ninja MCP decompile; DINPUT-LAYER packet.
// Behavior/CFG: mov eax,[0x005C8BC0]; ret - returns the application instance handle.
// Status/limits: TRANSCRIBED; reads the existing port global InstanceHandle (app_startup_natives.hpp), which transcribed StoreInstanceHandle 0x48F0D0 writes. Runtime UNOBSERVED.
#include "igi/retail/core/app_startup_natives.hpp"

extern "C" {

void* GetInstanceHandle_48F090()
{
    return InstanceHandle; // 0x0048F090: a1 c0 8b 5c 00 (mov eax,[0x5C8BC0]); c3
}

} // extern "C"
