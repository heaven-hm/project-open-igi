// IGI native Mem_AllocChunk - 0x004AA670 - transcribed from retail igi.exe.
// Retail: cdecl (size). Forwards to Mem_AllocWithRetry (0x004AA682) with size and the flag in g_936250.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

extern std::int32_t g_936250;
void* __cdecl Mem_AllocWithRetry(std::uint32_t size, std::int32_t retry); // 0x004AA682
void* __cdecl Mem_AllocChunk(std::uint32_t size)
{
    return Mem_AllocWithRetry(size, g_936250);
}
