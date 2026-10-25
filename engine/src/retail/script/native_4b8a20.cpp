// Native: Script_EvalArgInt_4B8A20; retail VA 0x004B8A20 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B8A20-0x004B8A4D (46 bytes, 3 blocks, 2 edges, 17 instructions).
// Tools/evidence: radare2 6.2.0 af/pd over the exact bytes (8b 44 24 04 8b 48 04 85 c9 74 0f ...), re-read from the retail image; 231 direct
//   call sites in the retail .text; declared as std::int32_t(void*, std::int32_t) by the QTask handler natives 0x0042DB00 and 0x0042FE80
//   (engine/src/retail/core/native_42db00.cpp line 59, native_42fe80.cpp line 51 area).
// Behavior/CFG: two stack arguments, no prologue: [esp+4] = argument frame, [esp+8] = 32-bit index. Loads the frame flag dword at +0x04 and
//   branches on it. When it is non-zero the value comes from Qvm_ReadIndexedInteger_4BCAB0(frame, index) - cdecl, two pushes (index then
//   frame), 8-byte cleanup at 0x004B8A36. When it is zero the argument table word at +0x08 is indexed with a 32-bit scale-4 read
//   (`mov (%edx,%eax,4),%ecx`), that entry is passed as the single argument of Qvm_EvaluateIntegerNode_4B9120 and the 4-byte cleanup
//   follows at 0x004B8A4A. Both exits return the callee's EAX unchanged and end with a plain ret; no branch result is masked or extended.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The file does not link yet: neither 0x004BCAB0 (41 bytes, 1 block, 14 instructions) nor
//   0x004B9120 (1188 bytes, 65 blocks, 484 instructions) has an engine native, and they are declared here with NATIVE_DECL exactly as
//   native_42dac0.cpp declares its callees. The GameNatives candidate for this address
//   (scripts/GameNatives/004b8a20-Script_EvalArgInt.cpp) is a Binary Ninja pseudo-C dump and disagrees with the bytes: its prototype is
//   int32_t(float, int32_t), its callee arguments are not the retail's two (it passes four words plus an uninitialised local), and it names
//   the callees Sys_ReadFileBlock_24 / Sys_AllocateMemory_28, which is not supported by the bodies. This transcription follows the retail
//   instructions only; the flag test direction (+0x04 nonzero selects the indexed-reader branch) is the retail `test %ecx,%ecx; je`.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x004BCAB0 - reads argument `index` through the interpreter frame's indexed reader and returns it as an integer in EAX (it clears a
// 4-byte local, calls 0x004BCAE0 with selector 1 and returns that local).
std::int32_t NATIVE_DECL Qvm_ReadIndexedInteger_4BCAB0(void* argument_frame, std::int32_t index);

// 0x004B9120 - evaluates one argument value node as an integer (it dispatches on the node's signed type byte at +0x01 through the jump
// table at 0x004B95C4 and returns EAX).
std::int32_t NATIVE_DECL Qvm_EvaluateIntegerNode_4B9120(const void* node);

std::int32_t NATIVE_DECL Script_EvalArgInt_4B8A20(void* argument_frame, std::int32_t index)
{
    std::uint8_t* const frame = static_cast<std::uint8_t*>(argument_frame);        // 0x004B8A20: mov 0x4(%esp),%eax

    if (*reinterpret_cast<std::uint32_t*>(frame + 0x04) != 0U)                     // 0x004B8A24/0x004B8A27/0x004B8A29: mov 0x4(%eax),%ecx; test; je
    {
        return Qvm_ReadIndexedInteger_4BCAB0(argument_frame, index);               // 0x004B8A2F-0x004B8A36: push index; push frame; call 0x004BCAB0; add esp,8
    }

    const std::uint32_t table = *reinterpret_cast<std::uint32_t*>(frame + 0x08);   // 0x004B8A3A: mov 0x8(%eax),%edx
    const std::uint32_t node = *reinterpret_cast<std::uint32_t*>(
        reinterpret_cast<std::uint8_t*>(static_cast<std::uintptr_t>(table)) +
        static_cast<std::uint32_t>(index) * 4U);                                   // 0x004B8A3D/0x004B8A41: mov 0x8(%esp),%eax; mov (%edx,%eax,4),%ecx
    return Qvm_EvaluateIntegerNode_4B9120(                                         // 0x004B8A44-0x004B8A4A: push %ecx; call 0x004B9120; add esp,4
        reinterpret_cast<const void*>(static_cast<std::uintptr_t>(node)));
}                                                                                  // 0x004B8A4D: ret

} // extern "C"

#undef NATIVE_DECL
