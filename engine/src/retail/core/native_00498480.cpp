// Native: Display_ProbeDirectXVersion (catalog name; retail symbol unknown); retail VA 0x00498480-0x004988C9.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1098 bytes, 38 blocks, 40 edges, 374 instructions, 428-byte frame (sub esp,0x11c + five pushes), two dword out-parameters, cdecl, no return value.
// Tools/evidence: radare2 6.2.0 af/pd over the exact image (per-instruction walk of all 374 instructions, no r2 extent merge); PE import directory parsed from the image for the eight IAT slots this routine calls; the six GUIDs and thirteen string literals were read as raw image bytes (.rdata 0x005348E0-0x00534A70 and 0x00543B10-0x00543C5C) and are reproduced byte-exactly below. Correction 2026-10-25, found by a paired runtime check rather than by reading: IID_IDirectDraw2 (0x00534A60) and IID_IDirectDrawSurface3 (0x00534A40) were first written with Data2 and Data3 byte-swapped (0x432B/0xCF11 and 0xB269), which made both QueryInterface calls fail. The image bytes are e0 f3 a6 b3 43 2b cf 11 .. and 00 4e 04 da b2 69 d0 11 .., i.e. Data2/Data3 are little-endian words 0x2B43/0x11CF and 0x69B2/0x11D0.
// Behavior/CFG: fills two out-parameters from a staged OS/DirectX probe. GetVersionExA(OSVERSIONINFOA, dwOSVersionInfoSize=0x94) first; failure leaves both outputs 0. dwPlatformId==VER_PLATFORM_WIN32_NT(2) writes 2 to platform and takes the version branch (major<4 -> tier 0 and return; major==4 -> the DINPUT-only NT4 ladder, tier 0x200 -> 0x300; major>4 falls through to the DDraw ladder); any other platform writes 1 and also falls through. The DDraw ladder loads DDRAW.DLL, resolves DirectDrawCreate, creates the object (tier 0x100), queries IID_IDirectDraw2 and releases it again (tier 0x200), loads DINPUT.DLL and resolves DirectInputCreateA (tier 0x300), then builds a DDSURFACEDESC (memset 0x6C bytes, dwSize 0x6C, dwFlags DDSD_CAPS, ddsCaps.dwCaps DDSCAPS_PRIMARYSURFACE), calls SetCooperativeLevel(NULL, DDSCL_NORMAL), CreateSurface, queries IID_IDirectDrawSurface3 (tier 0x500) and IID_IDirectDrawSurface4 (tier 0x600), releases the surface and the DDraw object, CoInitialize(NULL) + CoCreateInstance(CLSID_DirectMusic, CLSCTX_INPROC_SERVER) and releases it (tier 0x601), CoUninitialize, then resolves DirectDrawCreateEx and creates IID_IDirectDraw7 (tier 0x700), releasing it before freeing DDRAW.DLL. Every failure stage has its own OutputDebugStringA literal; the two surface-QI failures return silently (retail leaks those references and writes no diagnostic), and the DDraw-load failure path frees a null handle. No CALL to any game native: the closure is the eight imports plus COM vtable dispatch (QueryInterface +0x00, Release +0x08, CreateSurface +0x18, SetCooperativeLevel +0x50 on IDirectDraw).
// Status/limits: RUNTIME_TESTED + STRUCTURALLY_VERIFIED (all 38 blocks and every call site, GUID and literal matched to retail bytes); runtime UNOBSERVED. Compile evidence is the MSVC x86 build of the Windows target (this TU includes <windows.h>/<ddraw.h> like main.cpp and flow.cpp, so it is not part of the host clang translation unit set). Retail quirks reproduced, not cleaned up: FreeLibrary(NULL) at 0x004985A8, the "Couldn't LoadLibrary DDraw" text on the DirectDrawCreate GetProcAddress failure at 0x004985E7, the un-released IID_IDirectDrawSurface3/4 references at 0x004987BF/0x004987F4 and the two silent failure returns at 0x004987DC/0x0049880B.
#include <cstdint>
#include <cstring>

#include <windows.h>
#include <objbase.h>
#include <ddraw.h>

#include "igi/retail/core/native_00498480.hpp"

#include "igi/diagnostic_log.hpp"

extern "C" {

// GUIDs copied from retail .rdata, in the GUID field order (Data1/Data2/Data3 little-endian, Data4 as bytes).
static const GUID Iid_534A60 = { 0xB3A6F3E0U, 0x2B43U, 0x11CFU, { 0xA2U, 0xDEU, 0x00U, 0xAAU, 0x00U, 0xB9U, 0x33U, 0x56U } }; // 0x00534A60, IID_IDirectDraw2
static const GUID Iid_534A40 = { 0xDA044E00U, 0x69B2U, 0x11D0U, { 0xA1U, 0xD5U, 0x00U, 0xAAU, 0x00U, 0xB8U, 0xDFU, 0xBBU } }; // 0x00534A40, IID_IDirectDrawSurface3
static const GUID Iid_534A30 = { 0x0B2B8630U, 0xAD35U, 0x11D0U, { 0x8EU, 0xA6U, 0x00U, 0x60U, 0x97U, 0x97U, 0xEAU, 0x5BU } }; // 0x00534A30, IID_IDirectDrawSurface4
static const GUID Iid_534A50 = { 0x15E65EC0U, 0x3B9CU, 0x11D2U, { 0xB9U, 0x2FU, 0x00U, 0x60U, 0x97U, 0x97U, 0xEAU, 0x5BU } }; // 0x00534A50, IID_IDirectDraw7
static const GUID Clsid_5348F0 = { 0x636B9F10U, 0x0C7DU, 0x11D1U, { 0x95U, 0xB2U, 0x00U, 0x20U, 0xAFU, 0xDCU, 0x74U, 0x21U } }; // 0x005348F0, CLSID_DirectMusic
static const GUID Iid_5348E0 = { 0x6536115AU, 0x7B2DU, 0x11D2U, { 0xBAU, 0x18U, 0x00U, 0x00U, 0xF8U, 0x75U, 0xACU, 0x12U } }; // 0x005348E0

// Resolved entry points: retail calls them through GetProcAddress, so the call sites use retail's signatures.
using DirectDrawCreateFn = HRESULT(WINAPI*)(GUID*, LPDIRECTDRAW*, IUnknown*);
using DirectDrawCreateExFn = HRESULT(WINAPI*)(GUID*, LPVOID*, REFIID, IUnknown*);

void Display_ProbeDirectXVersion(std::uint32_t* tier, std::uint32_t* platform)
{
    OSVERSIONINFOA os_version;                 // 0x00498489: lea eax,[esp+0x94] (0x94-byte local, not pre-cleared)
    LPDIRECTDRAW direct_draw = nullptr;        // 0x00498494: [esp+0x14] = 0 (0x004985FD out-parameter)
    LPDIRECTDRAWSURFACE direct_draw_surface = nullptr; // 0x0049849C: [esp+0x18] = 0 (0x00498771 out-parameter)
    LPVOID direct_draw2 = nullptr;             // 0x00498498: [esp+0x1C] = 0 (0x0049863D out-parameter)
    LPVOID direct_music = nullptr;             // 0x0049882E: [esp+0x20] = 0 (0x0049883C out-parameter)
    LPVOID surface3 = nullptr;                 // 0x004984A0: [esp+0x24] = 0 (0x004987B2 out-parameter)
    LPVOID direct_draw7 = nullptr;             // 0x0049889A: [esp+0x28] out-parameter
    LPVOID surface4 = nullptr;                 // 0x004984A4: [esp+0x2C] = 0 (0x004987E1 out-parameter)
    DDSURFACEDESC surface_description;         // 0x00498713: lea edi,[esp+0x2C], rep stos 0x1B dwords

    os_version.dwOSVersionInfoSize = 0x94U;    // 0x004984A8: movl $0x94,[esp+0x9C]
    if (!GetVersionExA(&os_version))           // 0x004984B3: call [0x533108]
    {
        *platform = 0U;                        // 0x004984CD: mov [ecx],ebx (ecx = arg2)
        *tier = 0U;                            // 0x004984CF: mov [edx],ebx (edx = arg1)
        return;                                // 0x004984D9
    }

    if (os_version.dwPlatformId == 2U)         // 0x004984DA/0x004984E8: dwPlatformId == VER_PLATFORM_WIN32_NT
    {
        *platform = 2U;                        // 0x004984F8: movl [edi],0x2
        if (os_version.dwMajorVersion < 4U)    // 0x004984F1/0x004984FE/0x00498501: jae
        {
            *tier = 0U;                        // 0x0049850D
            return;                            // 0x00498516
        }
        if (os_version.dwMajorVersion == 4U)   // 0x00498517: jne -> DDraw ladder
        {
            *tier = 0x200U;                    // 0x00498525
            HMODULE const dinput_library = LoadLibraryA("DINPUT.DLL"); // 0x00498520/0x0049852C
            if (dinput_library == nullptr)     // 0x00498534
            {
                OutputDebugStringA("Couldn't LoadLibrary DInput\r\n"); // 0x00498538
                return;                        // 0x0049854D
            }
            FARPROC const direct_input_create = GetProcAddress(dinput_library, "DirectInputCreateA"); // 0x0049854E
            FreeLibrary(dinput_library);       // 0x0049855A/0x0049855D
            if (direct_input_create == nullptr) // 0x00498563
            {
                OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n"); // 0x00498567
                return;                        // 0x0049857C
            }
            *tier = 0x300U;                    // 0x0049857E
            return;                            // 0x0049858E
        }
    }
    else
    {
        *platform = 1U;                        // 0x0049858F: movl [edi],0x1
    }

    // 0x00498595: DDraw ladder, shared by the non-NT platform and by NT 5+.
    HMODULE const ddraw_library = LoadLibraryA("DDRAW.DLL"); // 0x0049859B/0x004985A0
    if (ddraw_library == nullptr)              // 0x004985A4
    {
        *tier = 0U;                            // 0x004985B0
        *platform = 0U;                        // 0x004985B2
        FreeLibrary(nullptr);                  // 0x004985AF/0x004985B4: retail frees the null handle
        return;                                // 0x004985C4
    }
    FARPROC const direct_draw_create_entry = GetProcAddress(ddraw_library, "DirectDrawCreate"); // 0x004985C5/0x004985CB
    if (direct_draw_create_entry == nullptr)   // 0x004985D1
    {
        *tier = 0U;                            // 0x004985DD
        *platform = 0U;                        // 0x004985DF
        FreeLibrary(ddraw_library);            // 0x004985E1
        OutputDebugStringA("Couldn't LoadLibrary DDraw\r\n"); // 0x004985E7: retail's literal on this path
        return;                                // 0x004985FC
    }
    DirectDrawCreateFn const direct_draw_create =
        reinterpret_cast<DirectDrawCreateFn>(direct_draw_create_entry);
    if (direct_draw_create(nullptr, &direct_draw, nullptr) < 0) // 0x004985FD/0x00498604
    {
        *tier = 0U;                            // 0x00498612
        *platform = 0U;                        // 0x00498614
        FreeLibrary(ddraw_library);            // 0x00498616
        OutputDebugStringA("Couldn't create DDraw\r\n"); // 0x0049861C
        return;                                // 0x00498631
    }
    *tier = 0x100U;                            // 0x00498641
    if (direct_draw->QueryInterface(Iid_534A60, &direct_draw2) < 0) // 0x00498650: vtable +0x00
    {
        direct_draw->Release();                // 0x0049865D: vtable +0x08
        FreeLibrary(ddraw_library);            // 0x00498661
        OutputDebugStringA("Couldn't QI DDraw2\r\n"); // 0x00498667
        return;                                // 0x0049867C
    }
    reinterpret_cast<IUnknown*>(direct_draw2)->Release(); // 0x00498684
    *tier = 0x200U;                            // 0x0049868C
    HMODULE const dinput_library = LoadLibraryA("DINPUT.DLL"); // 0x00498687/0x00498692
    if (dinput_library == nullptr)             // 0x00498696
    {
        OutputDebugStringA("Couldn't LoadLibrary DInput\r\n"); // 0x0049869A
        direct_draw->Release();                // 0x004986AC
        FreeLibrary(ddraw_library);            // 0x004986B0
        return;                                // 0x004986C0
    }
    FARPROC const direct_input_create = GetProcAddress(dinput_library, "DirectInputCreateA"); // 0x004986C1/0x004986C7
    if (direct_input_create == nullptr)        // 0x004986CD
    {
        FreeLibrary(dinput_library);           // 0x004986D8
        FreeLibrary(ddraw_library);            // 0x004986DB
        direct_draw->Release();                // 0x004986E4
        OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n"); // 0x004986E7
        return;                                // 0x004986FC
    }
    *tier = 0x300U;                            // 0x00498704
    FreeLibrary(dinput_library);               // 0x00498703/0x0049870A
    std::memset(&surface_description, 0, 0x6CU); // 0x00498713/0x00498719: rep stos 0x1B dwords
    surface_description.dwSize = 0x6CU;        // 0x0049871F
    surface_description.dwFlags = DDSD_CAPS;   // 0x00498727: movl $0x1
    surface_description.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE; // 0x0049872F: movl $0x200
    if (direct_draw->SetCooperativeLevel(nullptr, DDSCL_NORMAL) < 0) // 0x0049873F: vtable +0x50
    {
        direct_draw->Release();                // 0x0049874D
        FreeLibrary(ddraw_library);            // 0x00498751
        *tier = 0U;                            // 0x00498758
        OutputDebugStringA("Couldn't Set coop level\r\n"); // 0x00498753/0x0049875E
        return;                                // 0x0049876E
    }
    if (direct_draw->CreateSurface(&surface_description, &direct_draw_surface, nullptr) < 0) // 0x0049877E: vtable +0x18
    {
        direct_draw->Release();                // 0x0049878C
        FreeLibrary(ddraw_library);            // 0x00498790
        *tier = 0U;                            // 0x00498797
        OutputDebugStringA("Couldn't CreateSurface\r\n"); // 0x00498792/0x0049879D
        return;                                // 0x004987AD
    }
    if (direct_draw_surface->QueryInterface(Iid_534A40, &surface3) < 0) // 0x004987BF: IID_IDirectDrawSurface3
    {
        direct_draw->Release();                // 0x004987CC
        FreeLibrary(ddraw_library);            // 0x004987D0
        return;                                // 0x004987DC: silent, tier stays 0x300
    }
    *tier = 0x500U;                            // 0x004987E5
    if (direct_draw_surface->QueryInterface(Iid_534A30, &surface4) < 0) // 0x004987F4: IID_IDirectDrawSurface4
    {
        direct_draw->Release();                // 0x00498801
        FreeLibrary(ddraw_library);            // 0x00498805
        return;                                // 0x0049880B: silent, tier stays 0x500
    }
    *tier = 0x600U;                            // 0x00498816
    direct_draw_surface->Release();             // 0x0049881F
    direct_draw->Release();                     // 0x00498829
    CoInitialize(nullptr);                      // 0x0049882C/0x00498836 (result unused by retail)
    if (CoCreateInstance(Clsid_5348F0, nullptr, CLSCTX_INPROC_SERVER, Iid_5348E0, &direct_music) < 0) // 0x0049884F
    {
        OutputDebugStringA("Couldn't create CLSID_DirectMusic\r\n"); // 0x00498859
        FreeLibrary(ddraw_library);             // 0x00498865
        return;                                 // 0x00498871: tier stays 0x600
    }
    *tier = 0x601U;                             // 0x00498876
    reinterpret_cast<IUnknown*>(direct_music)->Release(); // 0x0049887F
    CoUninitialize();                           // 0x00498882
    FARPROC const direct_draw_create_ex_entry = GetProcAddress(ddraw_library, "DirectDrawCreateEx"); // 0x00498888/0x0049888E
    if (direct_draw_create_ex_entry != nullptr) // 0x00498896
    {
        DirectDrawCreateExFn const direct_draw_create_ex =
            reinterpret_cast<DirectDrawCreateExFn>(direct_draw_create_ex_entry);
        if (direct_draw_create_ex(nullptr, &direct_draw7, Iid_534A50, nullptr) >= 0) // 0x004988A6
        {
            *tier = 0x700U;                     // 0x004988B0
            reinterpret_cast<IUnknown*>(direct_draw7)->Release(); // 0x004988B9
        }
    }
    FreeLibrary(ddraw_library);                 // 0x004988BD
    // Infrastructure diagnostic only (rule 3a): reports the two values this routine
    // produced so a paired runtime check can compare them. It reads the out-params
    // after all writes and cannot change control flow, ordering or arguments.
    IGI_DIAG("0x00498480", "Display_ProbeDirectXVersion -> tier=0x%X platform=%u", *tier, *platform);
}

} // extern "C"
