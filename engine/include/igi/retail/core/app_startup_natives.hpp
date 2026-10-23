#pragma once

// The retail application start-up natives.
//
// igi.exe's main passes the command line to the native at 0x0048F0C0, which
// stores it at 0x005C8BC8, and the instance handle to the native at
// 0x0048F0D0, which stores it at 0x005C8BC0. The getter at 0x0048F0B0 does
// not read that instance-handle global: it reads 0x005C8BD4, the window-text
// pointer written by the native at 0x0048F100, whose transcriptions are the
// only other users of that address. The instance handle is read by the
// separate getter at 0x0048F090.

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

// Retail global at VA 0x005C8BD4: the last window-text pointer, written by the
// native at 0x0048F100 (SetAppTitle) and returned by the getter at 0x0048F0B0.
// Nothing in the current engine writes it because 0x0048F100 is not built yet;
// it is a separate 4-byte global from InstanceHandle.
extern char const* AppTitle;

// Native at VA 0x0048F0C0.
// mov eax,[esp+4]; mov [0x005C8BC8],eax; ret -- the argument bits are the return value.
char const* IGI_RETAIL_CDECL StoreCommandLine(char const* command_line);

// Native at VA 0x0048F080 (record name GetCommandLine; the C identifier must
// not collide with the Win32 GetCommandLine macro).
// mov eax,[0x005C8BC8]; ret
char const* IGI_RETAIL_CDECL GameCommandLine();

// Native at VA 0x0048F0D0.
// mov eax,[esp+4]; mov [0x005C8BC0],eax; ret -- the argument bits are the return value.
void* IGI_RETAIL_CDECL StoreInstanceHandle(void* instance);

// Native at VA 0x0048F0B0.
// mov eax,[0x005C8BD4]; ret -- returns the window-text pointer, not the
// instance handle (which the separate 0x0048F090 getter reads from 0x005C8BC0).
char const* IGI_RETAIL_CDECL GetAppTitle();

} // extern "C"

#undef IGI_RETAIL_CDECL
