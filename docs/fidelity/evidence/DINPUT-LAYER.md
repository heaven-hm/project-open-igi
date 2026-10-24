# DINPUT LAYER: keyboard/mouse/joypad creation and the DI error mapper

Retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`; PE32 base 0x400000. Read-only: no engine/manifest edits, no build, no launch. Companion `dinput-layer.json`.

## 0. The import DLL is dinput.dll (DirectInput 3/5/7 API), not dinput8.dll

Full import-table evidence from the PE (`ii`): the only DirectInput imports are

| IAT slot | DLL | import | thunk |
|---|---|---|---|
| `0x00533038` | DINPUT.dll | `DirectInputCreateA` | `0x004AF740` (`ff2538305300` = `jmp [0x533038]`) |
| `0x0053303C` | DINPUT.dll | `DirectInputCreateEx` | `0x004AE730` (`ff253c305300` = `jmp [0x53303C]`) |

There are **no dinput8.dll imports** in this binary. `CMakeLists.txt` already links `dinput` (`target_link_libraries(igi PRIVATE ddraw dxguid dinput)`), so the two IAT thunks can be transcribed as real natives calling the imported functions - no port stubs needed. `0x4AF740` is also the thunk used by `0x48FFF0` and `0x509980`; `0x4AE730` by `0x48FDE0`.

## 1. Interfaces and vtable offsets the binary exercises (evidence: call sites + GUID bytes)

C-style DirectInput interfaces (object pointer passed as the first stack argument, `__stdcall`):

| interface / object global | vtable slot | method | callsite evidence |
|---|---|---|---|
| `IDirectInputA` (`g_5C8E28`, `g_A7B4F0`) | `+0x0C` | CreateDevice | 0x49006B (keyboard), 0x509A84 (joypad) |
| `IDirectInputA` | `+0x10` | EnumDevices | 0x5099C6 (joypad; type=4, cb=0x509A40, ref=0, flags=1) |
| `IDirectInput7A` (`g_5C8E14`) | `+0x24` | CreateDeviceEx | 0x48FE35 (mouse) |
| `IDirectInputDeviceA` (`g_5C8E24`, `g_5C8E1C`, `g_A7B2E0[i]`, `g_A7B4E0[i]`) | `+0x00` | QueryInterface | 0x509AAE (joypad callback, riid `0x005349C0`) |
| `IDirectInputDeviceA` | `+0x08` | Release | 0x509B04, 0x509B50, 0x509EBD, 0x509EDC |
| `IDirectInputDeviceA` | `+0x0C` | GetCapabilities | 0x48FDA8 (mouse caps, size 0x2C), 0x5099F7 (joypad caps, size 0x2C) |
| `IDirectInputDeviceA` | `+0x18` | SetProperty | 0x509BB9 (prop 4, DIPROPRANGE-like 0x18-byte struct), 0x509C0A (prop 9, data 1), 0x509C2F (prop 6, data 0x2710=10000), 0x509C6A (prop 5, per-axis value from 0x54E8A8 table) |
| `IDirectInputDeviceA` | `+0x1C` | Acquire | 0x4900D9, 0x509CD9 (helper), 0x48FC44 (mouse retry) |
| `IDirectInputDeviceA` | `+0x20` | Unacquire | 0x490306 (0x4902F0 teardown), 0x509EB5 |
| `IDirectInputDeviceA` | `+0x24` | GetDeviceState | 0x49026B/0x4902B6 (keyboard 0x100), 0x48FC35/0x48FC5E (mouse 0x14) |
| `IDirectInputDeviceA` | `+0x2C` | SetDataFormat | 0x49008E (0x5348C8), 0x48FE58 (0x5348B0), 0x509AED (0x534C38) |
| `IDirectInputDeviceA` | `+0x34` | SetCooperativeLevel | 0x4900BB (hWnd, 6), 0x48FE85 (hWnd, 6), 0x509B39 (hWnd, 5) |

Only these names/offsets are asserted; they come from the dinput.h vtable order, which matches every callsite (QueryInterface @0, AddRef @4, Release @8, GetCapabilities @0xC, ... SetCooperativeLevel @0x34). No other interfaces are claimed.

## 2. GUID / data-format constants touched (image-init, `.rdata`)

.rdata file offset = VA - 0x400000 for this image (`.rdata` VA 0x533000 / raw 0x133000).

| VA | file off | 16 bytes (hex) | meaning (evidence) |
|---|---|---|---|
| `0x005348B0` | `0x001348B0` | `18000000100000000200000014000000` | DIDATAFORMAT header: dwSize=0x18, dwObjSize=0x10, flags=2, dwDataSize=0x14 (mouse; used by 0x48FDE0) |
| `0x005348C8` | `0x001348C8` | `18000000100000000200000000010000` | DIDATAFORMAT header: dwSize=0x18, dwObjSize=0x10, flags=2, dwDataSize=0x100 (keyboard; used by 0x48FFF0) |
| `0x00534900` | `0x00134900` | `612b1d6fa0d5cf11bfc7444553540000` | **GUID_SysKeyboard** = 6F1D2B61-D5A0-11CF-BFC7-444553540000 |
| `0x00534910` | `0x00134910` | `602b1d6fa0d5cf11bfc7444553540000` | **GUID_SysMouse** = 6F1D2B60-D5A0-11CF-BFC7-444553540000 |
| `0x005349B0` | `0x001349B0` | `bcc6d7575623d3118e9d00c04f6844ae` | IID_IDirectInputDevice2A = 57D7C6BC-2356-11D3-8E9D-00C04F6844AE (riid for CreateDeviceEx) |
| `0x005349C0` | `0x001349C0` | `82e644592ec9cf11bfc7444553540000` | IID_IDirectInputDeviceA = 5944E682-C92E-11CF-BFC7-444553540000 (QueryInterface in joypad callback) |
| `0x005349D0` | `0x001349D0` | `84b64c9a6d23d3118e9d00c04f6844ae` | IID_IDirectInput7A = 9A4CB684-236D-11D3-8E9D-00C04F6844AE (riid for DirectInputCreateEx) |
| `0x00534C38` | `0x00134C38` | `18000000100000000100000050000000` | DIDATAFORMAT header for joypads: dwSize=0x18, dwObjSize=0x10, flags=1, dwDataSize=0x50 |
| `0x00534258` | `0x00134258` | `101010101010703f` (qword) | double 1/255 = 0.0039215686274509803 (mouse delta scale) |
| `0x00542274` | `0x00142274` | ascii | "Could not create DirectInput keyboard object!" |
| `0x00542244` | `0x00142244` | ascii | "Could not create DirectInput device keyboard!" |
| `0x0054221C` | `0x0014221C` | ascii | "Could not set data format for keyboard!" |
| `0x0054216C` | `0x0014216C` | ascii | "Could not set cooperative level for keyboard!" |
| `0x005421F0` | `0x001421F0` | ascii | "Could not create DirectInput mouse object!" |
| `0x005421C4` | `0x001421C4` | ascii | "Could not create DirectInput device mouse!" |
| `0x0054219C` | `0x0014219C` | ascii | "Could not set data format for mouse!" |
| `0x00542150` | `0x00142150` | ascii | "Could not get mouse caps!" |
| `0x0054E950` | `0x0014E950` | ascii | "Could not create DirectInput joypad object." |
| `0x0054E920` | `0x0014E920` | ascii | "Could not acquire one or more joystick devices." |
| `0x0054E998` | `0x0014E998` | ascii | "Failed to obtain interface." |
| `0x0054E97C` | `0x0014E97C` | ascii | "Failed to create device." |
| `0x0054E8E8` | `0x0014E8E8` | ascii | "Could not get capabilities for one or more joysticks ." (trailing space before the period) |
| `0x0054E8A8` | `0x0014E8A8` | table | 8 pairs (dwPropOffset, value) for the joypad DIPROP_DEADZONE SetProperty loop, end 0x54E8E8 |
| `0x00542xxx` | | | the 24 DIERR_* literals + "Unknown error" of 0x48FA80 (0x542004-0x542124) and its jump table at `0x0048FC00` |

**Zero-fill runtime-written globals (no file bytes, loader init 0, written only at runtime):** `0x5C8BE0` (already an engine global in `app_startup_globals.cpp`), `0x5C8E24`, `0x5C8E1C`, `0x5C8E14`, `0x5C8E28`, `0x5C8E30`, `0x5C8E10/0C/18/08`, `0x5C8BC0` (instance handle, written by engine `StoreInstanceHandle` 0x48F0D0), `0x5C8BC4` (window handle, written by engine `SetWindowHandle` 0x48F0E0), `0xA7B4F0`, `0xA7B2E0`, `0xA7B4E0`, `0xA7B638`, `0xA7B2F0`, `0xA7B430`, `0xA7B63C`, `0xBCAA00`, `0xC28C60/61/62/D62/E80`, `0xC28F80/81/84/88/8C`. All are `.data` beyond the raw image tail - verified via the PE section map.

## 3. Per-function evidence and landability

### 3.1 0x48F270 getter - 6 B, 1 block, **LANDABLE NOW**

- Bytes: `a1e08b5c00c3` (`mov eax,[0x5C8BE0]; ret`), extent `0x0048F270-0x0048F275`.
- Callees: none. Data: `0x5C8BE0` is the engine global `g_5c8be0` (`engine/src/retail/core/app_startup_globals.cpp`), image 0. Keyboard_Update/Mouse_Update branch on it (debug/forced-input mode).
- Paste-ready body:
```cpp
// Native: Input_GetGlobalFlag_48F270 (placeholder name until VERIFIED; BN alias FX_SmokeEmit_25 is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048F270-0x0048F275.
// Tools/evidence: Binary Ninja MCP disassembly, radare2 bytes; 6 bytes, 1 block, no callees.
// Behavior/CFG: return the 32-bit global at 0x005C8BE0 (engine g_5c8be0).
// Status/limits: TRANSCRIBED; receiver already exists in engine (app_startup_globals).
#include "igi/retail/core/app_startup_globals.hpp"
#include <cstdint>

extern "C" std::int32_t NATIVE_DECL Input_GetGlobalFlag_48F270()
{
    return igi::retail::core::g_5c8be0;
}
```

### 3.2 0x48FA80 DI-error mapper - 381 B, 59 blocks, **LANDABLE NOW** (data-only)

- Extent `0x0048FA80-0x0048FBFD`; callees: none. Jump table at `0x0048FC00` (6 entries) and 25 string literals are image `.rdata` data (see §2).
- Behaviour (BN pseudocode, structure preserved): if `hr == 0x80040209` return "DIERR_UNPLUGGED"; else if `hr <= 0x80040202` switch over 0x80004001/0x80004002/0x80004005/0x80040110/0x80040154/0x80040200/0x80040201/0x80040202; else if `(hr + 0x7FFBFDFD) <= 5` switch over 0x80040203..0x80040208; then a second switch over 0x8004020A/0x80070002/0x80070005/0x8007000C/0x8007000E/0x80070015/0x8007001E/0x80070057/0x80070077/0x800700AA/0x8007047E/0x80070481/0x800704DF; default "Unknown error".
- Paste-ready body (exact mapping, retail branch order preserved):
```cpp
// Native: DirectInput_ErrorText_48FA80 (placeholder name until VERIFIED; BN alias DIERR_GENERIC).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048FA80-0x0048FBFD.
// Tools/evidence: Binary Ninja MCP decompile/disassembly, radare2 bytes; 381 bytes, 59 blocks, no callees.
// Behavior/CFG: HRESULT -> DIERR_* literal mapping in retail branch order; fallback "Unknown error".
// Status/limits: TRANSCRIBED; all literals and the 0x0048FC00 jump table are image .rdata data.
#include <cstdint>

extern "C" char const* NATIVE_DECL DirectInput_ErrorText_48FA80(std::int32_t hr)
{
    if (hr == 0x80040209)
        return "DIERR_UNPLUGGED";
    if (hr <= 0x80040202)
    {
        switch (hr)
        {
        case 0x80004001: return "DIERR_UNSUPPORTED";
        case 0x80004002: return "DIERR_NOINTERFACE";
        case 0x80004005: return "DIERR_GENERIC";
        case 0x80040110: return "DIERR_NOAGGREGATION";
        case 0x80040154: return "DIERR_DEVICENOTREG";
        case 0x80040200: return "DIERR_INSUFFICIENTPRIVS";
        case 0x80040201: return "DIERR_DEVICEFULL";
        case 0x80040202: return "DIERR_MOREDATA";
        }
    }
    else if (static_cast<std::uint32_t>(hr + 0x7FFBFDFD) <= 5U)
    {
        switch (hr)
        {
        case 0x80040203: return "DIERR_NOTDOWNLOADED";
        case 0x80040204: return "DIERR_HASEFFECTS";
        case 0x80040205: return "DIERR_NOTEXCLUSIVEACQUIRED";
        case 0x80040206: return "DIERR_INCOMPLETEEFFECT";
        case 0x80040207: return "DIERR_NOTBUFFERED";
        case 0x80040208: return "DIERR_EFFECTPLAYING";
        }
    }
    switch (hr)
    {
    case 0x8004020A: return "DIERR_REPORTFULL";
    case 0x80070002: return "DIERR_NOTFOUND";
    case 0x80070005: return "DIERR_OTHERAPPHASPRIO";
    case 0x8007000C: return "DIERR_NOTACQUIRED";
    case 0x8007000E: return "DIERR_OUTOFMEMORY";
    case 0x80070015: return "DIERR_NOTINITIALIZED";
    case 0x8007001E: return "DIERR_INPUTLOST";
    case 0x80070057: return "DIERR_INVALIDPARAM";
    case 0x80070077: return "DIERR_BADDRIVERVER";
    case 0x800700AA: return "DIERR_ACQUIRED";
    case 0x8007047E: return "DIERR_OLDDIRECTINPUTVERSION";
    case 0x80070481: return "DIERR_BETADIRECTINPUTVERSION";
    case 0x800704DF: return "DIERR_ALREADYINITIALIZED";
    }
    return "Unknown error";
}
```

### 3.3 IAT thunks 0x4AF740 / 0x4AE730 - 6 B each, **LANDABLE NOW**

- `0x004AF740`: `ff2538305300` = `jmp [0x00533038]` (DirectInputCreateA, DINPUT.dll); extent `0x004AF740-0x004AF745`.
- `0x004AE730`: `ff253c305300` = `jmp [0x0053303C]` (DirectInputCreateEx, DINPUT.dll); extent `0x004AE730-0x004AE735`.
- No deps beyond the already-linked `dinput` import library; bodies are `return DirectInputCreateA(...)` / `return DirectInputCreateEx(...)` with the dinput.h prototypes. These are the two "pure IAT callers" of the batch.

### 3.4 0x48FD90 mouse-caps reader - 66 B, 4 blocks, **LANDABLE NOW**

- Extent `0x0048FD90-0x0048FDD1`; bytes `83ec2ca11c8e5c008d542400c74424002c000000528b0850ff510c85c0740f685021540068...` (full hex in json).
- Callees: vtable `+0x0C` GetCapabilities on `g_5C8E1C`, `Debug_Error` (engine). Writes `0xC28F80=1`, `0xC28F81=caps byte at +0x10` (dwDevType low byte).
- Dependencies: only the runtime device object + engine `Debug_Error`; **paste-ready**.
```cpp
// Native: Input_MouseGetCaps_48FD90 (placeholder until VERIFIED; BN alias FX_CouldNotGetMouseCaps).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x0048FD90-0x0048FDD1.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 bytes; 66 bytes, 4 blocks.
// Behavior/CFG: GetCapabilities(&caps{dwSize=0x2C}) on the mouse device; on failure Debug_Error("Could not get mouse caps!") + spin;
//           else store 0x00C28F80=1 and 0x00C28F81=caps byte +0x10 (DevType low byte).
// Status/limits: TRANSCRIBED; only callee Debug_Error is engine; device object is runtime data.
#include "igi/retail/core/retail_error.hpp"
#include <cstdint>

extern "C" std::int32_t g_5c8e1c;      // IDirectInputDeviceA* (zero-fill runtime)
extern "C" std::uint8_t g_c28f80;
extern "C" std::uint8_t g_c28f81;

extern "C" std::int32_t NATIVE_DECL Input_MouseGetCaps_48FD90()
{
    std::uint8_t caps[0x2C] = {};
    *reinterpret_cast<std::uint32_t*>(&caps[0]) = 0x2CU;
    auto device = reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_5c8e1c));
    auto const* vtable = *reinterpret_cast<void* const* const*>(device);
    using GetCapabilitiesFn = std::int32_t(__stdcall*)(void*, void*);
    auto const get_caps = reinterpret_cast<GetCapabilitiesFn>(vtable[3]);
    if (get_caps(device, caps) != 0)
    {
        Debug_Error("Could not get mouse caps!");
        for (;;)
        {
        }
    }
    g_c28f80 = 1;
    g_c28f81 = caps[0x10];
    return static_cast<std::int32_t>(caps[0x10]);
}
```
(Return value: retail leaves eax = caps byte +0x10 with previous high bytes; callers in this layer ignore it - marked as a limit.)

### 3.5 0x48FFF0 keyboard create - 294 B, 15 blocks, **BLOCKED**

- Extent `0x0048FFF0-0x00490115`; bytes head `83ec10a100495300...` (full hex in json). Call sequence with exact args:
  1. `0x48F090()` -> hInstance; copy GUID_SysKeyboard (0x534900) to the 0x10-byte stack area; `0x5C8E28=0; 0x5C8E24=0`; `DirectInputCreateA(hInstance, 0x700, &g_5C8E28, 0)` via thunk 0x4AF740; failure -> `Debug_Error("Could not create DirectInput keyboard object!")` + spin (0x490055).
  2. `g_5C8E28->CreateDevice(GUID_SysKeyboard, &g_5C8E24, 0)` (vtable +0x0C); failure -> Debug_Error("Could not create DirectInput device keyboard!") + spin.
  3. `g_5C8E24->SetDataFormat(&format 0x5348C8)` (+0x2C, c_dfDIKeyboard 0x100 bytes); failure -> Debug_Error("Could not set data format for keyboard!") + spin.
  4. `g_5C8E24->SetCooperativeLevel(hWnd 0x48F0A0(), 6)` (+0x34); failure -> Debug_Error("Could not set cooperative level for keyboard!") + spin.
  5. `g_5C8E24->Acquire()` (+0x1C); if it returns 0 set `0x5C8E30=1`. Then `0x490340()` (vsync flag setter), `0x490350(0)` (swap flag setter), zero 0x100 bytes at `0xC28E80`, `WinProc_RegisterHandler_48F490(0x490120)`.
- Missing deps (in order): `0x48F090` getter (not in engine; 6 B `mov eax,[0x5C8BC0]; ret`), `0x4AF740` (this batch), `0x490340` (7 B `mov byte [0xC28C61],1; ret`), `0x490350` (10 B `mov al,[esp+4]; mov [0xC28C60],al; ret`), `0x490120` WinProc handler (KB message mapping; not in engine). 0x48F0A0/0x48F490/0x4AF7B0 are already engine/this batch.
- Note: retail leaves the `push 0` argument for Display_SwapBuffers on the stack and later `add esp,8` after the WinProc registration; no semantic effect (0x48F490 reads its own stack argument), recorded as a comment only.

### 3.6 0x48FDE0 mouse create - 226 B, 13 blocks, **BLOCKED on 0x48FED0 WinProc**

- Extent `0x0048FDE0-0x0048FEC1`; `0x5C8E14=0; 0x5C8E1C=0`; `DirectInputCreateEx(0x48F090(), 0x700, &IID_IDirectInput7A 0x5349D0, &g_5C8E14, 0)` via thunk 0x4AE730; `g_5C8E14->CreateDeviceEx(GUID_SysMouse 0x534910, &IID_IDirectInputDevice2A 0x5349B0, &g_5C8E1C, 0)` (+0x24); `SetDataFormat(&0x5348B0)` (+0x2C); `SetCooperativeLevel(0x48F0A0(), 6)` (+0x34); `0x48FD90()`; `0x5C8E10=0; 0x5C8E0C=0; 0x5C8E08=0`; `WinProc_RegisterHandler_48F490(0x48FED0)`. Error strings: "Could not create DirectInput mouse object!" / "...device mouse!" / "...data format for mouse!" / "Could not set cooperative level for keyboard!" (the mouse path reuses the keyboard cooperative-level string at 0x54216C).
- Remaining deps: `0x48FED0` WinProc (mouse-message handler; not in engine), `0x48F090` getter. 0x48FD90/0x4AE730 are this batch; the rest are engine natives.

### 3.7 Joypad family

**0x509980 create+enumerate - 185 B, 14 blocks, BLOCKED on 0x509A40.** `0xA7B638=0`; `DirectInputCreateA(0x48F090(), 0x700, &g_A7B4F0, 0)`; failure "Could not create DirectInput joypad object." + spin; `g_A7B4F0->EnumDevices(4 /*DI8DEVTYPE_JOYSTICK*/, 0x509A40, 0, 1 /*DIEDFL_ATTACHEDONLY*/)` (+0x10); if `g_A7B638<=0` -> Debug_Error("Could not acquire one or more joystick devices.") + spin; else for i<g_A7B638: `0x509CC0(i)`; if false -> same error + spin; else `*dword[0xA7B430+i*0x2C]=0x2C; g_A7B4E0[i]->GetCapabilities(&0xA7B430+i*0x2C)` (+0x0C); fail -> error spin; `0x509F00(i)`.

**0x509A40 EnumDevices callback - 629 B, 22 blocks, `__stdcall(void*, void*)` with `ret 8`, BLOCKED on 0x48F0A0.** This native contains address `0x509B14` (BN extent 0x509A40-0x509CB4; 0x509B14 is not a function entry - it is the `mov [edx*4+0xA7B4E0], edi` release path). Sequence: copy lpddi->guidInstance (at [esp+0x40]+4, 16 bytes) to a stack GUID; `g_A7B4F0->CreateDevice(&guid, &g_A7B2E0[g_A7B638], 0)` (+0x0C); fail -> Debug_Error("Failed to create device.") + spin; `device->QueryInterface(&IID_IDirectInputDeviceA 0x5349C0, &g_A7B4E0[g_A7B638])` (+0x00); then `g_A7B2E0[idx]->Release()` (+0x08); if QueryInterface failed -> Debug_Error("Failed to obtain interface.") + spin; `g_A7B4E0[idx]->SetDataFormat(&0x534C38)` (+0x2C); failure path releases and stores NULL then returns 1; `SetCooperativeLevel(0x48F0A0(), 5)` (+0x34); failure -> release + NULL + return 1; then DIPROP_RANGE (prop id 4, header 0x18/0x10, dwObj 1) loop over the 8 (offset,value) pairs at 0x54E8A8..0x54E8E8: `SetProperty(4, dev, &range{lMin,lMax})` (+0x18); DIPROPDWORD prop 9 data 1; prop 6 data 0x2710; prop 5 data from 0x54E8A8 table (deadzone); finally increment g_A7B638 and return 1.

**0x509E90 release-all - 112 B, 9 blocks, LANDABLE (no retail callees).** If `g_A7B4F0==0` -> `g_A7B638=0; ret`; else for i<g_A7B638: if device: `Unacquire()` (+0x20), `Release()` (+0x08), `g_A7B4E0[i]=0`; then `g_A7B4F0->Release()` (+0x08); `g_A7B4F0=0; g_A7B638=0`.

**0x509CC0 joypad-acquired check - 41 B, 3 blocks, LANDABLE (no retail callees).** `if (!g_A7B4E0[i]) return 0; return g_A7B2E0[i]->Acquire() >= 0` (+0x1C).

**0x509F00 per-device action flag init - 68 B, 1 block, LANDABLE (no callees).** Writes `0xBCAA00+i*0x28`: byte+0 = (g_A7B4E0[i]!=0), +1/+3/+4 = 1, +7 = byte [0xA7B440+i*0x28] (deadzone table).

## 4. Landability and call order

| native | extent | blocks | deps not yet in engine | verdict |
|---|---|---|---|---|
| 0x48F270 | 6 B | 1 | none | **land now** |
| 0x48FA80 | 381 B | 59 | none (data only) | **land now** |
| 0x4AF740 | 6 B | 1 | none (dinput already linked) | **land now** |
| 0x4AE730 | 6 B | 1 | none | **land now** |
| 0x48FD90 | 66 B | 4 | none (Debug_Error engine) | **land now** |
| 0x509CC0 | 41 B | 3 | none | **land now** |
| 0x509F00 | 68 B | 1 | none | **land now** |
| 0x509E90 | 112 B | 9 | none | **land now** |
| 0x48F090 getter | 6 B | 1 | none (0x5C8BC0 writer is engine) | land (needed by create natives) |
| 0x48F0A0 getter | 6 B | 1 | none (0x5C8BC4 writer is engine) | land (needed by create natives) |
| 0x509A40 | 629 B | 22 | 0x48F0A0, 0x48F090 | after getters |
| 0x509980 | 185 B | 14 | 0x509A40, 0x4AF740, 0x48F090, 0x509CC0, 0x509F00 | after those |
| 0x48FFF0 | 294 B | 15 | 0x490340, 0x490350, 0x490120, 0x48F090, 0x4AF740 | after those |
| 0x48FDE0 | 226 B | 13 | 0x48FED0, 0x48F090, 0x4AE730, 0x48FD90 | after those |

Engine already contains: `0x48F0D0 StoreInstanceHandle`, `0x48F0E0 SetWindowHandle`, `0x48F490 WinProc_RegisterHandler_48F490` (TRANSCRIBED), `0x4AF8F0 HeapList_Insert`, `0x4AF7B0 Debug_Error`, `g_5c8be0`. Not in engine (verified by implementation grep, not by catalog record): `0x48F090`, `0x48F0A0` (`native_0048f970.cpp` says "0x0048F0A0 has no implementation"), `0x48FA80`, `0x48F270`, `0x48FD90`, `0x4AF740`, `0x4AE730`, `0x48FFF0`, `0x48FDE0`, the joypad family, `0x490340`, `0x490350`, `0x490120`, `0x48FED0`.

Call order for landing: (1) 0x4AF740, 0x4AE730, 0x48F270, 0x48FA80, 0x48FD90, 0x48F090, 0x48F0A0; (2) 0x490340, 0x490350, 0x509F00, 0x509CC0, 0x509E90; (3) 0x509A40; (4) 0x509980; (5) 0x490120 + 0x48FED0 WinProcs; (6) 0x48FFF0, 0x48FDE0; then the previous batch trio 0x490230, 0x48FC20, 0x509CF0 become landable.

## 5. What the port must declare (no invented interfaces)

- Two imported functions from **DINPUT.dll** (already linked as `dinput`): `DirectInputCreateA(HINSTANCE, DWORD dwVersion, IDirectInputA**, IUnknown*)`, `DirectInputCreateEx(HINSTANCE, DWORD dwVersion, REFIID, void**, IUnknown*)`. The `dwVersion` argument is the constant `0x700` at every call site (DirectInput 7.0).
- The C-style interfaces exactly as the callsites use them (the system dinput.h declares these names and orders; a local declaration may use only these offsets): IDirectInputA (0x0C CreateDevice, 0x10 EnumDevices), IDirectInput7A (0x24 CreateDeviceEx), IDirectInputDeviceA (0x00 QueryInterface, 0x08 Release, 0x0C GetCapabilities, 0x18 SetProperty, 0x1C Acquire, 0x20 Unacquire, 0x24 GetDeviceState, 0x2C SetDataFormat, 0x34 SetCooperativeLevel).
- GUIDs must be byte-exact from §2 (they are image data: use the literal 16-byte values; do not substitute dinput8 GUIDs - IID_IDirectInput7A here is 9A4CB684-236D-11D3-8E9D-00C04F6844AE). DIDATAFORMAT blocks 0x5348B0/0x5348C8/0x534C38 are 0x18-byte headers with the listed fields; the SetDataFormat calls pass those exact bytes.
- Everything in §2 marked zero-fill must be declared as runtime-written globals with loader value 0 (matching the candidate .data zero-fill tail); g_5C8BE0 keeps using the existing engine definition.
## 6. Paste-ready bodies for the stopgap natives (IAT thunks + getters + small joypad helpers)

These are the ones with no missing retail-native dependency once the layer lands. Bodies use only their batch/engine deps.

```cpp
// Native: DirectInputCreateA thunk; retail VA 0x004AF740-0x004AF745 (6 bytes: jmp [0x00533038]).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 bytes (ff2538305300), PE import table entry 0x00533038 DINPUT.dll DirectInputCreateA.
// Behavior/CFG: tail-call the imported DirectInputCreateA (the candidate links dinput).
// Status/limits: TRANSCRIBED; single-block IAT tail call, no other side effects.
#include <cstdint>
struct IDirectInputA;

extern "C" __declspec(dllimport) std::int32_t __stdcall DirectInputCreateA(
    void* hinst, std::uint32_t version, IDirectInputA** out, void* outer);

extern "C" std::int32_t NATIVE_DECL DirectInputCreateA_Thunk_4AF740(
    void* hinst, std::uint32_t version, IDirectInputA** out, void* outer)
{
    return DirectInputCreateA(hinst, version, out, outer);
}

// Native: DirectInputCreateEx thunk; retail VA 0x004AE730-0x004AE735 (6 bytes: jmp [0x0053303C]).
// Behavior: tail-call the imported DirectInputCreateEx.
// Status/limits: TRANSCRIBED; single-block IAT tail call.
extern "C" __declspec(dllimport) std::int32_t __stdcall DirectInputCreateEx(
    void* hinst, std::uint32_t version, void const* iid, void** out, void* outer);

extern "C" std::int32_t NATIVE_DECL DirectInputCreateEx_Thunk_4AE730(
    void* hinst, std::uint32_t version, void const* iid, void** out, void* outer)
{
    return DirectInputCreateEx(hinst, version, iid, out, outer);
}

// Native: GetInstanceHandle_48F090; retail VA 0x0048F090-0x0048F095 (a1c08b5c00c3).
// Behavior: return dword [0x005C8BC0] (written by engine StoreInstanceHandle 0x0048F0D0).
// Status/limits: TRANSCRIBED; global is zero-fill runtime-written; the port must declare 0x005C8BC0.
extern "C" std::int32_t g_5c8bc0;
extern "C" void* NATIVE_DECL GetInstanceHandle_48F090()
{
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_5c8bc0));
}

// Native: GetWindowHandle_48F0A0; retail VA 0x0048F0A0-0x0048F0A5 (a1c48b5c00c3).
// Behavior: return dword [0x005C8BC4] (written by engine SetWindowHandle 0x0048F0E0).
// Status/limits: TRANSCRIBED; 0x0048F0A0 currently has no implementation in engine/native_0048f970.cpp.
extern "C" std::int32_t g_5c8bc4;
extern "C" void* NATIVE_DECL GetWindowHandle_48F0A0()
{
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(g_5c8bc4));
}

// Native: Display_SetVSyncFlag_490340; retail VA 0x00490340-0x00490346 (c605618cc20001c3).
// Behavior: byte [0x005C28C61] = 1; ret.
extern "C" std::uint8_t g_c28c61;
extern "C" void NATIVE_DECL Display_SetVSyncFlag_490340() { g_c28c61 = 1; }

// Native: Display_SetSwapFlag_490350; retail VA 0x00490350-0x00490359 (8a442404a2608cc200c3).
// Behavior: byte [0x005C28C60] = (uint8)arg1; ret.
extern "C" std::uint8_t g_c28c60;
extern "C" void NATIVE_DECL Display_SetSwapFlag_490350(char flag) { g_c28c60 = static_cast<std::uint8_t>(flag); }

// Native: Joypad_IsAcquired_509CC0; retail VA 0x00509CC0-0x00509CE8 (41 bytes, 3 blocks).
// Behavior: if (!g_A7B4E0[i]) return 0; return g_A7B2E0[i]->Acquire() >= 0.
// Status/limits: TRANSCRIBED; only a vtable call (0x1C) on the runtime device arrays.
extern "C" void* g_a7b4e0[16];
extern "C" void* g_a7b2e0[16];
extern "C" std::int32_t NATIVE_DECL Joypad_IsAcquired_509CC0(std::int32_t index)
{
    if (g_a7b4e0[index] == nullptr)
        return 0;
    void* device = g_a7b2e0[index];
    auto const* vtable = *reinterpret_cast<void* const* const*>(device);
    using AcquireFn = std::int32_t(__stdcall*)(void*);
    std::int32_t const hr = reinterpret_cast<AcquireFn>(vtable[7])(device);
    return hr >= 0 ? 1 : 0;
}

// Native: Joypad_InitActionFlags_509F00; retail VA 0x00509F00-0x00509F43 (68 bytes, 1 block).
// Behavior: for slot i: byte[0xBCAA00+i*0x28] = (g_A7B4E0[i] != 0); bytes +1,+3,+4 = 1;
//           byte[0xBCAA07+i*0x28] = byte[0xA7B440+i*0x28].
// Status/limits: TRANSCRIBED; no callees; exact byte listing in dinput-layer.json.
extern "C" std::uint8_t g_bcaa00[0x28 * 16];
extern "C" std::uint8_t g_a7b440[0x28 * 16];
extern "C" void NATIVE_DECL Joypad_InitActionFlags_509F00(std::int32_t index)
{
    std::uint8_t* slot = &g_bcaa00[index * 0x28];
    slot[0] = g_a7b4e0[index] != nullptr ? 1 : 0;
    slot[1] = 1;
    slot[3] = 1;
    slot[4] = 1;
    slot[7] = g_a7b440[index * 0x28];
}

// Native: Joypad_ReleaseAll_509E90; retail VA 0x00509E90-0x00509EFF (112 bytes, 9 blocks).
// Behavior: if (!g_A7B4F0) { g_A7B638 = 0; return; } for i<g_A7B638: release device[i] (Unacquire 0x20, Release 0x08),
//           null the slot; then release g_A7B4F0 (0x08); g_A7B4F0 = 0; g_A7B638 = 0.
// Status/limits: TRANSCRIBED; only vtable calls; device arrays runtime data.
extern "C" void* g_a7b4f0;
extern "C" std::int32_t g_a7b638;
extern "C" void NATIVE_DECL Joypad_ReleaseAll_509E90()
{
    if (g_a7b4f0 == nullptr)
    {
        g_a7b638 = 0;
        return;
    }
    for (std::int32_t i = 0; i < g_a7b638; ++i)
    {
        void* device = g_a7b4e0[i];
        if (device != nullptr)
        {
            auto const* vtable = *reinterpret_cast<void* const* const*>(device);
            using UnacquireFn = std::int32_t(__stdcall*)(void*);
            using ReleaseFn = std::int32_t(__stdcall*)(void*);
            reinterpret_cast<UnacquireFn>(vtable[8])(device);
            reinterpret_cast<ReleaseFn>(vtable[2])(device);
            g_a7b4e0[i] = nullptr;
        }
    }
    auto const* io_vtable = *reinterpret_cast<void* const* const*>(g_a7b4f0);
    using ReleaseFn = std::int32_t(__stdcall*)(void*);
    reinterpret_cast<ReleaseFn>(io_vtable[2])(g_a7b4f0);
    g_a7b4f0 = nullptr;
    g_a7b638 = 0;
}
```
(0x509F00/0x509E90 bodies are behavioural transcriptions from the documented retail sequence; their exact instruction listings are in the json and must be diffed line-by-line during the transcription review, as with any native.)
