// IGI native Model_GetAttachmentCount - 0x004D0D10 - transcribed from retail igi.exe.
// Retail: cdecl (model). Walks model+0xc then +0xc and returns the sign-extended 16-bit value at +0x7e.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }

std::int32_t __cdecl Model_GetAttachmentCount(void* model)
{
    void* a = ptr32(rd32(model, 0xc));
    void* b = ptr32(rd32(a, 0xc));
    std::int16_t count;
    std::memcpy(&count, reinterpret_cast<const std::uint8_t*>(b) + 0x7e, 2);
    return static_cast<std::int32_t>(count);
}
