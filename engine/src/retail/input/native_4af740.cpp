// Native: DirectInputCreateA (IAT thunk); retail VA 0x004AF740-0x004AF745 (6 bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 bytes ff 25 38 30 53 00; PE import table 0x00533038 = DINPUT.dll DirectInputCreateA; DINPUT-LAYER packet.
// Behavior/CFG: jmp dword [0x00533038] - forwards all arguments to the imported DirectInputCreateA.
// Status/limits: TRANSCRIBED; the IAT call is part of the retail native's flow (no stub, no reimplementation). Runtime UNOBSERVED.
#include <dinput.h>

extern "C" {

HRESULT WINAPI DirectInputCreateA(HINSTANCE hinst, DWORD dwVersion, LPDIRECTINPUTA* ppDI,
                                  LPUNKNOWN punkOuter)
{
    // 0x004AF740: ff 25 38 30 53 00 (jmp dword [0x533038]).
    return ::DirectInputCreateA(hinst, dwVersion, ppDI, punkOuter);
}

} // extern "C"
