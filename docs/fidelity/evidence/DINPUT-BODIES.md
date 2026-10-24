# DINPUT-BODIES: paste-ready bodies + per-body data lists

Retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` (PE32 base 0x400000). Read-only verification; no engine edits, no build. Every image byte listed is `VA-0x400000` in the file.

Bodies are ordered as requested. Each is preceded only by the constants/globals/callees/literals it needs. `NATIVE_DECL` maps to the retail calling convention shown in the provenance line. The two bodies you already have verbatim from the prior packet are reproduced unchanged from `20261024T041723Z/DINPUT-CALLBACKS.md` (extracted mechanically).

## 1. 0x509A40 callback - 629 B `0x00509A40-0x00509CB4`, 22 blocks, `__stdcall` ret 8

- Image: `0x005349C0` IID `82 e6 44 59 2e c9 cf 11 bf c7 44 45 53 54 00 00` (16 B).
- Image: `0x00534C38` DIDATAFORMAT (0x18 B) `180000001000000001000000500000002c000000a06f5100` (dwSize=0x18, dwObjSize=0x10, dwFlags=1, dwDataSize=0x50, dwNumObjs=0).
- Image: `0x0054E8A8` writable axis-pair table (64 B) `0c000000010000001000000001000000140000000100000000000000010000000400000001000000080000000100000018000000010000001c00000001000000` - must stay mutable (retail clears value dwords on 0x80070002).
- Globals (declared): `g_a7b4f0` void*; `g_a7b638` int32; `g_a7b2e0[16]` void*; `g_a7b4e0[16]` void*; plus `g_a7b63c` int32 at 0x00A7B63C (deadzone value) - NOT yet declared.
- Callees: `0x0048F0A0 GetWindowHandle_48F0A0`; `0x004AF7B0 Debug_Error`; COM via vtables (CreateDevice +0x0C, QueryInterface +0x00, Release +0x08, SetDataFormat +0x2C, SetCooperativeLevel +0x34, SetProperty +0x18).
- Literals: `0x0054E97C` `46 61 69 6c 65 64 20 74 6f 20 63 72 65 61 74 65 20 64 65 76 69 63 65 2e` "Failed to create device."; `0x0054E998` `46 61 69 6c 65 64 20 74 6f 20 6f 62 74 61 69 6e 20 69 6e 74 65 72 66 61 63 65 2e` "Failed to obtain interface.".

```cpp
// Native: Joypad_EnumDevicesCallback_509A40 (placeholder until VERIFIED; BN alias Physics_SolveConstraints_9 is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509A40-0x00509CB4.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 629 bytes, 22 blocks, __stdcall with ret 8.
// Behavior/CFG: EnumDevicesA callback; copies lpddi guidInstance (+4, 16 bytes), CreateDevice on g_A7B4F0 into g_A7B2E0[count],
//           QueryInterface(IID 0x005349C0) into g_A7B4E0[count] and Releases the raw device, SetDataFormat(0x00534C38),
//           SetCooperativeLevel(hWnd,5), DIPROP_RANGE over the 8 pairs at 0x0054E8A8 (clearing a pair on 0x80070002),
//           DIPROP_AUTOCENTER=1, DIPROP_SATURATION=0x2710, DIPROP_DEADZONE=dword[0x00A7B63C] skipping cleared pairs,
//           then ++g_A7B638 and return 1. Device failures release + null the g_A7B4E0 slot and return 1;
//           CreateDevice/QueryInterface failures call Debug_Error and spin forever.
// Status/limits: TRANSCRIBED; deps 0x0048F0A0, 0x004AF7B0 and the landed runtime-data block; image tables 0x005349C0/0x00534C38/0x0054E8A8 must exist once. Runtime UNOBSERVED.
#include "igi/retail/core/native_0048f0e0.hpp"
#include "igi/retail/core/retail_error.hpp"
#include <cstdint>

extern "C" {

// Declared once by the landed runtime-data block:
void* g_a7b4f0;
std::int32_t g_a7b638;
void* g_a7b2e0[16];
void* g_a7b4e0[16];
std::int32_t g_a7b63c;

// Image data: the only three constants this native needs (byte-exact, file offset = VA - 0x400000).
static std::uint8_t const g_iid_idirectinputdevice_5349c0[16] = {
    0x82, 0xE6, 0x44, 0x59, 0x2E, 0xC9, 0xCF, 0x11, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00};
static std::uint8_t const g_format_joystick_534c38[0x18] = {
    0x18, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x50, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static std::int32_t g_axis_pairs_54e8a8[16] = {
    0x0C, 1, 0x10, 1, 0x14, 1, 0x00, 1, 0x04, 1, 0x08, 1, 0x18, 1, 0x1C, 1}; // writable: values are cleared below

std::int32_t __stdcall Joypad_EnumDevicesCallback_509A40(void* device_instance, void* unused)
{
    (void)unused; // retail never reads the pvRef argument
    std::uint32_t guid[4];
    std::uint32_t const* src = reinterpret_cast<std::uint32_t const*>(static_cast<std::uint8_t const*>(device_instance) + 4);
    guid[0] = src[0]; // 0x00509A4E: dword copy of lpddi->guidInstance (4 x dword)
    guid[1] = src[1];
    guid[2] = src[2];
    guid[3] = src[3];

    std::int32_t count = g_a7b638;
    void* const io = g_a7b4f0;
    void* const* const io_vtable = *reinterpret_cast<void* const* const*>(io);
    typedef std::int32_t (__stdcall* CreateDeviceFn)(void*, void const*, void**, void*);
    std::int32_t const create_hr = reinterpret_cast<CreateDeviceFn>(io_vtable[3])(io, guid, &g_a7b2e0[count], 0); // [ecx+0xC]
    if (create_hr < 0)
    {
        Debug_Error("Failed to create device."); // 0x00509CAA
        for (;;)
        {
        }
    }

    count = g_a7b638;
    void* const raw_device = g_a7b2e0[count];
    void* const* const raw_vtable = *reinterpret_cast<void* const* const*>(raw_device);
    typedef std::int32_t (__stdcall* QueryInterfaceFn)(void*, void const*, void**);
    typedef std::int32_t (__stdcall* ReleaseFn)(void*);
    std::int32_t const qi_hr = reinterpret_cast<QueryInterfaceFn>(raw_vtable[0])(raw_device, g_iid_idirectinputdevice_5349c0, &g_a7b4e0[count]); // [edx]
    reinterpret_cast<ReleaseFn>(raw_vtable[2])(raw_device); // 0x00509AC2: release raw device before testing the QI result
    if (qi_hr < 0)
    {
        Debug_Error("Failed to obtain interface."); // 0x00509AC9
        for (;;)
        {
        }
    }

    count = g_a7b638;
    void* const device = g_a7b4e0[count];
    void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
    typedef std::int32_t (__stdcall* SetDataFormatFn)(void*, void const*);
    if (reinterpret_cast<SetDataFormatFn>(device_vtable[0x2C / 4])(device, g_format_joystick_534c38) < 0) // [ecx+0x2C]
    {
        reinterpret_cast<ReleaseFn>(device_vtable[2])(device); // 0x00509B04
        g_a7b4e0[g_a7b638] = 0;                              // 0x00509B14 (0x509B14 is inside this native)
        return 1;
    }

    count = g_a7b638;
    void* const coop_device = g_a7b4e0[count];
    void* const* const coop_vtable = *reinterpret_cast<void* const* const*>(coop_device);
    typedef std::int32_t (__stdcall* SetCooperativeLevelFn)(void*, void*, std::uint32_t);
    if (reinterpret_cast<SetCooperativeLevelFn>(coop_vtable[0x34 / 4])(coop_device, GetWindowHandle_48F0A0(), 5) < 0) // [ebx+0x34]
    {
        reinterpret_cast<ReleaseFn>(coop_vtable[2])(coop_device); // 0x00509B50
        g_a7b4e0[g_a7b638] = 0;                                 // 0x00509B5A
        return 1;
    }

    typedef std::int32_t (__stdcall* SetPropertyFn)(void*, std::uint32_t, void const*);
    std::uint32_t range[6];
    range[0] = 0x18;      // dwSize
    range[1] = 0x10;      // dwHeaderSize
    range[3] = 1;         // dwHow = DIPH_BYOFFSET
    range[4] = 0xFFFFFF80; // lMin -128
    range[5] = 0x00000080; // lMax  128
    for (std::int32_t pair = 0; pair < 8; ++pair)
    {
        range[2] = static_cast<std::uint32_t>(g_axis_pairs_54e8a8[pair * 2]); // dwObj = axis offset
        count = g_a7b638;
        void* const prop_device = g_a7b2e0[count]; // property calls use the raw device slot
        void* const* const prop_vtable = *reinterpret_cast<void* const* const*>(prop_device);
        std::int32_t const prop_hr = reinterpret_cast<SetPropertyFn>(prop_vtable[0x18 / 4])(prop_device, 4, range); // [ecx+0x18]
        if (prop_hr != 0)
        {
            if (prop_hr != 0x80070002) // DIERR_NOTFOUND is expected for axes without range support
            {
                count = g_a7b638;
                void* const fail_device = g_a7b4e0[count];
                void* const* const fail_vtable = *reinterpret_cast<void* const* const*>(fail_device);
                reinterpret_cast<ReleaseFn>(fail_vtable[2])(fail_device); // 0x509C95 -> 0x509B01
                g_a7b4e0[g_a7b638] = 0;
                return 1;
            }
            g_axis_pairs_54e8a8[pair * 2 + 1] = 0; // 0x00509BCB: clear the support flag in the writable table
        }
    }

    std::uint32_t pdw[5];
    pdw[0] = 0x14; // dwSize
    pdw[1] = 0x10; // dwHeaderSize
    pdw[2] = 0;    // dwObj
    pdw[3] = 0;    // dwHow = DIPH_DEVICE
    pdw[4] = 1;    // dwData (DIPROP_AUTOCENTER)
    count = g_a7b638;
    void* const ac_device = g_a7b2e0[count];
    reinterpret_cast<SetPropertyFn>((*reinterpret_cast<void* const* const*>(ac_device))[0x18 / 4])(ac_device, 9, pdw); // [ecx+0x18], result ignored

    pdw[4] = 0x2710; // DIPROP_SATURATION = 10000
    count = g_a7b638;
    void* const sat_device = g_a7b2e0[count];
    reinterpret_cast<SetPropertyFn>((*reinterpret_cast<void* const* const*>(sat_device))[0x18 / 4])(sat_device, 6, pdw); // result ignored

    pdw[3] = 1;              // dwHow = DIPH_BYOFFSET
    pdw[4] = static_cast<std::uint32_t>(g_a7b63c); // dwData = [0x00A7B63C]
    for (std::int32_t pair = 0; pair < 8; ++pair)
    {
        if (g_axis_pairs_54e8a8[pair * 2 + 1] == 0) // skip axes whose RANGE was not found
        {
            continue;
        }
        pdw[2] = static_cast<std::uint32_t>(g_axis_pairs_54e8a8[pair * 2]);
        count = g_a7b638;
        void* const dz_device = g_a7b2e0[count];
        void* const* const dz_vtable = *reinterpret_cast<void* const* const*>(dz_device);
        if (reinterpret_cast<SetPropertyFn>(dz_vtable[0x18 / 4])(dz_device, 5, pdw) != 0) // [ecx+0x18]
        {
            count = g_a7b638;
            void* const fail_device = g_a7b4e0[count];
            void* const* const fail_vtable = *reinterpret_cast<void* const* const*>(fail_device);
            reinterpret_cast<ReleaseFn>(fail_vtable[2])(fail_device);
            g_a7b4e0[g_a7b638] = 0;
            return 1;
        }
    }

    g_a7b638 = g_a7b638 + 1; // 0x00509C84
    return 1;
}

} // extern "C"
```

## 2. 0x509980 joypad create+enumerate - 185 B `0x00509980-0x00509A38`, 14 blocks, no args, returns `g_A7B638`

- Image: none directly (curated by 0x509A40 and the thunk).
- Globals: `g_a7b638`, `g_a7b4f0`, `g_a7b4e0[16]` (all declared); `g_a7b430` caps block 16*0x2C at 0x00A7B430 - NOT yet declared.
- Callees: `0x0048F090 GetInstanceHandle_48F090`; `0x004AF740` DirectInputCreateA import thunk; `0x004AF7B0 Debug_Error`; `0x00509CC0 Joypad_IsAcquired_509CC0`; `0x00509F00 Joypad_InitActionFlags_509F00`; callback `0x00509A40`.
- Literals: `0x0054E950` `43 6f 75 6c 64 20 6e 6f 74 20 63 72 65 61 74 65 20 44 69 72 65 63 74 49 6e 70 75 74 20 6a 6f 79 70 61 64 20 6f 62 6a 65 63 74 2e` "Could not create DirectInput joypad object."; `0x0054E920` `43 6f 75 6c 64 20 6e 6f 74 20 61 63 71 75 69 72 65 20 6f 6e 65 20 6f 72 20 6d 6f 72 65 20 6a 6f 79 73 74 69 63 6b 20 64 65 76 69 63 65 73 2e` "Could not acquire one or more joystick devices."; `0x0054E8E8` `43 6f 75 6c 64 20 6e 6f 74 20 67 65 74 20 63 61 70 61 62 69 6c 69 74 69 65 73 20 66 6f 72 20 6f 6e 65 20 6f 72 20 6d 6f 72 65 20 6a 6f 79 73 74 69 63 6b 73 20 2e` "Could not get capabilities for one or more joysticks ." (space before period).

```cpp
// Native: Joypad_Init_509980 (placeholder until VERIFIED; BN alias Physics_CouldNotCreateDirect is wrong).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00509980-0x00509A38.
// Tools/evidence: Binary Ninja MCP disassembly/decompile, radare2 6.2.0 bytes/disassembly; 185 bytes, 14 blocks, no arguments.
// Behavior/CFG: g_A7B638=0; DirectInputCreateA(hinst,0x700,&g_A7B4F0,0); EnumDevices(4, 0x509A40, 0, 1); if no devices return 0;
//           per device: 0x509CC0 acquire check, GetCapabilities into g_A7B430+index*0x2C with dwSize=0x2C, 0x509F00; return count.
//           Every failure calls Debug_Error and spins forever.
// Status/limits: TRANSCRIBED; deps 0x0048F090, 0x004AF740, 0x004AF7B0, 0x00509A40, 0x00509CC0, 0x00509F00. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/native_48f090.hpp"
#include <cstdint>

extern "C" {

void* g_a7b4f0;
std::int32_t g_a7b638;
void* g_a7b4e0[16];
std::uint8_t g_a7b430[16 * 0x2C];

extern "C" __declspec(dllimport) std::int32_t __stdcall DirectInputCreateA(
    void* hinst, std::uint32_t version, void** out, void* outer);

extern "C" std::int32_t __stdcall Joypad_EnumDevicesCallback_509A40(void* device_instance, void* unused);
extern "C" std::int32_t NATIVE_DECL Joypad_IsAcquired_509CC0(std::int32_t index);
extern "C" void NATIVE_DECL Joypad_InitActionFlags_509F00(std::int32_t index);

std::int32_t NATIVE_DECL Joypad_Init_509980()
{
    g_a7b638 = 0;
    std::int32_t const create_hr = DirectInputCreateA(
        GetInstanceHandle_48F090(), 0x700, &g_a7b4f0, nullptr); // 0x0050999A thunk 0x004AF740
    if (create_hr != 0)
    {
        Debug_Error("Could not create DirectInput joypad object."); // 0x005099A3
        for (;;)
        {
        }
    }

    void* const io = g_a7b4f0;
    void* const* const io_vtable = *reinterpret_cast<void* const* const*>(io);
    typedef std::int32_t (__stdcall* EnumDevicesFn)(void*, std::uint32_t, void*, void*, std::uint32_t);
    reinterpret_cast<EnumDevicesFn>(io_vtable[0x10 / 4])(
        io, 4, reinterpret_cast<void*>(&Joypad_EnumDevicesCallback_509A40), nullptr, 1); // [ecx+0x10] at 0x005099C6

    std::int32_t count = g_a7b638;
    if (count == 0)
    {
        return count; // retail returns eax = g_A7B638 = 0
    }

    std::int32_t index = 0;
    std::uint8_t* caps = g_a7b430;
    do
    {
        if (Joypad_IsAcquired_509CC0(index) == 0) // 0x005099DF
        {
            Debug_Error("Could not acquire one or more joystick devices."); // 0x00509A1B
            for (;;)
            {
            }
        }
        *reinterpret_cast<std::uint32_t*>(caps) = 0x2C; // 0x005099ED: DIDEVCAPS.dwSize
        void* const device = g_a7b4e0[index];
        void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
        typedef std::int32_t (__stdcall* GetCapabilitiesFn)(void*, void*);
        if (reinterpret_cast<GetCapabilitiesFn>(device_vtable[0x0C / 4])(device, caps) != 0) // [edx+0x0C] at 0x005099F7
        {
            Debug_Error("Could not get capabilities for one or more joysticks ."); // 0x00509A2A
            for (;;)
            {
            }
        }
        Joypad_InitActionFlags_509F00(index); // 0x005099FF
        count = g_a7b638;
        ++index;
        caps += 0x2C;
    } while (index < count);

    return g_a7b638; // 0x00509A1A ret, eax = count
}

} // extern "C"
```

## 3. 0x48FFF0 keyboard creation - 294 B `0x0048FFF0-0x00490115`, 15 blocks, `void()`

- Image: `0x00534900` GUID_SysKeyboard (16 B) `61 2b 1d 6f a0 d5 cf 11 bf c7 44 45 53 54 00 00`; `0x005348C8` DIDATAFORMAT (0x18 B) `18 00 00 00 10 00 00 00 02 00 00 00 00 01 00 00 00 01 00 00 40 e7 4a 00` (dwDataSize=0x100).
- Globals: `g_5c8e28` void* (0x005C8E28), `g_5c8e24` void* (0x005C8E24), `g_5c8e30` int32 (0x005C8E30) - all zero-fill runtime data, NOT yet declared; `g_c28e80` uint8[256] declared; `g_c28c60` declared (display_state_flags.hpp).
- Callees: `0x0048F090 GetInstanceHandle_48F090`; `0x004AF740` DirectInputCreateA import thunk; `0x004AF7B0 Debug_Error`; `0x0048F0A0 GetWindowHandle_48F0A0`; `0x00490340 Display_SetVSyncFlag_490340`; `0x00490350 Display_SetSwapFlag_490350`; `0x0048F490 WinProc_RegisterHandler_48F490`; `0x00490120 Keyboard_WinProc_490120` (registered, declared in native_490120.cpp).
- Literals: `0x00542274` `43 6f 75 6c 64 20 6e 6f 74 20 63 72 65 61 74 65 20 44 69 72 65 63 74 49 6e 70 75 74 20 6b 65 79 62 6f 61 72 64 20 6f 62 6a 65 63 74 21` "Could not create DirectInput keyboard object!"; `0x00542244` `43 6f 75 6c 64 20 6e 6f 74 20 63 72 65 61 74 65 20 44 69 72 65 63 74 49 6e 70 75 74 20 64 65 76 69 63 65 20 6b 65 79 62 6f 61 72 64 21` "Could not create DirectInput device keyboard!"; `0x0054221C` `43 6f 75 6c 64 20 6e 6f 74 20 73 65 74 20 64 61 74 61 20 66 6f 72 6d 61 74 20 66 6f 72 20 6b 65 79 62 6f 61 72 64 21` "Could not set data format for keyboard!"; `0x0054216C` `43 6f 75 6c 64 20 6e 6f 74 20 73 65 74 20 63 6f 6f 70 65 72 61 74 69 76 65 20 6c 65 76 65 6c 20 66 6f 72 20 6b 65 79 62 6f 61 72 64 21` "Could not set cooperative level for keyboard!".

```cpp
// Native: Keyboard_Create_48FFF0 (placeholder until VERIFIED); retail VA 0x0048FFF0-0x00490115 (294 bytes, 15 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile/callees; DINPUT-BODIES packet section 3.
// Behavior/CFG: zero g_5C8E28/g_5C8E24; DirectInputCreateA(GetInstanceHandle_48F090(),0x700,&g_5C8E28,0);
//           CreateDevice(&GUID_SysKeyboard 0x00534900, &g_5C8E24, 0); SetDataFormat(0x005348C8);
//           SetCooperativeLevel(hWnd,6); Acquire and latch g_5C8E30 when it returns 0; Display_SetVSyncFlag_490340();
//           Display_SetSwapFlag_490350(0); zero 0x100 bytes at g_C28E80; WinProc_RegisterHandler_48F490(0x00490120).
//           Every DirectInput/COM failure calls Debug_Error and spins forever.
// Status/limits: TRANSCRIBED; engine deps 0x0048F090/0x0048F0A0/0x0048F490/0x00490340/0x00490350/0x004AF7B0 + thunk
//           0x004AF740; g_5C8E24/g_5C8E28/g_5C8E30 still need declarations. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/dinput_globals.hpp"
#include "igi/retail/input/native_48f090.hpp"
#include "igi/retail/input/native_48f0a0.hpp"
#include "igi/retail/render/display_state_flags.hpp"

#include <cstdint>

extern "C" {

void* g_5c8e24;        // retail 0x005C8E24, IDirectInputDeviceA* (keyboard)
void* g_5c8e28;        // retail 0x005C8E28, IDirectInputA* (keyboard)
std::int32_t g_5c8e30; // retail 0x005C8E30, keyboard-acquired flag

std::int32_t Keyboard_WinProc_490120(void*, std::uint32_t, std::uint32_t, std::int32_t);
std::int32_t __stdcall DirectInputCreateA(void* hinst, std::uint32_t version, void** out, void* outer); // thunk 0x004AF740
void WinProc_RegisterHandler_48F490(void* proc);

void NATIVE_DECL Keyboard_Create_48FFF0()
{
    std::uint32_t guid[4];
    // 0x0048FFF3-0x00490021: guidInstance copied from GUID_SysKeyboard 0x00534900 as four dwords into the stack local.
    std::uint32_t const* const system_keyboard = reinterpret_cast<std::uint32_t const*>(0x00534900);
    guid[0] = system_keyboard[0];
    guid[1] = system_keyboard[1];
    guid[2] = system_keyboard[2];
    guid[3] = system_keyboard[3];

    g_5c8e28 = nullptr;
    g_5c8e24 = nullptr;
    // 0x00490039-0x0049003F: DirectInputCreateA(hinst, 0x700, &g_5C8E28, 0).
    if (DirectInputCreateA(GetInstanceHandle_48F090(), 0x700, &g_5c8e28, nullptr) != 0)
    {
        Debug_Error("Could not create DirectInput keyboard object!"); // 0x00490048
        for (;;)
        {
        }
    }

    typedef std::int32_t (__stdcall* CreateDeviceFn)(void*, void const*, void**, void*);
    typedef std::int32_t (__stdcall* SetDataFormatFn)(void*, void const*);
    typedef std::int32_t (__stdcall* SetCooperativeLevelFn)(void*, void*, std::uint32_t);
    typedef std::int32_t (__stdcall* AcquireFn)(void*);

    void* const direct_input = g_5c8e28;
    void* const* const di_vtable = *reinterpret_cast<void* const* const*>(direct_input);
    // 0x00490057-0x0049006B: IDirectInputA::CreateDevice at vtable +0x0C.
    if (reinterpret_cast<CreateDeviceFn>(di_vtable[0x0C / 4])(direct_input, guid, &g_5c8e24, nullptr) != 0)
    {
        Debug_Error("Could not create DirectInput device keyboard!"); // 0x00490072
        for (;;)
        {
        }
    }

    void* const device = g_5c8e24;
    void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
    if (reinterpret_cast<SetDataFormatFn>(device_vtable[0x2C / 4])(device, reinterpret_cast<void const*>(0x005348C8)) != 0)
    {
        Debug_Error("Could not set data format for keyboard!"); // 0x00490095
        for (;;)
        {
        }
    }
    if (reinterpret_cast<SetCooperativeLevelFn>(device_vtable[0x34 / 4])(
            device, GetWindowHandle_48F0A0(), 6) != 0)
    {
        Debug_Error("Could not set cooperative level for keyboard!"); // 0x004900C2
        for (;;)
        {
        }
    }

    // 0x004900D1-0x004900E8: Acquire; the flag is latched only when Acquire returns 0.
    if (reinterpret_cast<AcquireFn>(device_vtable[0x1C / 4])(device) == 0)
    {
        g_5c8e30 = 1;
    }
    Display_SetVSyncFlag_490340();  // 0x004900EA
    Display_SetSwapFlag_490350(0);  // 0x004900EF-0x004900F1
    for (std::int32_t i = 0; i < 0x40; ++i)
    {
        reinterpret_cast<std::uint32_t*>(g_c28e80)[i] = 0; // 0x004900F6-0x00490107: rep stosd 0x40 dwords
    }
    WinProc_RegisterHandler_48F490(reinterpret_cast<void*>(&Keyboard_WinProc_490120)); // push 0x00490120, call 0x0048F490
}

} // extern "C"
```

## 4. 0x48FDE0 mouse creation - 226 B `0x0048FDE0-0x0048FEC1`, 13 blocks, `void()`

- Image: `0x00534910` GUID_SysMouse (16 B) `60 2b 1d 6f a0 d5 cf 11 bf c7 44 45 53 54 00 00`; `0x005349D0` IID_IDirectInput7A (16 B) `84 b6 4c 9a 6d 23 d3 11 8e 9d 00 c0 4f 68 44 ae`; `0x005349B0` IID_IDirectInputDevice2A (16 B) `bc c6 d7 57 56 23 d3 11 8e 9d 00 c0 4f 68 44 ae`; `0x005348B0` DIDATAFORMAT (0x18 B) `18 00 00 00 10 00 00 00 02 00 00 00 14 00 00 00 0b 00 00 00 80 e6 4a 00` (dwDataSize=0x14).
- Globals: `g_5c8e14` void* (0x005C8E14), `g_5c8e1c` void* (declared), `g_5c8e10/0c/08` int32 (declared).
- Callees: `0x0048F090 GetInstanceHandle_48F090`; `0x004AE730` DirectInputCreateEx import thunk; `0x004AF7B0 Debug_Error`; `0x0048F0A0 GetWindowHandle_48F0A0`; `0x0048FD90 Input_MouseGetCaps_48FD90`; `0x0048F490 WinProc_RegisterHandler_48F490`; `0x0048FED0 Mouse_WinProc_48FED0` (registered).
- Literals: `0x005421F0` `43 6f 75 6c 64 20 6e 6f 74 20 63 72 65 61 74 65 20 44 69 72 65 63 74 49 6e 70 75 74 20 6d 6f 75 73 65 20 6f 62 6a 65 63 74 21` "Could not create DirectInput mouse object!"; `0x005421C4` `43 6f 75 6c 64 20 6e 6f 74 20 63 72 65 61 74 65 20 44 69 72 65 63 74 49 6e 70 75 74 20 64 65 76 69 63 65 20 6d 6f 75 73 65 21` "Could not create DirectInput device mouse!"; `0x0054219C` `43 6f 75 6c 64 20 6e 6f 74 20 73 65 74 20 64 61 74 61 20 66 6f 72 6d 61 74 20 66 6f 72 20 6d 6f 75 73 65 21` "Could not set data format for mouse!"; cooperative-level failure reuses the keyboard literal `0x0054216C` `43 6f 75 6c 64 20 6e 6f 74 20 73 65 74 20 63 6f 6f 70 65 72 61 74 69 76 65 20 6c 65 76 65 6c 20 66 6f 72 20 6b 65 79 62 6f 61 72 64 21` "Could not set cooperative level for keyboard!" (retail pushes 0x0054216C here too).

```cpp
// Native: Mouse_Create_48FDE0 (placeholder until VERIFIED); retail VA 0x0048FDE0-0x0048FEC1 (226 bytes, 13 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile/callees; DINPUT-BODIES packet section 4.
// Behavior/CFG: zero g_5C8E14/g_5C8E1C; DirectInputCreateEx(hinst,0x700,IID_IDirectInput7A 0x005349D0,&g_5C8E14,0);
//           IDirectInput7A::CreateDeviceEx(GUID_SysMouse 0x00534910, IID_IDirectInputDevice2A 0x005349B0, &g_5C8E1C, 0);
//           SetDataFormat(0x005348B0); SetCooperativeLevel(hWnd,6); Input_MouseGetCaps_48FD90(); zero g_5C8E10/0C/08;
//           WinProc_RegisterHandler_48F490(0x0048FED0). Every failure calls Debug_Error and spins forever;
//           the cooperative-level failure string is the keyboard one (retail 0x0054216C), not a mouse variant.
// Status/limits: TRANSCRIBED; engine deps 0x0048F090/0x0048F0A0/0x0048F490/0x0048FD90/0x004AF7B0 + thunk 0x004AE730;
//           g_5C8E14 still needs a declaration. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/dinput_globals.hpp"
#include "igi/retail/input/native_48f090.hpp"
#include "igi/retail/input/native_48f0a0.hpp"

#include <cstdint>

extern "C" {

void* g_5c8e14; // retail 0x005C8E14, IDirectInput7A* (mouse)

std::int32_t Mouse_WinProc_48FED0(void*, std::uint32_t, std::uint32_t, std::uint32_t);
std::int32_t __stdcall DirectInputCreateEx(void* hinst, std::uint32_t version, void const* riid, void** out, void* outer); // thunk 0x004AE730
void WinProc_RegisterHandler_48F490(void* proc);
void Input_MouseGetCaps_48FD90();

void NATIVE_DECL Mouse_Create_48FDE0()
{
    g_5c8e14 = nullptr;
    g_5c8e1c = nullptr;
    // 0x0048FDFF-0x0048FE05: DirectInputCreateEx(hinst, 0x700, IID_IDirectInput7A, &g_5C8E14, 0).
    if (DirectInputCreateEx(GetInstanceHandle_48F090(), 0x700,
            reinterpret_cast<void const*>(0x005349D0), &g_5c8e14, nullptr) != 0)
    {
        Debug_Error("Could not create DirectInput mouse object!"); // 0x0048FE0E
        for (;;)
        {
        }
    }

    typedef std::int32_t (__stdcall* CreateDeviceExFn)(void*, void const*, void const*, void**, void*);
    typedef std::int32_t (__stdcall* SetDataFormatFn)(void*, void const*);
    typedef std::int32_t (__stdcall* SetCooperativeLevelFn)(void*, void*, std::uint32_t);

    void* const direct_input = g_5c8e14;
    void* const* const di_vtable = *reinterpret_cast<void* const* const*>(direct_input);
    // 0x0048FE1D-0x0048FE35: IDirectInput7A::CreateDeviceEx at vtable +0x24.
    if (reinterpret_cast<CreateDeviceExFn>(di_vtable[0x24 / 4])(direct_input,
            reinterpret_cast<void const*>(0x00534910), reinterpret_cast<void const*>(0x005349B0),
            &g_5c8e1c, nullptr) != 0)
    {
        Debug_Error("Could not create DirectInput device mouse!"); // 0x0048FE3C
        for (;;)
        {
        }
    }

    void* const device = g_5c8e1c;
    void* const* const device_vtable = *reinterpret_cast<void* const* const*>(device);
    if (reinterpret_cast<SetDataFormatFn>(device_vtable[0x2C / 4])(device, reinterpret_cast<void const*>(0x005348B0)) != 0)
    {
        Debug_Error("Could not set data format for mouse!"); // 0x0048FE5F
        for (;;)
        {
        }
    }
    if (reinterpret_cast<SetCooperativeLevelFn>(device_vtable[0x34 / 4])(
            device, GetWindowHandle_48F0A0(), 6) != 0)
    {
        Debug_Error("Could not set cooperative level for keyboard!"); // 0x0048FE8D, byte-identical keyboard literal
        for (;;)
        {
        }
    }

    Input_MouseGetCaps_48FD90(); // 0x0048FE9C
    g_5c8e10 = 0;               // 0x0048FEA6
    g_5c8e0c = 0;               // 0x0048FEAC
    g_5c8e08 = 0;               // 0x0048FEB2
    WinProc_RegisterHandler_48F490(reinterpret_cast<void*>(&Mouse_WinProc_48FED0)); // push 0x0048FED0, call 0x0048F490
}

} // extern "C"
```

## 5. 0x490230 Keyboard_Update - 177 B `0x00490230-0x004902E1`, 11 blocks, `int32_t()`

- Image: none.
- Globals: `g_5c8e30` int32 (0x005C8E30); `g_5c8e24` void* (0x005C8E24); `g_c28c60` uint8 (declared); `g_c28c62` uint8[256] (0x00C28C62); `g_c28d62` uint8[256] (0x00C28D62); `g_c28e80` uint8[256] (declared). Only `g_5c8e24/g_5c8e30/g_c28c62/g_c28d62` still need declarations (zero-fill).
- Callees: `0x0048F270 Input_GetInstance_48F270`; COM via vtables (Acquire +0x1C, GetDeviceState +0x24).
- Literals: none.

```cpp
// Native: Keyboard_Update_490230 (placeholder until VERIFIED); retail VA 0x00490230-0x004902E1 (177 bytes, 11 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile; DINPUT-BODIES packet section 5.
// Behavior/CFG: if g_5C8E30 is clear, Acquire the keyboard and latch g_5C8E30 on success; snapshot C28C62 into C28D62;
//           if g_C28C60 is set clear C28C62 and return 0; else if Input_GetInstance_48F270() != 0 copy g_C28E80 over
//           C28C62 and return it; else GetDeviceState(0x100, C28C62), on failure re-Acquire then retry once,
//           clearing C28C62 after either failed attempt; return the last GetDeviceState result (0 after a clear).
// Status/limits: TRANSCRIBED; deps 0x0048F270 and the COM interface only; g_5C8E24/g_5C8E30/g_C28C62/g_C28D62 need
//           declarations. Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"
#include "igi/retail/input/native_48f270.hpp"
#include "igi/retail/render/display_state_flags.hpp"

#include <cstdint>

extern "C" {

void* g_5c8e24;              // retail 0x005C8E24, IDirectInputDeviceA* (keyboard)
std::int32_t g_5c8e30;       // retail 0x005C8E30, keyboard-acquired flag
std::uint8_t g_c28c62[256];  // retail 0x00C28C62, current keyboard scan state
std::uint8_t g_c28d62[256];  // retail 0x00C28D62, previous keyboard scan state

std::int32_t NATIVE_DECL Keyboard_Update_490230()
{
    typedef std::int32_t (__stdcall* AcquireFn)(void*);
    typedef std::int32_t (__stdcall* GetDeviceStateFn)(void*, std::int32_t, void*);

    // 0x00490230-0x0049024E: no Acquire attempt while the acquired flag is set.
    if (g_5c8e30 == 0)
    {
        void* const device = g_5c8e24;
        void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
        if (reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device) == 0)
        {
            g_5c8e30 = 1;
        }
    }

    // 0x00490254-0x00490264: previous := current (0x100 bytes).
    for (std::int32_t i = 0; i < 0x40; ++i)
    {
        reinterpret_cast<std::uint32_t*>(g_c28d62)[i] = reinterpret_cast<std::uint32_t const*>(g_c28c62)[i];
    }
    // 0x00490267-0x00490279: disable byte forces an empty state and returns 0.
    if (g_c28c60 != 0)
    {
        for (std::int32_t i = 0; i < 0x40; ++i)
        {
            reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
        }
        return 0;
    }
    // 0x0049027F-0x0049029A: non-zero instance handle uses the win-proc-written scan state.
    std::int32_t result = Input_GetInstance_48F270();
    if (result != 0)
    {
        for (std::int32_t i = 0; i < 0x40; ++i)
        {
            reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = reinterpret_cast<std::uint32_t const*>(g_c28e80)[i];
        }
        return result;
    }
    // 0x0049029E-0x004902DB: DirectInput state; one retry after a re-Acquire.
    void* const device = g_5c8e24;
    void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
    result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x100, g_c28c62);
    if (result != 0)
    {
        if (reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device) != 0)
        {
            for (std::int32_t i = 0; i < 0x40; ++i)
            {
                reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
            }
            result = 0;
        }
        else
        {
            result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x100, g_c28c62);
            if (result != 0)
            {
                for (std::int32_t i = 0; i < 0x40; ++i)
                {
                    reinterpret_cast<std::uint32_t*>(g_c28c62)[i] = 0;
                }
                result = 0;
            }
        }
    }
    return result;
}

} // extern "C"
```

## 6. 0x48FC20 Mouse_Update - 359 B `0x0048FC20-0x0048FD87`, 38 blocks, `int32_t()`

- Image: `0x00534258` double 1/255 `10 10 10 10 10 10 70 3f` (look-delta scale); `0x005333EC` float 0.0 `00 00 00 00` (used by the joypad body, not here).
- Globals: `g_5c8e1c` void* (declared); `g_c28f84` float (0x00C28F84), `g_c28f88` float (0x00C28F88), `g_c28f8c` int32 (0x00C28F8C) - NOT yet declared; `g_5c8e10/0c/18/08` int32 (declared).
- Callees: `0x0048F270 Input_GetInstance_48F270`; `0x0048FA80 DirectInput_ErrorText_48FA80`; `0x004AF7B0 Debug_Error`; COM via vtables (GetDeviceState +0x24, Acquire +0x1C).
- Literals: `0x00542134` `46 61 69 6c 65 64 20 67 65 74 20 6d 6f 75 73 65 20 73 74 61 74 65 3a 20 25 73` "Failed get mouse state: %s".

```cpp
// Native: Mouse_Update_48FC20 (placeholder until VERIFIED); retail VA 0x0048FC20-0x0048FD87 (359 bytes, 38 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile/callees; DINPUT-BODIES packet section 6.
// Behavior/CFG: GetDeviceState(0x14, DIMOUSESTATE2); on failure re-Acquire, retry once, then Debug_Error("Failed get mouse
//           state: %s", 0x48FA80(hr)) and spin; store lX/255 and lY/255 as floats at 0xC28F84/0xC28F88; then either the
//           win-proc button/wheel path (when Input_GetInstance_48F270() != 0: bits 0/1/2, wheel +/- -> 0x100/0x200,
//           g_5C8E08 adjusted by one per frame and the adjusted value returned) or the DirectInput path (8 button bytes ->
//           bits 0..7, lZ > 0 -> 0x100, lZ < 0 -> 0x200, g_5C8E08 untouched, lZ returned). Result dword -> 0xC28F8C.
// Status/limits: TRANSCRIBED; deps 0x0048F270/0x0048FA80/0x004AF7B0 and the COM interface; g_C28F84/88/8C need
//           declarations. Runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/input/dinput_globals.hpp"
#include "igi/retail/input/native_48f270.hpp"

#include <cstdint>

extern "C" {

float g_c28f84;        // retail 0x00C28F84, mouse lX / 255
float g_c28f88;        // retail 0x00C28F88, mouse lY / 255
std::int32_t g_c28f8c; // retail 0x00C28F8C, assembled mouse button/wheel mask

const char* DirectInput_ErrorText_48FA80(std::int32_t error);

std::int32_t NATIVE_DECL Mouse_Update_48FC20()
{
    typedef std::int32_t (__stdcall* GetDeviceStateFn)(void*, std::int32_t, void*);
    typedef std::int32_t (__stdcall* AcquireFn)(void*);

    std::uint8_t state[0x14]; // DIMOUSESTATE2: LONG lX, lY, lZ; BYTE rgbButtons[8]
    void* const device = g_5c8e1c;
    void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
    std::int32_t result = 0;
    result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x14, state);
    if (result != 0)
    {
        if (reinterpret_cast<AcquireFn>(vtable[0x1C / 4])(device) != 0)
        {
            return result; // 0x0048FC4F: Acquire failure returns its HRESULT unchanged
        }
        result = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(device, 0x14, state);
        if (result != 0)
        {
            Debug_Error("Failed get mouse state: %s", DirectInput_ErrorText_48FA80(result)); // 0x0048FC65-0x0048FC76
            for (;;)
            {
            }
        }
    }

    // 0x0048FC7B-0x0048FC95: fild/fmul qword 0x00534258 (=1/255), stored as float.
    g_c28f84 = static_cast<float>(*reinterpret_cast<std::int32_t const*>(state)) * static_cast<float>(0.0039215686274509803);
    g_c28f88 = static_cast<float>(*reinterpret_cast<std::int32_t const*>(state + 4)) * static_cast<float>(0.0039215686274509803);

    if (Input_GetInstance_48F270() != 0)
    {
        // 0x0048FCA4-0x0048FCF3: win-proc-written buttons and the wheel accumulator.
        result = 0;
        if (g_5c8e10 != 0)
        {
            result = 1;
        }
        if (g_5c8e0c != 0)
        {
            result |= 2;
        }
        if (g_5c8e18 != 0)
        {
            result |= 4;
        }
        std::int32_t wheel = g_5c8e08;
        if (wheel > 0)
        {
            result |= 0x100;
            --wheel;
            g_5c8e08 = wheel;
            if (wheel >= 0)
            {
                g_c28f8c = result;
                return wheel; // 0x0048FCE1 -> 0x0048FD7C
            }
        }
        if (wheel < 0)
        {
            result |= 0x200;
            ++wheel;
            g_5c8e08 = wheel; // 0x0048FCE7-0x0048FCEE
        }
        g_c28f8c = result;
        return wheel;
    }

    // 0x0048FCFE-0x0048FD74: DirectInput buttons and lZ axis.
    result = 0;
    std::uint8_t const* const buttons = state + 0x0C;
    if (buttons[0] != 0) { result = 1; }
    if (buttons[1] != 0) { result |= 2; }
    if (buttons[2] != 0) { result |= 4; }
    if (buttons[3] != 0) { result |= 8; }
    if (buttons[4] != 0) { result |= 0x10; }
    if (buttons[5] != 0) { result |= 0x20; }
    if (buttons[6] != 0) { result |= 0x40; }
    if (buttons[7] != 0) { result |= 0x80; }
    std::int32_t const z = *reinterpret_cast<std::int32_t const*>(state + 8);
    if (z > 0)
    {
        result |= 0x100;
        g_c28f8c = result;
        return z; // 0x0048FD63-0x0048FD73
    }
    if (z < 0)
    {
        result |= 0x200;
    }
    g_c28f8c = result;
    return z; // 0x0048FD7C-0x0048FD86
}

} // extern "C"
```

## 7. 0x509CF0 Joypad_Update - 412 B `0x00509CF0-0x00509E8B`, 32 blocks, `void()`

- Image: `0x00534BB0` double 1/128 `00 00 00 00 00 00 80 3f` (axis scale); `0x005333EC` float 0.0 `00 00 00 00` (lRz compare).
- Globals: `g_a7b638` int32 (declared); `g_a7b4e0[16]` (declared); `g_bcaa00` uint8[0x28*16] (declared; this native uses the block base 0x00BCAA20); plus `g_a7b2f0` uint8[16*0x50] (0x00A7B2F0, current DIJOYSTATE2 buffers) and `g_a7b4f8` uint8[16*0x50] (0x00A7B4F8, pre-poll snapshot) - NOT yet declared.
- Callees: `0x00509CC0 Joypad_IsAcquired_509CC0`; COM via vtables (Poll +0x64, GetDeviceState +0x24).
- Literals: none. Error constant: `0x8007001E` DIERR_NOTACQUIRED.

```cpp
// Native: Joypad_Update_509CF0 (placeholder until VERIFIED); retail VA 0x00509CF0-0x00509E8B (412 bytes, 32 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 full bytes/disassembly; Binary Ninja MCP decompile; DINPUT-BODIES packet section 7.
// Behavior/CFG: per device slot (g_A7B4E0[i] != 0): snapshot the 0x50-byte state at 0x00A7B2F0+i*0x50 into 0x00A7B4F8+i*0x50
//           (rep movsd 0x14), Poll the device (+0x64), GetDeviceState(0x50, current); negative result: on
//           0x8007001E call Joypad_IsAcquired_509CC0(i), then zero the 0x50-byte current state. Then per device build the
//           action mask in the 0x00BCAA20 block (32 button bytes at state+0x30 -> bits 4..0x23, X/Y sign -> bits 0..3,
//           clear-on-release byte at +5, axes scaled by 1/128 into floats at 0x00BCAA08/0C, lRz into 0x00BCAA18/1C,
//           sliders raw into 0x00BCAA10/14). Counts are re-read from g_A7B638 after every device.
// Status/limits: TRANSCRIBED; deps 0x00509CC0 and the COM interface; g_A7B2F0/g_A7B4F8 still need declarations.
//           Runtime UNOBSERVED.
#include "igi/retail/input/dinput_globals.hpp"

#include <cstdint>

extern "C" {

std::uint8_t g_a7b2f0[16 * 0x50]; // retail 0x00A7B2F0, current joypad state buffers
std::uint8_t g_a7b4f8[16 * 0x50]; // retail 0x00A7B4F8, pre-poll snapshot buffers

void NATIVE_DECL Joypad_Update_509CF0()
{
    typedef std::int32_t (__stdcall* PollFn)(void*);
    typedef std::int32_t (__stdcall* GetDeviceStateFn)(void*, std::int32_t, void*);

    std::int32_t count = g_a7b638;
    if (count > 0)
    {
        std::int32_t offset = 0;
        std::int32_t index = 0;
        do
        {
            void* const device = g_a7b4e0[index];
            if (device != nullptr)
            {
                std::uint8_t* const current = g_a7b2f0 + offset;
                std::uint8_t* const snapshot = g_a7b4f8 + offset;
                for (std::int32_t i = 0; i < 0x14; ++i) // 0x00509D1D-0x00509D30: rep movsd, 0x50 bytes
                {
                    reinterpret_cast<std::uint32_t*>(snapshot)[i] =
                        reinterpret_cast<std::uint32_t const*>(current)[i];
                }
                void* const* const vtable = *reinterpret_cast<void* const* const*>(device);
                reinterpret_cast<PollFn>(vtable[0x64 / 4])(device); // 0x00509D35
                std::int32_t const hr = reinterpret_cast<GetDeviceStateFn>(vtable[0x24 / 4])(
                    device, 0x50, current); // 0x00509D44
                if (hr < 0)
                {
                    if (hr == 0x8007001E) // DIERR_NOTACQUIRED
                    {
                        Joypad_IsAcquired_509CC0(index); // 0x00509D52-0x00509D5C
                    }
                    for (std::int32_t i = 0; i < 0x14; ++i) // 0x00509D5F-0x00509D68: rep stosd
                    {
                        reinterpret_cast<std::uint32_t*>(current)[i] = 0;
                    }
                }
            }
            count = g_a7b638;
            ++index;
            offset += 0x50;
        } while (index < count);
    }
    if (count <= 0)
    {
        return;
    }

    // 0x00509D8B-0x00509E7E: per-device action block at g_bcaa00 + i*0x28; slot points at block+0x20.
    std::uint8_t* state = g_a7b2f0;
    std::uint8_t* slot = g_bcaa00 + 0x20;
    std::int32_t remaining = count;
    do
    {
        std::uint32_t const previous = *reinterpret_cast<std::uint32_t const*>(slot);
        *reinterpret_cast<std::uint32_t*>(slot + 4) = previous; // 0x00BCAA24
        *reinterpret_cast<std::uint32_t*>(slot) = 0;
        std::uint32_t mask = 0;
        for (std::int32_t i = 0; i < 0x20; ++i) // 0x00509DAC-0x00509DC6: 32 buttons at state+0x30
        {
            std::uint32_t const pressed = (state[0x30 + i] & 0x80U) == 0x80U ? 1U : 0U;
            mask |= pressed << ((i + 4) & 0x1F);
        }
        *reinterpret_cast<std::uint32_t*>(slot) = mask;
        if (slot[-0x1C] != 0) // 0x00BCAA04, per-slot action-enabled byte
        {
            std::int32_t const x = *reinterpret_cast<std::int32_t const*>(state);
            if (x < 0)
            {
                *reinterpret_cast<std::uint32_t*>(slot) |= 1U;
            }
            else if (x > 0)
            {
                *reinterpret_cast<std::uint32_t*>(slot) |= 2U;
            }
            std::int32_t const y = *reinterpret_cast<std::int32_t const*>(state + 4);
            if (y < 0)
            {
                *reinterpret_cast<std::uint32_t*>(slot) |= 4U;
            }
            else if (y > 0)
            {
                *reinterpret_cast<std::uint32_t*>(slot) |= 8U;
            }
        }
        if (slot[-0x1B] != 0) // 0x00BCAA05, release latch
        {
            if (*reinterpret_cast<std::uint32_t const*>(slot) == 0)
            {
                slot[-0x1B] = 0;
            }
            *reinterpret_cast<std::uint32_t*>(slot) = 0;
        }
        std::int32_t const rotation = *reinterpret_cast<std::int32_t const*>(state + 0x14); // lRz
        // 0x00509E10-0x00509E2B: fild/fmul qword 0x00534BB0 (=1/128), stored as float.
        *reinterpret_cast<float*>(slot - 0x18) =
            static_cast<float>(*reinterpret_cast<std::int32_t const*>(state)) * 0.0078125f;
        *reinterpret_cast<float*>(slot - 0x14) =
            static_cast<float>(*reinterpret_cast<std::int32_t const*>(state + 4)) * 0.0078125f;
        // 0x00509E2E-0x00509E64: fcomp dword 0x005333EC; equal (lRz == 0) takes the fchs branch.
        if (rotation == 0)
        {
            *reinterpret_cast<float*>(slot - 0x08) = -0.0f;
            *reinterpret_cast<float*>(slot - 0x04) = 0.0f;
        }
        else
        {
            *reinterpret_cast<float*>(slot - 0x08) = 0.0f;
            *reinterpret_cast<float*>(slot - 0x04) = static_cast<float>(rotation) * 0.0078125f;
        }
        // 0x00509E6B-0x00509E7E: sliders are stored raw (no scale), then the pointers advance.
        *reinterpret_cast<float*>(slot - 0x10) =
            static_cast<float>(*reinterpret_cast<std::int32_t const*>(state + 0x18));
        *reinterpret_cast<float*>(slot - 0x0C) =
            static_cast<float>(*reinterpret_cast<std::int32_t const*>(state + 0x1C));
        state += 0x50;
        slot += 0x28;
        --remaining;
    } while (remaining != 0);
}

} // extern "C"
```

## 8. Fourth quartet member - `0x507EC0 Input_Run` (already landed, nothing to transcribe)

- `engine/src/retail/input/native_507ec0.cpp` exists in the tree: 78 B `0x00507EC0-0x00507F0E`, 8 blocks, status TRANSCRIBED, no native deps. Nothing from this request touches it.
- The quartet caller is `0x4ED560 InputPort_LoadParameters` (5 blocks, bytes `535657e8c82cfaffe8b326faffe87ec701008b5c241033ffbef020bc008b46b085c074105356893d9cefa500e82fa9010083c40881c6b00000004781feb023bc007cda5f5e5bc3`): it calls `0x490230` at `0x004ED563`, `0x48FC20` at `0x004ED568`, `0x509CF0` at `0x004ED56D`, then loops `0x004ED57D-0x004ED5A1` over the input ports at `0x00BC20F0..0x00BC23B0` (stride 0xB0) calling `0x507EC0 Input_Run` at `0x004ED58C` when slot[+0x04] (dword at `0x00BC20A0`) is non-null, with `0x00A5EF9C = port index`.

## 9. Declarations still missing (zero-fill runtime data; add once)

| retail VA | type | used by |
|---|---|---|
| `0x005C8E14` | `void*` | 0x48FDE0 |
| `0x005C8E24` | `void*` | 0x48FFF0, 0x490230 |
| `0x005C8E28` | `void*` | 0x48FFF0 |
| `0x005C8E30` | `int32` | 0x48FFF0, 0x490230 |
| `0x00C28C62` | `uint8[256]` | 0x490230 |
| `0x00C28D62` | `uint8[256]` | 0x490230 |
| `0x00C28F84` | `float` | 0x48FC20 |
| `0x00C28F88` | `float` | 0x48FC20 |
| `0x00C28F8C` | `int32` | 0x48FC20 |
| `0x00A7B2F0` | `uint8[16*0x50]` | 0x509CF0 |
| `0x00A7B4F8` | `uint8[16*0x50]` | 0x509CF0 |
| `0x00A7B63C` | `int32` | 0x509A40 (deadzone value) |
| `0x00A7B430` | `uint8[16*0x2C]` | 0x509980 (DIDEVCAPS block) |

## 10. Blocked bodies

None. Every callee is a landed engine native (`GetInstanceHandle_48F090`, `GetWindowHandle_48F0A0`, `WinProc_RegisterHandler_48F490`, `Display_SetVSyncFlag_490340`, `Display_SetSwapFlag_490350`, `Input_GetInstance_48F270`, `Input_MouseGetCaps_48FD90`, `DirectInput_ErrorText_48FA80`, `Joypad_IsAcquired_509CC0`, `Joypad_InitActionFlags_509F00`, `Debug_Error`) except the two DirectInput import thunks (`0x4AF740`, `0x4AE730`), which the bodies declare as `__stdcall` DLL imports; if your batch landed them under their own thunk names, replace the two declarations with those.
Two bodies also need the image tables `0x00534900/0x00534910/0x005349B0/0x005349C0/0x005349D0` (16-byte GUID/IID images), `0x005348B0/0x005348C8/0x00534C38` (0x18-byte DIDATAFORMAT headers), `0x00534258` (double 1/255), `0x00534BB0` (double 1/128) and the writable `0x0054E8A8` axis table; express them as file-scope constants with the exact bytes above unless they were landed with the prior GUID batch.
