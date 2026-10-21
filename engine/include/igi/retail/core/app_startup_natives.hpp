#pragma once

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

extern "C" {

// Retail global at VA 0x005C8BC8. Its semantic name is not yet established.
extern std::int32_t Native_005C8BC8;

// Native at VA 0x0048F0C0.
// mov eax,[esp+4]; mov [0x005C8BC8],eax; ret
std::int32_t IGI_RETAIL_CDECL Native_0048F0C0(std::int32_t value);

} // extern "C"

#undef IGI_RETAIL_CDECL
