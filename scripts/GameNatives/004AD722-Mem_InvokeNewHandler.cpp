// IGI native Mem_InvokeNewHandler - 0x004AD722 - transcribed from retail igi.exe.
// Retail: cdecl (size). Calls the new handler stored at g_936254 with the size when it is set; returns 1 when the handler returns non-zero, otherwise 0.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

extern int (__cdecl *g_936254)(std::uint32_t);
int __cdecl Mem_InvokeNewHandler(std::uint32_t size)
{
    if (g_936254 != nullptr) {
        if (g_936254(size) != 0) {
            return 1;
        }
    }
    return 0;
}
