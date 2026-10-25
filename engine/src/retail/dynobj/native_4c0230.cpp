// Native: DynObj_GetField0C_4C0230; retail VA 0x004C0230 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C0230-0x004C0239 (10 bytes, 1 block, 0 edges, 4 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (8b 44 24 04 8b 08 8b 41 0c c3); nine nop bytes follow, then the next entry.
// Behavior/CFG: one cdecl argument is a pointer to a pointer; loads the inner pointer and returns the dword at inner+0x0C. No branch, no store, no callee, no bounds check.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. Both loads are dereferences of caller-supplied pointers with no validation, exactly as retail; nothing here establishes the struct type, only the two offsets.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL DynObj_GetField0C_4C0230(void* holder)
{
    void* const inner = *reinterpret_cast<void* const*>(holder);        // 0x004C0234: mov ecx,[eax]
    return *reinterpret_cast<const std::int32_t*>(                   // 0x004C0236: mov eax,[ecx+0xC]
        static_cast<std::uint8_t*>(inner) + 0x0CU);
}

} // extern "C"

#undef NATIVE_DECL
