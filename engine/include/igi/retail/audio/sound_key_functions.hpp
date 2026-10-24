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

// Retail IGI.EXE routine at VA 0x00497390. Ghidra's symbol remains unresolved.
void NATIVE_DECL Sound_ExtractNameFromPath(char* output, char* input_path);

// Retail IGI.EXE routine at VA 0x00497450. The decompiler/listing return-value
// discrepancy is recorded in game-engine/REVERSE_ENGINEERING.md.
std::int32_t NATIVE_DECL Sound_ComputeRollingNameHash(char* name);

// Retail IGI.EXE routine at VA 0x00497410. Ghidra's symbol remains unresolved.
std::int32_t NATIVE_DECL Sound_ComputeWeightedNameHash(char* name);

// Retail logger called by Sound_ExtractNameFromPath for empty or dotless paths.
void NATIVE_DECL Engine_LogFormattedMessage(const char* format, ...);

}
