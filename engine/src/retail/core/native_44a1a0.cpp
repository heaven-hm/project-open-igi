// Native: Task_Handler_44A1A0 (placeholder name for the event 2 handler that Task_DefineGlassShatter_449D30 links at 0x00449E33); retail VA 0x0044A1A0-0x0044A1B1.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 18 bytes (0x12), 1 block, 0 edges,
//   6 instructions, counted with af + afbj over the exact image. The body ends at the `ret` at 0x0044A1B1; fourteen 0x90 pad bytes follow at
//   0x0044A1B2-0x0044A1BF and the next function starts at 0x0044A1C0.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the objdump listing build/retail-dis.txt. The complete listing of the extent is `mov 0x4(%esp),%eax`
//   (0x0044A1A0), `mov 0x2b040(%eax),%ecx` (0x0044A1A4), `push %ecx` (0x0044A1AA), `call 0x004015F0` (0x0044A1AB), `pop %ecx` (0x0044A1B0),
//   `ret` (0x0044A1B1) - six instructions, no branch, no other callee and no data reference of its own. The argument slot is released by
//   `pop %ecx` rather than `add $0x4,%esp`, so the pushed value is discarded into ecx.
// Behavior/CFG: the handler ABI is `void(void* context)`, the same shape as every other transcribed handler in this tree (FrameHandler in
//   core/task_types.hpp is `void (*)(void*)`): the context arrives as the single stack argument. It reads the pointer at context+0x2B040 - the
//   same +0x2B040 field that 0x00449D30's registration sizes the task object for, 0x2B048 bytes - and passes it to the task destructor
//   0x004015F0, then returns. No branch, no loop, no global written, no return value.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 6-instruction listing and afbj (18 B / 1 block / 0 edges); runtime UNOBSERVED.
//   Recorded limits: (a) 0x0044A1A0 is an address-suffixed placeholder name; the routine's retail symbol is not established, only its role as
//   the event 2 handler linked by 0x00449D30 for the "GlassShatter" class. (b) The type of the object at context+0x2B040 is not established by
//   these six instructions, so it is handled as void*, which is what the destructor's own transcribed signature takes. (c) The effect of the
//   destructor itself is that file's declared scope, not this one's. (d) Codegen note: retail does `push %ecx; call 0x4015F0; pop %ecx; ret`,
//   while the port's compiled object emits `mov %eax,0x4(%esp); jmp 0x4015F0` - GCC's tail-call form. The argument and the callee are the same,
//   the callee returns straight to this handler's caller, and ecx is caller-saved scratch, so the difference is not observable through the
//   stack, the callee or the return value.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern "C" void QTask_DestroyTask_4015F0(void* task); // 0x004015F0: the task destructor (core/native_4015f0.cpp)

void NATIVE_DECL Task_Handler_44A1A0(void* const context)
{
    // 0x0044A1A0: mov 0x4(%esp),%eax (the context argument); 0x0044A1A4: mov 0x2b040(%eax),%ecx.
    void* const object = *reinterpret_cast<void**>(static_cast<char*>(context) + 0x2B040);

    // 0x0044A1AA: push %ecx; 0x0044A1AB: call 0x4015F0; 0x0044A1B0: pop %ecx (the argument dword is popped into ecx and discarded);
    // 0x0044A1B1: ret.
    QTask_DestroyTask_4015F0(object);
}

} // extern "C"

#undef NATIVE_DECL
