// Native: DirectInputCreateEx (IAT thunk); retail VA 0x004AE730-0x004AE735 (6 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes ff 25 3c 30 53 00; PE import table 0x0053303C = DINPUT.dll DirectInputCreateEx; DINPUT-LAYER packet.
// Behavior/CFG: jmp dword [0x0053303C] - forwards all arguments to the imported DirectInputCreateEx.
// Status/limits: TRANSCRIBED; the IAT call is part of the retail native's flow (no stub). Runtime UNOBSERVED.
#include <dinput.h>

extern "C" {

HRESULT WINAPI DirectInputCreateEx(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut,
                                   LPUNKNOWN punkOuter)
{
    // 0x004AE730: ff 25 3c 30 53 00 (jmp dword [0x53303C]).
    return ::DirectInputCreateEx(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

} // extern "C"
