#pragma once

#include <cstdint>

namespace igi::retail::sys {

// Native: String_ToUpperChar() - VA 0x004AD73D - partial port (C-locale branch only).
// Previous catalog alias: Camera_ApplyWideScreen_28.
// ASCII upper-case conversion taken when global 0x009361E4 is zero (its initial value).
// The locale-table branch used when that global is set is not ported.
std::uint32_t String_ToUpperChar(std::uint32_t character);
inline std::uint32_t Camera_ApplyWideScreen_28(std::uint32_t character) { return String_ToUpperChar(character); }

// Native: Resource_CanonicalizePath() - VA 0x004B5AF0 - complete port.
// Previous catalog alias: Sys_ResolveSymbol_15.
// Copies `source` into `destination` applying the upper-case conversion, then writes the
// terminating NUL. AL is zero on return; the upper EAX bytes retain the final
// uppercase-helper result (nonzero for a final high-bit byte in the C locale).
std::int32_t Resource_CanonicalizePath(char* destination, char* source);
inline std::int32_t Sys_ResolveSymbol_15(char* destination, char* source)
{
    return Resource_CanonicalizePath(destination, source);
}

// Native: Resource_CalculatePathHash() - VA 0x004B5B30 - complete port.
// Previous catalog alias: Sys_CheckHeap_15.
// Case-sensitive string hash: starts at 1, folds `data = (data + 1) * character` per byte,
// and returns the result modulo 0x3037.
std::uint32_t Resource_CalculatePathHash(const char* text);
inline std::uint32_t Sys_CheckHeap_15(const char* text)
{
    return Resource_CalculatePathHash(text);
}

} // namespace igi::retail::sys
