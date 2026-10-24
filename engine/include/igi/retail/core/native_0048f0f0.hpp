#pragma once

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern std::int32_t Native_005C8BCC;
std::int32_t NATIVE_DECL SetConfigFileName(std::int32_t value);

} // extern "C"

#undef NATIVE_DECL
