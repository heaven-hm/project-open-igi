// Native: DynObj_InvokeField18_4BF1E0; retail VA 0x004BF1E0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004BF1E0-0x004BF1EB (12 bytes, 1 block, 0 edges, 5 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the raw bytes (8b 44 24 04 50 ff 50 18 83 c4 04 c3); nine nop bytes follow, then the next entry.
// Behavior/CFG: one cdecl argument is passed unchanged as the single argument of a call through the handler pointer stored at object+0x18, then the callee's value is returned. No branch, no store, no global.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The handler is fetched from the object, so there is no direct callee and the object's type is not established beyond the +0x18 slot. An unset slot would call null, exactly as retail would.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

using DynObjField18Handler = std::int32_t(NATIVE_DECL*)(void* object);

std::int32_t NATIVE_DECL DynObj_InvokeField18_4BF1E0(void* object)
{
    void* const handler = *reinterpret_cast<void* const*>(           // 0x004BF1E5: call *0x18(%eax)
        static_cast<std::uint8_t*>(object) + 0x18U);
    return reinterpret_cast<DynObjField18Handler>(handler)(object);  // 0x004BF1E4: push eax; 0x004BF1E8: add esp,4
}

} // extern "C"

#undef NATIVE_DECL
