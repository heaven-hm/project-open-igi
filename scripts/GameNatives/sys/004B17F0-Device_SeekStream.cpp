// IGI native Device_SeekStream - 0x004B17F0 - transcribed from retail igi.exe.
// Retail: cdecl (stream, offset). Dispatches through the device table entry at offset 0x28 (0x9436e4, 140 bytes per entry) with (index, stream, offset) and returns its result.
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

extern std::uint8_t g_9436bc[];
typedef int (__cdecl *DeviceOp1)(std::int32_t, void*);
typedef int (__cdecl *DeviceOp2)(std::int32_t, void*, std::int32_t);
int __cdecl Device_SeekStream(void* stream, std::int32_t offset)
{
    std::int32_t index = static_cast<std::int32_t>(rd32(stream, 0));
    DeviceOp2 op = reinterpret_cast<DeviceOp2>(static_cast<std::uintptr_t>(rd32(g_9436bc, index * 140 + 0x28)));
    return op(index, stream, offset);
}
