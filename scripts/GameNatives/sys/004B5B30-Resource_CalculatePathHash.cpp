// IGI native Resource_CalculatePathHash - 0x004B5B30 - transcribed from retail igi.exe.
// Retail: cdecl (path). eax = 1; for each non-zero byte c: eax = (eax + 1) * (signed char)c. Returns eax % 0x3037 (unsigned div).
#include <cstdint>

std::uint32_t __cdecl Resource_CalculatePathHash(const char* path)
{
    std::uint32_t eax = 1;
    const char* edx = path;
    char cl = *edx;
    while (cl != 0) {
        std::int32_t ecx = static_cast<std::int32_t>(static_cast<signed char>(cl));
        eax = (eax + 1u) * static_cast<std::uint32_t>(ecx);
        cl = edx[1];
        edx++;
    }
    return eax % 0x3037u;
}
