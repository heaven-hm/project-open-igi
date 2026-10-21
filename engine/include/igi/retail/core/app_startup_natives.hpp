#pragma once

// The retail application start-up natives.
//
// igi.exe's main passes the command line to the native at 0x0048F0C0, which
// stores it at 0x005C8BC8, and the instance handle to the native at
// 0x0048F0D0, which stores it at 0x005C8BC0.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

// Retail global at VA 0x005C8BC8: the command line pointer.
extern char const* CommandLine;

// Retail global at VA 0x005C8BC0: the instance handle.
extern void* InstanceHandle;

// Native at VA 0x0048F0C0.
// mov eax,[esp+4]; mov [0x005C8BC8],eax; ret
std::int32_t IGI_RETAIL_CDECL StoreCommandLine(char const* command_line);

// Native at VA 0x0048F080.
// mov eax,[0x005C8BC8]; ret
char const* IGI_RETAIL_CDECL GetRetailCommandLine();

// Native at VA 0x0048F0D0.
// mov eax,[esp+4]; mov [0x005C8BC0],eax; ret
std::int32_t IGI_RETAIL_CDECL StoreInstanceHandle(void* instance);

// Native at VA 0x0048F0B0.
// mov eax,[0x005C8BC0]; ret
void* IGI_RETAIL_CDECL GetInstanceHandle();

} // extern "C"

#undef IGI_RETAIL_CDECL
