#pragma once

// Retail command-line parser (0x0048F360), its tokenizer (0x004A5A0C) and the
// CLI handler bodies main (0x0048F530) registers, reconstructed from igi.exe
// (SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01).
//
// Static evidence: Binary Ninja (selected decompiler) + Radare2 CFG + angr
// CFGFast per address (build/native-analysis/0x0048F360/capture.json,
// build/audit/p05b-20261023T222309Z/, build/audit/p05c-20261023T223344Z/,
// build/audit/p05d-20261023T225957Z/). Retail runtime evidence: Tester
// individual full-game Frida records under
// build/tester/20261023T220230Z/p05c/ and runtime/.
//
// Handler dispatch is token-gated: registering a handler does not run it.
// In `window level1` only the Window handler executes; the others need their
// own CLI token and are not dispatched by registration alone.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004A5A0C: strtok-like tokenizer over a 32-byte delimiter bitmap; mutates the
// input string, keeps its continuation pointer in a file-scope state (retail
// image global 0x00936060), returns the token start or null.
char* NATIVE_DECL AppMain_TokenizeDelimited(char* text, char const* delimiters);

// 0x0048F360: copies the stored command line (0x0048F080) into a 0x200-byte
// stack buffer, tokenizes it, case-insensitively prefix-matches each token
// against option_name and calls handler(token + strlen(option_name)) on a match.
// Reaches handler only for token-matched options; returns the final tokenizer
// result (null at end of line).
void* NATIVE_DECL AppMain_ParseCmdLineArgs(
    char* option_name, std::int32_t(NATIVE_DECL* handler)(char* suffix));

// CLI handlers registered by main, all cdecl int(char* suffix) with the suffix
// pointing inside the parser's stack copy after the matched option name.
std::int32_t NATIVE_DECL AppMain_WindowCommand(char* suffix);          // 0x0048F350
std::int32_t NATIVE_DECL AppMain_WinInputCommand(char* suffix);        // 0x0048F920
std::int32_t NATIVE_DECL AppMain_GermanyCommand(char* suffix);         // 0x0048F930
std::int32_t NATIVE_DECL AppMain_USACommand(char* suffix);             // 0x0048F950
std::int32_t NATIVE_DECL AppMain_HardcoreGermanCommand(char* suffix);  // 0x0048F960

} // extern "C"
