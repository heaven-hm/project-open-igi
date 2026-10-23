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

extern std::int32_t Native_005C8BCC;
std::int32_t IGI_RETAIL_CDECL SetConfigFileName(std::int32_t value);

} // extern "C"

#undef IGI_RETAIL_CDECL
