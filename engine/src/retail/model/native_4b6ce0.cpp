// Native: Model_GetBoneWord0C_4B6CE0; retail VA 0x004B6CE0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B6CE0 (see the per-instruction comments; 7 instructions, 1 blocks, 0 edges).
// Tools/evidence: radare2 6.2.0 af/afij/pdf and NASM ndisasm over the exact bytes; it reads the word at model+0x14, the pointer at model+0x08 and a signed word at that pointer plus index*36 + 0x0C
// Behavior/CFG: reads a 16-bit index at object+0x14, a table pointer at object+0x08, and returns the sign-extended word at table + index*36 + 0x0C. No branch, no store, no call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The object's type is not established beyond the offsets this body reads.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::model {

std::int32_t NATIVE_DECL Model_GetLevelWord_4B6CE0(void* model)
{
    const std::uint16_t index = *reinterpret_cast<const std::uint16_t*>(   // 0x4B6CE6: mov 0x14(%ecx),%ax
        static_cast<std::uint8_t*>(model) + 0x14U);
    const std::uint8_t* const table = *reinterpret_cast<std::uint8_t* const*>( // 0x4B6CEA: mov 0x8(%ecx),%ecx
        static_cast<std::uint8_t*>(model) + 0x08U);
    return static_cast<std::int32_t>(*reinterpret_cast<const std::int16_t*>( // 0x4B6CF0: movswl 0xC(%ecx,%eax,4),%eax
        table + (static_cast<std::uint32_t>(index) * 9U) * 4U + 0x0CU));
}

} // namespace igi::retail::model

#undef NATIVE_DECL
