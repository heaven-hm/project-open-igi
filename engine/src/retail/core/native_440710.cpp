// Native: Task_DefineAlarmControl (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA
//   0x00440710-0x0044083B.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 300 bytes, 1 block, 83 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 83 instructions / 300 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0044083B and 0x00440840 is the next
//   function, which this body uses as its first handler). `ps` at 0x0053C540 (the name literal, byte-identical to "AlarmControl"); `px 4` at
//   0x0057C164 (the id slot) and 0x0057C168 (the container slot), both image value 0. The prologue was read before any `lea` was resolved:
//   `sub esp,0x350` followed immediately by argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO.
//   A full push-by-push simulation closes exactly: the registration's `add esp,0x38` is fourteen pushes (the five registration pushes plus the
//   first three links), the fourth to seventh links' `add esp,0xC` is three each (four of them), the grow's `add esp,0x8` is its two argument
//   pushes, the first append's `add esp,0x18` is six, and the second append has no cleanup of its own - its six pushes are removed by the tail's
//   `add esp,0x368` (0x368 = the 0x350 frame plus 0x18). Every callee was swept against the tree's `// Native:` provenance index with no lane
//   exemption and every one resolves to an existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x350-byte frame and no stack probe. It registers the "AlarmControl" task type of size 0x350 with
//   parent Sys_GetGlobal16_443E80() -> id at 0x0057C164, stores the id and links SEVEN handlers: e0->0x00440840, e1->0x004408A0 and
//   e2->0x00440970 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x004409E0, 0x004F1A70->0x004409F0, 0x004F1210->0x00440A40 and
//   0x004F1220->0x00440B20. It then grows a parameter container - **NOT with a literal capacity: `push $0x2` is left outstanding across the
//   `call 0x443E90` so that it becomes `Script_TaskContainer_Grow`'s SECOND argument, while the getter's DWORD result becomes its FIRST** -
//   i.e. `0x0057C168 = Script_TaskContainer_Grow((int32_t*)Sys_GetGlobal32_443E90(), 2)`, and appends TWO records through 0x004F1A80 with
//   recorded deltas 0x2AC and 0x2FC (from `lea 0x2B8/0x308(%esp)` with three pushes outstanding). One basic block, no branch, no loop, 19 call
//   sites over 12 distinct callees. **This body has NO 0x004F1950 allocation and NO 0x004B8810 script-name registration**: the container is a
//   pre-existing object addressed by 0x0057C1A0, grown in place.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check is DEGENERATE here and is reported as such: the container is grown by
//   Grow(container, 2) where the count argument is the literal two, and there are two static append sites, but the capacity lives in a container this
//   body did not create (0x0057C1A0, an object owned by Sys_GetGlobal32_443E90's own file), so the body cannot be said to have established a
//   capacity of its own.** What the body does establish is: two static append sites, and Grow asked for two. Whether the resulting container holds
//   more than two slots depends on the previous value at 0x0057C1A0, which this body reads but does not create - so `records <= capacity` is the only
//   claim, exactly as the 0x0043B850 correction requires.
//   The argument order is the trap in this body: retail pushes `$0x2` BEFORE the getter call and pushes the getter's result AFTER it, so the literal
//   two is `Script_TaskContainer_Grow`'s second argument (the count) and the getter's DWORD is its first (the container pointer). Reading the call as
//   `Grow(2, getter)` from source order inverts both arguments; the tree's declaration is
//   `void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count)` (script/native_4f1980.cpp).
//   Both the id slot 0x0057C164 and the container slot 0x0057C168 are in writable `.data` (the section table puts `.rdata` at VA 0x00533000 with
//   virtual size 0x28F6, ending at 0x005358F6 and `.data` at VA 0x00536000), both have image value 0, so their initialisers are 0 and their symbol
//   class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification
//   rather than the section. Arg6 of 0x004F1A80 is 0 at both sites (checked per site), so this body takes the address of none of its own slots.
//   Deferred pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's third
//   argument. Handler immediates 0x00440840, 0x004408A0, 0x00440970, 0x004409E0, 0x004409F0, 0x00440A40 and 0x00440B20 are raw code immediates with
//   no implementation in the tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail
//   that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced - the
//   getter's inert `$0x2` here doubles as Grow's count, and the port expresses that by passing the literal two directly; (2) the cleanups are
//   reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (four times),
//   0x8, 0x18 and 0x368 are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port, which
//   cannot change behaviour because the links that consume them go through 0x00401400 or 0x00401530, which mask the event word for the phase index.
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

// 0x0057C164: the "AlarmControl" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`. Only the low
// 16 bits are ever stored.
std::uint16_t g_57c164 = 0U;

// 0x0057C168: the grown parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c168 = 0U;

void Task_DefineAlarmControl()
{
    // 0x00440710: sub esp,0x350 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00440716: push 0 (TaskType_Register's fourth argument, pushed first); 0x00440718: push 0 (the parent getter's inert argument);
    // 0x0044071A: call 0x443E80; 0x0044071F: push eax; 0x00440720: push 0x350; 0x00440725: push "AlarmControl"; 0x0044072A: call 0x401900.
    const std::uint16_t alarm_control_id = TaskType_Register("AlarmControl",         // 0x00440725: push 0x0053C540
                                                             0x350U,                  // 0x00440720: push 0x350 (object size, equal to the frame)
                                                             Sys_GetGlobal16_443E80(), // 0x0044071A: call 0x443E80 (parent type)
                                                             0U);                     // 0x00440718: push 0 (fourth argument)
    ::g_57c164 = alarm_control_id; // 0x00440737: mov word [0x57C164],ax - between the pushes of the first link

    // 0x44072F: push 0x440840 (handler, deferred argument 3);
    // 0x440734: push $0 (argument 2, literal event id); 0x44073D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c164), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440840U)));

    // 0x440748: push 0x4408A0 (handler, deferred argument 3);
    // 0x44074D: push $1 (argument 2, literal event id); 0x440750: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c164), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4408A0U)));

    // 0x44075C: push 0x440970 (handler, deferred argument 3);
    // 0x440761: push $2 (argument 2, literal event id); 0x440764: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c164), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440970U)));

    // 0x44076C: push 0x4409E0 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x44077F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c164),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4409E0U)));

    // 0x440787: push 0x4409F0 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x440799: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c164),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4409F0U)));

    // 0x4407A1: push 0x440A40 (handler, deferred argument 3); the Sys_GetGlobal32_4F1210 getter supplies argument 2;
    // 0x4407B4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c164),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440A40U)));

    // 0x4407BC: push 0x440B20 (handler, deferred argument 3); the Sys_GetGlobal32_4F1220 getter supplies argument 2;
    // 0x4407CF: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c164),
        static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440B20U)));


    // 0x004407D7: push 0x2 (Script_TaskContainer_Grow's SECOND argument, the count - pushed BEFORE the getter call and deliberately left
    // outstanding across it); 0x004407D9: call 0x443E90 (argument 1, the container pointer, arrives in eax); 0x004407DE: push eax (Grow's FIRST
    // argument); 0x004407DF: call 0x4F1980; 0x004407E4: add esp,0x8; 0x004407E7: mov [0x57C168],eax.
    // The literal two is pushed first here only because retail reuses the getter's inert argument slot; the port states both arguments directly.
    ::g_57c168 = reinterpret_cast<std::uint32_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_443E90())), 2)); // 0x004407D7: push 0x2

    // 0x4407EC: push $0x0 (arg6); 0x4407EE: push $0x7 (arg5); 0x4407F0: call 0x4F0FC0 (arg4);
    // 0x4407FC: lea 0x2B8(%esp),%eax resolves to frame+0x2AC (arg3, three pushes outstanding); 0x440803: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x44080A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c168)),
                                append_frame, append_frame + 0x2AC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x440812: push $0x0 (arg6); 0x440814: push $0x7 (arg5); 0x440816: call 0x4F0FC0 (arg4);
    // 0x440822: lea 0x308(%esp),%eax resolves to frame+0x2FC (arg3, three pushes outstanding); 0x440829: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x440830: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c168)),
                                append_frame, append_frame + 0x2FC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);


    // 0x00440835: add esp,0x368 - the 0x350 frame plus the second append's six outstanding pushes; 0x0044083B: ret. There is no
    // 0x004B8810 script-name registration in this body.
} // end of Task_DefineAlarmControl

} // extern "C"
