// Native: Sys_InitGlobalListHead_4F1CD0; retail VA 0x004F1CD0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004F1CD0-0x004F1CE2 (19 bytes, 1 block, 0 edges, 6 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and NASM ndisasm over the exact bytes; sole call target 0x004F1D60.
// Behavior/CFG: one argument, the list head to initialise. The object at 0x004F1CF0 is pushed as the context argument and then the routine tail-calls the already transcribed initialiser 0x004F1D60(head, 0x004F1CF0), which stores context at head+0x0C, links head+0x00 to head+0x04, sets head+0x08 to head and clears head+0x04. Straight-line, no branch.
// Status/limits: STRUCTURALLY_VERIFIED. The 0x004F1CF0 object is only ever used by address: the retail image has exactly one reference to it (the push at 0x004F1CD4) and no read or write of its contents anywhere, so its size and type are unobservable; only its identity as a context value is established. The candidate therefore owns one zero-initialised byte and passes its address, which reproduces the only property retail demonstrates.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::sys {

// 0x004F1CF0: context object, referenced only by address (0x004F1CD4). Contents are never read or written in retail.
extern "C" std::uint8_t g_4f1cf0 = 0U;

void NATIVE_DECL Sys_InitListHead_4F1D60(void* head, std::int32_t context);

void NATIVE_DECL Sys_InitGlobalListHead_4F1CD0(void* head)
{
    Sys_InitListHead_4F1D60(head, reinterpret_cast<std::int32_t>(&g_4f1cf0));   // 0x4F1CD4: push $0x004F1CF0; 0x4F1CDA: call 0x004F1D60
}                                              // 0x4F1CE2: ret

} // namespace igi::retail::sys

#undef NATIVE_DECL
