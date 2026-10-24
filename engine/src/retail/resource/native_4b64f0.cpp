// Native: Resource_UploadMipmapPacket_4B64F0; retail VA 0x004B64F0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B64F0-0x004B6530 (65 bytes, 1 block, 0 edges, 26 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and the raw bytes (8b44240c 8b4c2424 8b542420 50 8b442420 51 8b4c2420 52 8b542420 50 8b442420 51 8b4c2418 52 50 8b442424 8b11 8b4904 8d04c0 52 8d1481 52 e883beffff 83c424 c3); section 22.3 of the evidence sheet.
// Behavior/CFG: forwards nine values to Texture_UploadMipmapPacket (0x004B23B0). Argument1 is a pair of dwords: its first dword is passed as the callee's second argument and its second dword is the base of a 0x24-stride record selected by argument2, passed as the callee's first argument; the rest are forwarded with argument3 moved to the ninth slot, because retail pushes argument3 first. No branch, no store, no data reference of its own.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The volatile sink is the only non-retail construct in this body: at -O3 GCC otherwise tail-jumps into the callee (2 blocks/1 edge) where retail calls, cleans 0x24 argument bytes and returns, so the sink preserves the retail call/return shape at the cost of one stack store. It carries no logic and does not reorder anything. Texture_UploadMipmapPacket is transcribed in engine/src/retail/texture/texture_loaders.cpp. The three middle arguments are passed through without interpretation, so their types stay unestablished.
#include "igi/retail/texture/texture_loaders.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Resource_UploadMipmapPacket_4B64F0(void* context, std::int32_t index, std::int32_t argument3,
                                                           std::int32_t argument4, std::int32_t argument5,
                                                           std::int32_t argument6, std::int32_t argument7,
                                                           std::int32_t argument8, std::int32_t argument9)
{
    const std::uint8_t* const fields = static_cast<const std::uint8_t*>(context);
    const std::uint32_t first =                                        // 0x004B651B: mov edx,[ecx]
        *reinterpret_cast<const std::uint32_t*>(fields);
    const std::uint32_t second =                                       // 0x004B651D: mov ecx,[ecx+0x4]
        *reinterpret_cast<const std::uint32_t*>(fields + 0x4U);
    const std::uintptr_t record =                                      // 0x004B6520-0x004B6524: lea eax,[eax+eax*8]; lea edx,[ecx+eax*4]
        static_cast<std::uintptr_t>(second) + 0x24U * static_cast<std::uintptr_t>(index);

    // 0x004B6528: call 0x004B23B0 with the pushes at 0x004B64FC-0x004B6527; retail keeps the callee's eax
    // and returns it after cleaning 0x24 bytes of arguments, so the sink below preserves the call/return shape.
    const volatile std::int32_t result = ::igi::retail::texture::Texture_UploadMipmapPacket(
        reinterpret_cast<std::int32_t*>(record), reinterpret_cast<void*>(static_cast<std::uintptr_t>(first)),
        argument4, argument5, argument6, argument7, argument8, argument9,
        reinterpret_cast<void*>(static_cast<std::uintptr_t>(argument3)));
    return result;
}

} // extern "C"

#undef NATIVE_DECL
