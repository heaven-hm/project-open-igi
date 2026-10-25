// Native: Qvm_GetFloatArg; retail VA 0x004B8A50 (tree label; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8A50-0x004B8A7B (44 bytes, 3 blocks, 2 edges, 17 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (8b 44 24 04 8b 48 04 85 c9 74 0f ...), re-read from the retail image; 201 direct call sites in the retail .text; the identical transcription already sits at engine/src/retail/script/qvm_execute.cpp:200 with its shared declaration at engine/include/igi/retail/script/qvm_execute.hpp:68 (the manifest records VA 0x004B8A50 -> that path, STRUCTURALLY_VERIFIED), and that file is not in the CMake source list, which is why the QTask handler natives fail to link. This file is the address-bound implementation the one-file-per-VA rule requires; the qvm_execute.cpp copy (which reads through file-local load_word/pointer_from_word helpers and memcpy) is the duplicate the integrator has to retire.
// Behavior/CFG: [esp+4] = argument frame, [esp+8] = 32-bit index, no prologue. The frame flag dword at +0x04 is tested at 0x004B8A57: when it is non-zero the value comes from Qvm_ReadIndexedArgument(frame, index) (cdecl, two pushes - index then frame - 8-byte cleanup at 0x004B8A66). When it is zero the argument table word at +0x08 is indexed with a 32-bit scale-4 read (`mov (%edx,%eax,4),%ecx` at 0x004B8A71), that entry is the single argument of Qvm_EvaluateArgumentValue and the cleanup after it is `pop %ecx` (0x004B8A7A). The accessor performs no conversion, store or x87 arithmetic of its own: both exits hand the callee's ST0 value straight back, which is why it is declared long double and why the callers consume it with fmuls/fstps.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; does not link yet because 0x004BD010 (41 bytes, 1 block, 14 instructions) and 0x004B9660 (1265 bytes, 75 blocks, 447 instructions) have no engine native. Candidate check against scripts/GameNatives/qvm/004B8A50-Qvm_GetFloatArg.cpp: the entry, 3 blocks/2 edges, the +0x04 flag, the +0x08 scale-4 table and the two ST0 callees AGREE with the retail bytes. Recorded disagreements: (a) this body has no char-sized indexed branch - the indexed access is the 32-bit `mov (%edx,%eax,4),%ecx` and there is no byte load at 0x004B8A50 (the only byte store in this family is inside 0x004BD040); (b) the candidate reads through two file-local helpers plus memcpy, which the no-helper rule forbids; (c) it names the callees _4BD010/_4B9660 while qvm_execute.cpp and this file keep one name per VA (Qvm_ReadIndexedArgument/Qvm_EvaluateArgumentValue). The archived profile-path dump docs/fidelity/evidence/superseded/004B8A50-Profile_ValueSet-ghidra-dump-20261023.cpp is the earlier wrong reconstruction the manifest warns about: uninitialised return values on the indexed branch and the misfiled Profile_ValueSet identity.
#include "igi/retail/script/qvm_execute.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BD010 - reads argument `index` through the interpreter frame's indexed reader and returns it as a real in ST0 (it fills a 4-byte
// local through 0x004BCAE0 with selector 2 and ends with `flds 0x14(%esp)`).
long double NATIVE_DECL Qvm_ReadIndexedArgument(void* argument_frame, std::int32_t index);

// 0x004B9660 - evaluates one argument value node and leaves its real value in ST0 (it dispatches on the node's signed type byte at +0x01 and
// falls back on the constant 0.0 at 0x005333B0 after reporting the expression error).
long double NATIVE_DECL Qvm_EvaluateArgumentValue(const void* node);

long double NATIVE_DECL Qvm_GetFloatArg(void* argument_frame, std::int32_t index)
{
    std::uint8_t* const frame = static_cast<std::uint8_t*>(argument_frame);        // 0x004B8A50: mov 0x4(%esp),%eax

    if (*reinterpret_cast<std::uint32_t*>(frame + 0x04) != 0U)                     // 0x004B8A54/0x004B8A57/0x004B8A59: mov 0x4(%eax),%ecx; test; je
    {
        return Qvm_ReadIndexedArgument(argument_frame, index);                     // 0x004B8A5F-0x004B8A66: push index; push frame; call 0x004BD010; add esp,8
    }

    const std::uint32_t table = *reinterpret_cast<std::uint32_t*>(frame + 0x08);   // 0x004B8A6A: mov 0x8(%eax),%edx
    const std::uint32_t node = *reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(table)) +
        static_cast<std::uint32_t>(index) * 4U);                                   // 0x004B8A6D/0x004B8A71: mov 0x8(%esp),%eax; mov (%edx,%eax,4),%ecx
    return Qvm_EvaluateArgumentValue(                                              // 0x004B8A74/0x004B8A75/0x004B8A7A: push %ecx; call 0x004B9660; pop %ecx
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(node)));
}                                                                                  // 0x004B8A7B: ret

} // extern "C"

#undef NATIVE_DECL
