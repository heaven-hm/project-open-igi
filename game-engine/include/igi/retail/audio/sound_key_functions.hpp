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

// Retail IGI.EXE routine at VA 0x00497390. Ghidra's symbol remains unresolved.
void IGI_RETAIL_CDECL FUN_00497390(char* output, char* input_path);

// Retail IGI.EXE routine at VA 0x00497450. The decompiler/listing return-value
// discrepancy is recorded in game-engine/REVERSE_ENGINEERING.md.
std::int32_t IGI_RETAIL_CDECL FUN_00497450(char* name);

// Retail IGI.EXE routine at VA 0x00497410. Ghidra's symbol remains unresolved.
std::int32_t IGI_RETAIL_CDECL FUN_00497410(char* name);

// Retail logger called by FUN_00497390 for empty or dotless paths.
void IGI_RETAIL_CDECL FUN_004af810(const char* format, ...);

}
