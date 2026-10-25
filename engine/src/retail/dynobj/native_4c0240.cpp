// Native: DynObj_GetField04_4C0240; retail VA 0x004C0240 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C0240-0x004C0249 (10 bytes, 1 block, 0 edges, 4 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (8b 44 24 04 8b 08 8b 41 04 c3); nine nop bytes follow.
// Behavior/CFG: the same shape as 0x004C0230 with a different field: returns the dword at (*(argument))+0x04. No branch, no store, no callee.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. Nothing here establishes the struct type, only the offset.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL DynObj_GetField04_4C0240(void* holder)
{
    void* const inner = *reinterpret_cast<void* const*>(holder);        // 0x004C0244: mov ecx,[eax]
    return *reinterpret_cast<const std::int32_t*>(                   // 0x004C0246: mov eax,[ecx+0x4]
        static_cast<std::uint8_t*>(inner) + 0x04U);
}

} // extern "C"

#undef NATIVE_DECL
