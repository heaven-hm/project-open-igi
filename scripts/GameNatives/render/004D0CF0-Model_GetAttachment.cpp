// IGI native Model_GetAttachment - 0x004D0CF0 - transcribed from retail igi.exe.
// Retail: cdecl (model, index). ecx=[[esp+4]+0xc]; edx=[[ecx+0xc]+0x8c]; return edx + index*0x44.
#include <cstdint>

void* __cdecl Model_GetAttachment(void* model, std::int32_t index)
{
    std::uint32_t a = *reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(model) + 0xc);
    std::uint32_t b = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(a) + 0xc);
    std::uint32_t t = *reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(b) + 0x8c);
    return reinterpret_cast<void*>(static_cast<std::uintptr_t>(t + static_cast<std::uint32_t>(index) * 0x44u));
}
