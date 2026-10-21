// IGI native Resource_MatchPrefix - 0x004AAD10 - transcribed from retail igi.exe.
// Retail: cdecl (a, b, n). Counts the bytes of a scanned up to its terminator (at most n), compares b against a for that many bytes, then compares the last compared bytes unsigned: 0 when equal, -1 when b is greater, 1 when b is smaller. n of 0 returns 0.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }


int __cdecl Resource_MatchPrefix(const char* a, const char* b, std::uint32_t n)
{
    if (n == 0) {
        return 0;
    }
    const unsigned char* pa = reinterpret_cast<const unsigned char*>(a);
    const unsigned char* pb = reinterpret_cast<const unsigned char*>(b);
    std::uint32_t scanned = 0;
    while (scanned < n) {
        ++scanned;
        if (pa[scanned - 1] == 0) {
            break;
        }
    }
    std::uint32_t i = 0;
    std::uint32_t remaining = scanned;
    while (remaining != 0) {
        ++i;
        --remaining;
        if (pb[i - 1] != pa[i - 1]) {
            break;
        }
    }
    unsigned char last = pb[i - 1];
    unsigned char other = pa[i - 1];
    if (last > other) {
        return -1;
    }
    if (last == other) {
        return 0;
    }
    return 1;
}
