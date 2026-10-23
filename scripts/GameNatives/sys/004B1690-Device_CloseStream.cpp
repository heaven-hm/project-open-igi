// IGI native Device_CloseStream - 0x004B1690 - transcribed from retail igi.exe.
// Retail: cdecl (stream). The default stream at g_943980 only clears g_943988. Otherwise dispatches through the device table at 0x9436bc (140 bytes per entry, indexed by the first dword of the stream) with (index, stream).
#include <cstdint>
#include <cstring>
static inline std::uint32_t rd32(const void* p, std::intptr_t off) { std::uint32_t v; std::memcpy(&v, reinterpret_cast<const std::uint8_t*>(p) + off, 4); return v; }
static inline void wr32(void* p, std::intptr_t off, std::uint32_t v) { std::memcpy(reinterpret_cast<std::uint8_t*>(p) + off, &v, 4); }
static inline void* ptr32(std::uint32_t v) { return reinterpret_cast<void*>(static_cast<std::uintptr_t>(v)); }
static inline std::uint32_t u32(const void* p) { return static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(p)); }

extern std::uint8_t g_9436bc[];
typedef int (__cdecl *DeviceOp1)(std::int32_t, void*);
typedef int (__cdecl *DeviceOp2)(std::int32_t, void*, std::int32_t);
extern std::uint8_t g_943980[];
extern std::int32_t g_943988;
int __cdecl Device_CloseStream(void* stream)
{
    if (u32(stream) == u32(g_943980)) {
        g_943988 = 0;
        return 0;
    }
    std::int32_t index = static_cast<std::int32_t>(rd32(stream, 0));
    DeviceOp1 op = reinterpret_cast<DeviceOp1>(static_cast<std::uintptr_t>(rd32(g_9436bc, index * 140)));
    return op(index, stream);
}
