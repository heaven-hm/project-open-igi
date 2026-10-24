// IGI native Mem_AllocWithRetry - 0x004AA682 - transcribed from retail igi.exe.
// Retail: cdecl (size, retry). Sizes above 0xffffffe0 return null. Otherwise allocates with Mem_AllocateRawBlock (0x004AA6AE); on failure, when retry is set, calls Mem_InvokeNewHandler (0x004AD722) and tries again while it returns non-zero.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

void* __cdecl Mem_AllocateRawBlock(std::uint32_t size); // 0x004AA6AE
int __cdecl Mem_InvokeNewHandler(std::uint32_t size); // 0x004AD722
void* __cdecl Mem_AllocWithRetry(std::uint32_t size, std::int32_t retry)
{
    if (size > 0xffffffe0u) {
        return nullptr;
    }
    for (;;) {
        void* block = Mem_AllocateRawBlock(size);
        if (block != nullptr) {
            return block;
        }
        if (retry == 0) {
            return block;
        }
        if (Mem_InvokeNewHandler(size) == 0) {
            return nullptr;
        }
    }
}
