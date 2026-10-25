// Native: Sys_InvokeSlotWord_4B1000; retail VA 0x004B1000 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1000-0x004B101F (32 bytes, 1 block, 0 edges, 11 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet (landability sweep row).
// Behavior/CFG: computes first*0x82 + second like 0x004B0EC0, then calls the dword at 0x00A950A4 indexed by that value with the third argument; the pushed argument is removed with pop ecx after the call. Returns the callee's EAX; no branch.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The table entry is written by 0x004B0EC0 and starts zeroed, so an unset slot is a null call in this build, exactly as retail's empty table implies. The callee's return type is unproven.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern std::uint32_t g_a950a4[0x2000];

std::uint32_t NATIVE_DECL Sys_InvokeSlotWord_4B1000(std::int32_t first, std::int32_t second, std::int32_t argument)
{
    using SlotOperation = std::uint32_t (NATIVE_DECL*)(std::int32_t argument);

    const std::uint32_t index = static_cast<std::uint32_t>(second) +
                                static_cast<std::uint32_t>(first) * 0x82U;   // 0x004B1008-0x004B100F
    SlotOperation operation = reinterpret_cast<SlotOperation>(::g_a950a4[index]); // 0x004B1017: call dword [eax*4 + 0x00A950A4]
    return operation(argument);                                              // 0x004B1016/0x004B101E: push argument; call; pop ecx
}   // 0x004B101F: ret

} // extern "C"

#undef NATIVE_DECL
