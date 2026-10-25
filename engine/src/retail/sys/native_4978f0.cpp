// Native: Sys_EnumRenderModesPair_4978F0; retail VA 0x004978F0-0x00497915 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004978F0-0x00497915
//   (38 bytes, 1 block, 11 instructions; no branches).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf over the exact image. Body: `sub esp,8` builds a two-word local; `mov eax,[esp+0xC]` is argument 1
//   and `mov ecx,[esp+0x10]` argument 2; `lea edx,[esp]` takes the local's address; `mov [esp],eax` fills word 0; `push edx` then `push
//   0x00497920` are the two arguments of the following call (so argument 1 of the callee is the callback 0x00497920 and argument 2 is the
//   local's address); `mov [esp+0xC],ecx` fills word 1 after those pushes, at the pre-push [esp+4]; then `call 0x004B0EE0`, `add esp,0x10`, ret.
//   0x004B0EE0 is the transcribed sys/native_4b0ee0.cpp (Sys_EnumRenderModes_4B0EE0(callback, int32 argument)), whose callback receives
//   (RenderModeEntry*, argument), so the local's address travels to the callback as its second parameter.
// Behavior/CFG: builds the two-word local {first, second}, then enumerates the render modes through 0x004B0EE0 with callback 0x00497920 and the
//   local's address as the callback argument; nothing is read back from the local and nothing is returned.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The callback 0x00497920 is a raw code immediate in retail and is not transcribed yet (native
//   gap). Retail fills word 1 of the local between the two pushes and the call; this transcription fills both words before the call, which is
//   the only order that preserves the observable state the callback sees (the stores themselves are not observable in between). The address is
//   passed as the callee's `std::int32_t argument`, exactly as retail passes it; that is only correct on the 32-bit target, and the cast says so.
//   The two-word local is an anonymous struct rather than an array: an address-taken array makes host clang emit a stack-protector call
//   (___stack_chk_fail/___stack_chk_guard) that retail does not have, while the same 8 bytes as two scalars keep the frame retail-sized.
// The incomplete RenderModeEntry below is the same type the callee's signature uses (declared in sys/native_4b0ee0.cpp at global scope), so the
// callback pointer is passed at the callee's own type instead of an invented one.
#include <cstdint>

struct RenderModeEntry; // same type as the callback parameter of Sys_EnumRenderModes_4B0EE0

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" void Sys_EnumRenderModes_4B0EE0(void (NATIVE_DECL* callback)(RenderModeEntry*, std::int32_t),
                                           std::int32_t argument); // 0x004B0EE0

extern "C" {

void NATIVE_DECL Sys_EnumRenderModesPair_4978F0(std::uint32_t first, std::uint32_t second)
{
    struct { std::uint32_t word0; std::uint32_t word1; } pair;      // 0x004978F0: sub esp,8 (retail's two-word local)
    pair.word0 = first;                                                // 0x004978FB/0x004978FF: lea edx,[esp]; mov [esp],eax (eax = [esp+0xC] = argument 1)
    pair.word1 = second;                                               // 0x00497909: mov [esp+0xC],ecx (ecx = [esp+0x10] = argument 2; word 1 of the local)
    Sys_EnumRenderModes_4B0EE0(
        reinterpret_cast<void (NATIVE_DECL*)(RenderModeEntry*, std::int32_t)>(
            static_cast<std::uintptr_t>(0x00497920U)),              // 0x00497904: push 0x00497920 (callee argument 1)
        static_cast<std::int32_t>(
            reinterpret_cast<std::uintptr_t>(&pair)));              // 0x00497903: push edx (callee argument 2, the local's address)
    // 0x00497912: add esp,0x10 (8-byte local + 2 argument pushes); 0x00497915: ret.
}

} // extern "C"

#undef NATIVE_DECL
