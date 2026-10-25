// Native: Task_ReleaseThree_449950 (behavior-derived placeholder: it releases its argument through three separate callees in retail's order; retail symbol unknown); retail VA 0x00449950-0x0044996F.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 32 bytes (0x20), 1 block, 0 edges, 13 instructions, counted over the exact image.
// Tools/evidence: build/retail-dis.txt, complete extent: push %esi (0x00449950); mov 0x8(%esp),%esi (0x00449951 - the argument, read after the push); push %esi (0x00449955); call 0x004168D0 (0x00449956);
//   push %eax (0x0044995B); call 0x004C17C0 (0x0044995C); mov 0x68(%esi),%eax (0x00449961); push %esi (0x00449964); push %eax (0x00449965); call 0x004C7560 (0x00449966);
//   add $0x10,%esp (0x0044996B); pop %esi (0x0044996E); ret (0x0044996F). No branch. The single add releases all four pushed argument dwords (1 + 1 + 2), so all three callees are cdecl.
// Behavior/CFG: no loop, no return value. In order: call 0x004168D0(argument), then pass that call's result (not the argument) to 0x004C17C0, whose own result is discarded,
//   then call 0x004C7560 with the dword at argument+0x68 first and the argument itself second. esi is preserved across all three calls by push/pop, so the second callee receives the first callee's return value and nothing is re-read.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 13-instruction listing; runtime UNOBSERVED. Recorded limits: (a) placeholder name with the address kept as its suffix. (b) All three callees
//   (0x004168D0, 0x004C17C0, 0x004C7560) are untranscribed, so this file reports them undefined in the tree - that is the true state. (c) Their signatures are carried as (void*) -> std::uint32_t because each
//   is called with one 32-bit stack argument; the first one's return value is passed on as the second call's argument, and the third takes two. No type beyond that width is claimed.
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

extern std::uint32_t Sub_4168D0(void* object);                      // 0x004168D0: untranscribed
// 0x004C17C0: implemented in core/qtask_list.cpp as QTaskList_Remove(void*, void*). It takes
// TWO arguments, and this call site supplies both - see the note below on where arg2 comes from.
extern std::int32_t QTaskList_Remove(void* task_list, void* task);
extern void Sub_4C7560(std::uint32_t field, void* object);          // 0x004C7560: untranscribed

void NATIVE_DECL Task_ReleaseThree_449950(void* const object)
{
    NATIVE_LOG_ENTER("Task_ReleaseThree_449950", "0x00449950");
    // 0x00449955-0x0044995C: call 0x4168D0(object), then 0x4C17C0(result, object).
    //
    // Only ONE dword is pushed immediately before `call 0x4C17C0` (push eax at 0x0044995B), yet
    // the callee reads two arguments. The second is the `push esi` at 0x00449955: the single
    // `add esp,0x10` at 0x0044996B defers ALL cleanup to the end of the routine, so that push is
    // still on the stack and becomes arg2.
    //
    // Entering 0x004C17C0 esp is E-16 (E = esp on entry to this routine): push esi -> E-20 so
    // [esp+8] = E-12 = eax = arg1; push edi -> E-24 so [esp+0x10] = E-8 = the object = arg2.
    //
    // Push-counting at this site cannot see arg2. Counting the RUNNING esp delta can, and the
    // delta must be restored after each `call` returns - carrying a callee's esp forward past its
    // return is the error that first made this look like a one-argument call.
    QTaskList_Remove(reinterpret_cast<void*>(static_cast<std::uintptr_t>(Sub_4168D0(object))),
                     object);

    // 0x00449961-0x00449966: call 0x4C7560(object[0x68], object) - the field first, the object second.
    Sub_4C7560(*reinterpret_cast<std::uint32_t*>(static_cast<char*>(object) + 0x68), object);
}

} // extern "C"

#undef NATIVE_DECL
