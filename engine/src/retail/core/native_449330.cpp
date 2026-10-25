// Native: Task_DispatchAndRelease_449330 (behavior-derived placeholder: it dispatches through the handler table at 0x00A976E0 and then releases a region of its argument; retail symbol unknown); retail VA 0x00449330-0x00449359.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 42 bytes (0x2A), 1 block, 0 edges, 13 instructions, counted over the exact image.
// Tools/evidence: build/retail-dis.txt, complete extent: push %esi (0x00449330); call 0x00481340 (0x00449331); mov 0x8(%esp),%esi (0x00449336 - the argument, read after the push);
//   and $0xffff,%eax (0x0044933A); push %esi (0x0044933F); mov 0xa976e0(,%eax,4),%eax (0x00449340 - table lookup, index scaled by 4); call *%eax (0x00449347);
//   add $0x278,%esi (0x00449349); push %esi (0x0044934F); call 0x004F1340 (0x00449350); add $0x8,%esp (0x00449355); pop %esi (0x00449358); ret (0x00449359). No conditional branch.
//   The callee 0x00481340 is Sys_GetGlobal16_481340 (transcribed); the table base 0x00A976E0 is in the image's zero-fill region above 0x00552000, so its entries exist only at runtime.
// Behavior/CFG: no loop, no return value. It calls 0x00481340 first, masks the result with 0xFFFF, and uses that as an index into the table at 0x00A976E0, calling the 4-byte entry it finds there with the
//   argument as its single argument. It then calls 0x004F1340 with the argument advanced by 0x278. The two adds release 2 argument dwords total (1 + 1). The dispatch target is not statically known:
//   the port keeps the same table lookup and indirect call rather than resolving or enumerating the table, because the entries are runtime data.
// Codegen divergence (same callees, same arguments, same order): retail reaches 0x004F1340 with `call` and then cleans both dwords itself; GCC tail-calls it
//   (`add $0x4,%esp; pop %ebx; jmp Sub_4F1340`) and places the second argument in the incoming argument slot, which is the correct cdecl tail-call ABI - the
//   callee returns straight to this routine's caller, which cleans that single argument exactly as it would have for this routine. The indirect dispatch stays a real call.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 13-instruction listing; runtime UNOBSERVED. Recorded limits: (a) placeholder name with the address kept as its suffix. (b) 0x004F1340 is
//   untranscribed and is reported undefined in the tree; that is the true state. (c) The table's element count is NOT established by this body, so it is declared as an array of unknown bound - no size is claimed.
//   (d) The indexed callee is carried as void(*)(void*) because exactly one 32-bit argument is pushed before `call *%eax`; the table may in principle hold other entry kinds, and this body cannot tell.
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

extern std::uint32_t Sys_GetGlobal16_481340();                      // 0x00481340: transcribed (core/native_443680.cpp)
extern void Sub_4F1340(void* region);                               // 0x004F1340: untranscribed

// 0x00A976E0: the dispatch table. Zero-fill in the image, so it is populated at runtime; its length is not established by this body.
extern void (*g_a976e0[])(void*);

void NATIVE_DECL Task_DispatchAndRelease_449330(void* const object)
{
    NATIVE_LOG_ENTER("Task_DispatchAndRelease_449330", "0x00449330");
    // 0x00449331-0x00449347: index = 0x481340() & 0xFFFF; g_a976e0[index](object).
    g_a976e0[Sys_GetGlobal16_481340() & 0xFFFFU](object);

    // 0x00449349-0x00449350: call 0x4F1340(object + 0x278).
    Sub_4F1340(static_cast<char*>(object) + 0x278);
}

} // extern "C"

#undef NATIVE_DECL
