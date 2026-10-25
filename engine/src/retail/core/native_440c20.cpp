// Native: Task_DefineSCameraControl (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00440C20-0x00440D25.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 262 bytes, 1 block, 71 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 71 instructions / 262 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x00440D25 and 0x00440D30 is the next
//   function, which this body uses as its first handler). `ps` at 0x0053C580 (the name literal, byte-identical to "SCameraControl"); `px 4` at
//   0x0057C170 (the container slot) and 0x0057C174 (the id slot), both image value 0. The prologue was read before any `lea` was resolved:
//   `sub esp,0x2F8` followed immediately by argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO.
//   A full push-by-push simulation closes exactly: the registration's `add esp,0x38` is fourteen pushes (the five registration pushes plus the
//   first three links), the fourth to seventh links' `add esp,0xC` is three each (four of them), the grow's `add esp,0x8` is its two argument
//   pushes, and the single append has no cleanup of its own - its six pushes are removed by the tail's `add esp,0x310` (0x310 = the 0x2F8 frame
//   plus 0x18). Every callee was swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an
//   existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x2F8-byte frame and no stack probe. It registers the "SCameraControl" task type of size 0x2F8 with
//   parent Sys_GetGlobal16_443E80() -> id at 0x0057C174, stores the id and links SEVEN handlers: e0->0x00440D30, e1->0x00440D70 and
//   e2->0x00440DE0 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x00440E40, 0x004F1A70->0x00440E50, 0x004F1210->0x00440EA0 and
//   0x004F1220->0x00440F30. It then grows a parameter container - `push $0x1` is left outstanding across the `call 0x443E90` so that it becomes
//   `Script_TaskContainer_Grow`'s SECOND argument while the getter's DWORD becomes its FIRST - i.e.
//   `0x0057C170 = Script_TaskContainer_Grow((int32_t*)Sys_GetGlobal32_443E90(), 1)`, and appends ONE record through 0x004F1A80 with recorded
//   delta 0x2A4 (from `lea 0x2B0(%esp)` with three pushes outstanding). One basic block, no branch, no loop, 17 call sites over 12 distinct callees.
//   **This body has NO 0x004F1950 allocation and NO 0x004B8810 script-name registration**; the container is a pre-existing object addressed by
//   0x0057C1A0, grown in place.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check is DEGENERATE here for the same reason as 0x00440710: the container is
//   grown by `Grow(container, 1)` on an object this body reads but does not create (0x0057C1A0), so the body establishes one static append site and a
//   request for one extra slot - not a capacity of its own.** `records <= capacity` remains the only claim.
//   This body is the closest sibling of 0x00440710 ("AlarmControl", 300 bytes): identical callee set, identical seven-link order with the same 3/4
//   split between 0x00401400 and 0x00401530, identical grow shape, and the ONLY differences are the name, the object size and frame (0x2F8 versus
//   0x350), the grow count (1 versus 2), the append count (1 versus 2) and the handler addresses. **The resemblance is recorded as a navigation aid
//   only - every field above was read from this body's own bytes; nothing was carried across from its sibling.** The deltas differ accordingly
//   (0x2A4 here versus 0x2AC and 0x2FC there), which a pattern-carried transcription would have got wrong.
//   The argument order is the trap in this body too: retail pushes `$0x1` BEFORE the getter call and pushes the getter's result AFTER it, so the
//   literal one is `Script_TaskContainer_Grow`'s second argument (the count) and the getter's DWORD is its first (the container pointer). The tree's
//   declaration is `void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count)`
//   (script/native_4f1980.cpp).
//   Both the container slot 0x0057C170 and the id slot 0x0057C174 are in writable `.data` (the section table puts `.rdata` at VA 0x00533000 with
//   virtual size 0x28F6, ending at 0x005358F6 and `.data` at VA 0x00536000), both have image value 0, so their initialisers are 0 and their symbol
//   class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification
//   rather than the section. **The id is the WORD at 0x0057C174 and the container pointer is the DWORD at the lower address 0x0057C170** - read from
//   the stores, not from the previous body's order. Arg6 of 0x004F1A80 is 0 at the single site, so this body takes the address of none of its own
//   slots. Deferred pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's
//   third argument. Handler immediates 0x00440D30, 0x00440D70, 0x00440DE0, 0x00440E40, 0x00440E50, 0x00440EA0 and 0x00440F30 are raw code
//   immediates with no implementation in the tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes
//   are not reproduced - the getter's inert `$0x1` here doubles as Grow's count, and the port expresses that by passing the literal one directly;
//   (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38,
//   0xC (four times), 0x8 and 0x310 are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the
//   port, which cannot change behaviour because the links that consume them go through 0x00401400 or 0x00401530, which mask the event word for the
//   phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_443E80(void);            // 0x00443E80: parent type getter (sys/native_443e80.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_443E90(void);            // 0x00443E90: `mov eax,[0x57C1A0]; ret` (sys/native_443e90.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret` (sys/native_4f0fc0.cpp)
extern "C" void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C170: the grown parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c170 = 0U;

// 0x0057C174: the "SCameraControl" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`. Only the
// low 16 bits are ever stored.
std::uint16_t g_57c174 = 0U;

void Task_DefineSCameraControl()
{
    // 0x00440C20: sub esp,0x2F8 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00440C26: push 0 (TaskType_Register's fourth argument, pushed first); 0x00440C28: push 0 (the parent getter's inert argument);
    // 0x00440C2A: call 0x443E80; 0x00440C2F: push eax; 0x00440C30: push 0x2F8; 0x00440C35: push "SCameraControl"; 0x00440C3A: call 0x401900.
    const std::uint16_t scamera_control_id = TaskType_Register("SCameraControl",         // 0x00440C35: push 0x0053C580
                                                               0x2F8U,                   // 0x00440C30: push 0x2F8 (object size, equal to the frame)
                                                               Sys_GetGlobal16_443E80(), // 0x00440C2A: call 0x443E80 (parent type)
                                                               0U);                     // 0x00440C28: push 0 (fourth argument)
    ::g_57c174 = scamera_control_id; // 0x00440C47: mov word [0x57C174],ax - between the pushes of the first link

    // 0x440C3F: push 0x440D30 (handler, deferred argument 3);
    // 0x440C44: push $0 (argument 2, literal event id); 0x440C4D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c174), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440D30U)));

    // 0x440C58: push 0x440D70 (handler, deferred argument 3);
    // 0x440C5D: push $1 (argument 2, literal event id); 0x440C60: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c174), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440D70U)));

    // 0x440C6C: push 0x440DE0 (handler, deferred argument 3);
    // 0x440C71: push $2 (argument 2, literal event id); 0x440C74: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c174), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440DE0U)));

    // 0x440C7C: push 0x440E40 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x440C8F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c174),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440E40U)));

    // 0x440C97: push 0x440E50 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x440CA9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c174),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440E50U)));

    // 0x440CB1: push 0x440EA0 (handler, deferred argument 3); the Sys_GetGlobal32_4F1210 getter supplies argument 2;
    // 0x440CC4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c174),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440EA0U)));

    // 0x440CCC: push 0x440F30 (handler, deferred argument 3); the Sys_GetGlobal32_4F1220 getter supplies argument 2;
    // 0x440CDF: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c174),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440F30U)));


    // 0x00440CE7: push 0x1 (Script_TaskContainer_Grow's SECOND argument, the count - pushed BEFORE the getter call and deliberately left
    // outstanding across it); 0x00440CE9: call 0x443E90 (argument 1, the container pointer, arrives in eax); 0x00440CEE: push eax (Grow's FIRST
    // argument); 0x00440CEF: call 0x4F1980; 0x00440CF4: add esp,0x8; 0x00440CF7: mov [0x57C170],eax.
    // The literal one is pushed first here only because retail reuses the getter's inert argument slot; the port states both arguments directly.
    ::g_57c170 = reinterpret_cast<std::uint32_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_443E90())), 1)); // 0x00440CE7: push 0x1

    // 0x00440CFC: push $0x0 (arg6); 0x00440CFE: push $0x7 (arg5); 0x00440D00: call 0x4F0FC0 (arg4);
    // 0x00440D0C: lea 0x2B0(%esp),%eax resolves to frame+0x2A4 (arg3, three pushes outstanding); 0x00440D13: lea 0xc(%esp),%ecx resolves to
    // frame+0x00 (arg2); 0x00440D1A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c170)),
                                append_frame, append_frame + 0x2A4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
    // 0x00440D1F: add esp,0x310 - the 0x2F8 frame plus the single append's six outstanding pushes; 0x00440D25: ret.
} // end of Task_DefineSCameraControl

} // extern "C"

