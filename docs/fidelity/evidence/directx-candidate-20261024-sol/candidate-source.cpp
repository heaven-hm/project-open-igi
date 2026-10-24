#include "00498480-Native_00498480.hpp"
#include <ddraw.h>
#include <dmusicc.h>

#define NATIVE_DECL __cdecl

// Native: Native_00498480; retail VA 0x00498480; x86 cdecl, two DWORD output pointers; raw EAX result.
// Artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 6.0.10601, Radare2 6.2.0, angr CFGFast; docs/fidelity/evidence/directx-probe-20261024-sol/.
// CFG/behavior: 1098 bytes, 38 retail blocks; BN/r2 CFG similarity 1.0. Preserves the DLL/COM sequence, output stores and error cleanup.
// Status/limits: TRANSCRIBED candidate; original source names/type spelling unknown; production intake and independent scoped runtime review required.
extern "C" std::uint32_t NATIVE_DECL Native_00498480(std::uint32_t* capability,
                                                  std::uint32_t* platform)
{
    IDirectDraw* draw = nullptr;                 // retail stack +0x10
    IDirectDraw2* draw2 = nullptr;               // +0x18
    IDirectDrawSurface* surface = nullptr;       // +0x14
    IDirectDrawSurface3* surface3 = nullptr;     // +0x20
    IDirectDrawSurface4* surface4 = nullptr;     // +0x28
    OSVERSIONINFOA version;
    version.dwOSVersionInfoSize = 0x94;
    const BOOL version_result = ::GetVersionExA(&version);
    if (version_result == 0)
    {
        *platform = 0;
        *capability = 0;
        return static_cast<std::uint32_t>(version_result);
    }

    if (version.dwPlatformId != 2)
    {
        *platform = 1;
    }
    else
    {
        *platform = 2;
        if (version.dwMajorVersion < 4)
        {
            *capability = 0;
            // 0x498503..516: EAX is the first output address on this return.
            return reinterpret_cast<std::uintptr_t>(capability);
        }
        if (version.dwMajorVersion == 4)
        {
            *capability = 0x200;
            HMODULE input_module = ::LoadLibraryA("DINPUT.DLL");
            if (input_module == nullptr)
            {
                ::OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");
                // The retail returns the raw register after this void API; no BOOL meaning is asserted.
                std::uint32_t eax;
                __asm__ volatile("" : "=a"(eax) : : "memory");
                return eax;
            }
            FARPROC input_create = ::GetProcAddress(input_module, "DirectInputCreateA");
            const BOOL free_result = ::FreeLibrary(input_module);
            if (input_create == nullptr)
            {
                ::OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n");
                std::uint32_t eax;
                __asm__ volatile("" : "=a"(eax) : : "memory");
                return eax;
            }
            *capability = 0x300;
            return static_cast<std::uint32_t>(free_result);
        }
    }

    HMODULE draw_module = ::LoadLibraryA("DDRAW.DLL");
    if (draw_module == nullptr)
    {
        *capability = 0;
        *platform = 0;
        // Retail 0x4985AF/5B4 calls FreeLibrary even for the NULL handle.
        return static_cast<std::uint32_t>(::FreeLibrary(nullptr));
    }
    auto draw_create = reinterpret_cast<HRESULT (WINAPI *)(GUID*, IDirectDraw**, IUnknown*)>(
        ::GetProcAddress(draw_module, "DirectDrawCreate"));
    if (draw_create == nullptr)
    {
        *capability = 0;
        *platform = 0;
        ::FreeLibrary(draw_module);
        ::OutputDebugStringA("Couldn't LoadLibrary DDraw\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }
    if (draw_create(nullptr, &draw, nullptr) < 0)
    {
        *capability = 0;
        *platform = 0;
        ::FreeLibrary(draw_module);
        ::OutputDebugStringA("Couldn't create DDraw\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }

    *capability = 0x100;
    if (draw->QueryInterface(IID_IDirectDraw2, reinterpret_cast<void**>(&draw2)) < 0)
    {
        draw->Release();
        ::FreeLibrary(draw_module);
        ::OutputDebugStringA("Couldn't QI DDraw2\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }
    draw2->Release();
    *capability = 0x200;
    HMODULE input_module = ::LoadLibraryA("DINPUT.DLL");
    if (input_module == nullptr)
    {
        ::OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");
        draw->Release();
        return static_cast<std::uint32_t>(::FreeLibrary(draw_module));
    }
    if (::GetProcAddress(input_module, "DirectInputCreateA") == nullptr)
    {
        // 0x4986D2..6E4 unloads both DLL handles before this Release; retain that order.
        ::FreeLibrary(input_module);
        ::FreeLibrary(draw_module);
        draw->Release();
        ::OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }
    *capability = 0x300;
    ::FreeLibrary(input_module);

    DDSURFACEDESC description;
    // 0x49870C..719: exactly 27 DWORD stores, no extra memset helper call.
    std::uint32_t* destination = reinterpret_cast<std::uint32_t*>(&description);
    std::uint32_t count = 0x1b;
    __asm__ volatile("rep stosl" : "+D"(destination), "+c"(count) : "a"(0U) : "memory");
    description.dwSize = 0x6c;
    description.dwFlags = 1;
    description.ddsCaps.dwCaps = 0x200;
    if (draw->SetCooperativeLevel(nullptr, 8) < 0)
    {
        draw->Release();
        ::FreeLibrary(draw_module);
        *capability = 0;
        ::OutputDebugStringA("Couldn't Set coop level\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }
    if (draw->CreateSurface(&description, &surface, nullptr) < 0)
    {
        draw->Release();
        ::FreeLibrary(draw_module);
        *capability = 0;
        ::OutputDebugStringA("Couldn't CreateSurface\r\n");
        std::uint32_t eax;
        __asm__ volatile("" : "=a"(eax) : : "memory");
        return eax;
    }
    if (surface->QueryInterface(IID_IDirectDrawSurface3, reinterpret_cast<void**>(&surface3)) < 0)
    {
        draw->Release();
        return static_cast<std::uint32_t>(::FreeLibrary(draw_module));
    }
    *capability = 0x500;
    if (surface->QueryInterface(IID_IDirectDrawSurface4, reinterpret_cast<void**>(&surface4)) < 0)
    {
        draw->Release();
        return static_cast<std::uint32_t>(::FreeLibrary(draw_module));
    }
    *capability = 0x600;
    // The retail does not release surface3/surface4 here; do not add cleanup.
    surface->Release();
    draw->Release();
    IUnknown* music = nullptr;                   // retail +0x1C
    ::CoInitialize(nullptr);
    if (::CoCreateInstance(CLSID_DirectMusic, nullptr, 1, IID_IDirectMusic,
                           reinterpret_cast<void**>(&music)) < 0)
    {
        ::OutputDebugStringA("Couldn't create CLSID_DirectMusic\r\n");
        // CoUninitialize is absent on this retail failure branch.
        return static_cast<std::uint32_t>(::FreeLibrary(draw_module));
    }
    *capability = 0x601;
    music->Release();
    ::CoUninitialize();
    auto draw_create_ex = reinterpret_cast<HRESULT (WINAPI *)(GUID*, void**, REFIID, IUnknown*)>(
        ::GetProcAddress(draw_module, "DirectDrawCreateEx"));
    IDirectDraw7* draw7;                         // retail +0x24, not zero-initialized
    if (draw_create_ex != nullptr &&
        draw_create_ex(nullptr, reinterpret_cast<void**>(&draw7), IID_IDirectDraw7, nullptr) >= 0)
    {
        *capability = 0x700;
        draw7->Release();
    }
    return static_cast<std::uint32_t>(::FreeLibrary(draw_module));
}

#undef NATIVE_DECL
