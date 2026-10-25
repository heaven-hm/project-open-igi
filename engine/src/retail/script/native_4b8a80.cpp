// Native: Sys_ResolveSymbol_23_4B8A80; retail VA 0x004B8A80 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8A80-0x004B8AC1 (66 bytes, 3 blocks, 2 edges, 25 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (8b 44 24 04 8b 48 04 85 c9 74 19 ...), re-read from the retail image; 75 direct call sites in the retail .text; declared as char*(void*, std::int32_t, void*, std::uint32_t) by the QTask handler natives 0x0042DB00, 0x0042DCE0 and 0x0042FE80 (engine/src/retail/core/native_42db00.cpp, native_42fe80.cpp); catalogue entry assets/IGINatives_All_GameNatives.json records VA 0x004B8A80 as Sys_ResolveSymbol_23 with Binary Ninja prototype char*(float arg1, int32_t arg2, char* arg3, char* arg4) - the float type on arg1 is stale, the body dereferences arg1 ([eax+4]).
// Behavior/CFG: four stack arguments, no prologue: [esp+4] = argument frame, [esp+8] = 32-bit index, [esp+0xC] = destination buffer, [esp+0x10] = count. The frame flag dword at +0x04 is tested at 0x004B8A87. When it is non-zero the call is Qvm_ReadIndexedBlock_4BD040(frame, index, destination, count) with four pushes and `add esp,0x10` at 0x004B8AA0; when it is zero the argument table word at +0x08 is indexed with a 32-bit scale-4 read (`mov (%edx,%eax,4),%ecx`) and the call is Qvm_RenderNodeText_4B8CE0(node, destination, count) with three pushes and `add esp,0xC` at 0x004B8ABE. Both exits return the callee's EAX unchanged. Retail argument staging: destination and count are loaded before the pushes and frame/index are re-read from the shifted stack ([esp+0xC] after the first push), which is the compiler's shuffle of the same four arguments, not extra arguments.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; does not link yet because 0x004BD040 (38 bytes, 1 block, 16 instructions) and 0x004B8CE0 (778 bytes, 41 blocks, 262 instructions) have no engine native and are declared here with NATIVE_DECL. The name's `_23` suffix carries no meaning recovered from this body: the same suffix labels the neighbouring catalogue entry 0x004B8AD0 (Sys_CheckHeap_23), the body takes four arguments and never compares anything against 23, so the suffix is a catalogue grouping label and is not evidence of an argument count or a symbol class. The count word is 0x10 and 0x20 at the two handler call sites, but how 0x004B8CE0 consumes it (a capacity or a byte count) is inside the untranscribed callee, so it stays unestablished here. The return value is ignored by those call sites and the callee's two observed exits give 0 and the destination pointer, so char* is the callers' type and not a proven result.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BD040 - clears the destination's first byte, fills it through 0x004BCAE0 with selector 3 and returns the destination pointer in EAX.
char* NATIVE_DECL Qvm_ReadIndexedBlock_4BD040(void* argument_frame, std::int32_t index,
                                              void* destination, std::uint32_t count);

// 0x004B8CE0 - resolves one argument value node into the destination bytes; its null-node path stores the two-byte constant at 0x005474D8
// (0x20, 0x00) and returns 0, and its other paths dispatch on the node's signed type byte at +0x01 (jump table at 0x004B9000).
char* NATIVE_DECL Qvm_RenderNodeText_4B8CE0(const void* node, char* destination,
                                                std::uint32_t count);

char* NATIVE_DECL Sys_ResolveSymbol_23_4B8A80(void* argument_frame, std::int32_t index,
                                              void* destination, std::uint32_t count)
{
    NATIVE_LOG_ENTER("Sys_ResolveSymbol_23_4B8A80", "0x004B8A80");
    std::uint8_t* const frame = static_cast<std::uint8_t*>(argument_frame);        // 0x004B8A80: mov 0x4(%esp),%eax

    if (*reinterpret_cast<std::uint32_t*>(frame + 0x04) != 0U)                     // 0x004B8A84/0x004B8A87/0x004B8A89: mov 0x4(%eax),%ecx; test; je
    {
        return Qvm_ReadIndexedBlock_4BD040(argument_frame, index, destination, count); // 0x004B8A8B-0x004B8AA0: four pushes; call 0x004BD040; add esp,0x10
    }

    const std::uint32_t table = *reinterpret_cast<std::uint32_t*>(frame + 0x08);   // 0x004B8AAD: mov 0x8(%eax),%edx
    const std::uint32_t node = *reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(table)) +
        static_cast<std::uint32_t>(index) * 4U);                                   // 0x004B8AB0/0x004B8AB5: mov 0xc(%esp),%eax; mov (%edx,%eax,4),%ecx
    return Qvm_RenderNodeText_4B8CE0(                                          // 0x004B8AB8-0x004B8ABE: three pushes; call 0x004B8CE0; add esp,0xC
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(node)), static_cast<char*>(destination), count);
}                                                                                  // 0x004B8AC1: ret

} // extern "C"

#undef NATIVE_DECL
