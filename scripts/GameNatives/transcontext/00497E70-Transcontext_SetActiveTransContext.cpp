// IGI native TransContext_SetActiveTransContext - 0x00497E70 - transcribed from retail igi.exe.
// Retail: cdecl (source). Copies 0x2a dwords from source into the active block at 0xbcaae0, calls the handler at g_a94e84[g_6e5ba4] (when set) with a pointer to a stack slot holding the block address, sets g_6e5bb0 to 1 and increments g_6e5bb4, returning the new count.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

extern std::int32_t g_6e5ba4;
extern std::uint32_t g_a94e84[];
extern std::uint8_t g_bcaae0[];
extern std::int32_t g_6e5bb0;
extern std::int32_t g_6e5bb4;
int __cdecl TransContext_SetActiveTransContext(const void* source)
{
    typedef void (__cdecl *Handler)(void**);
    Handler handler = reinterpret_cast<Handler>(static_cast<std::uintptr_t>(g_a94e84[g_6e5ba4]));
    void* arg = g_bcaae0;
    std::memcpy(g_bcaae0, source, 0x2a * 4);
    if (handler != nullptr) {
        handler(&arg);
    }
    g_6e5bb0 = 1;
    g_6e5bb4 = g_6e5bb4 + 1;
    return g_6e5bb4;
}
