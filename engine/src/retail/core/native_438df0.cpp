// Native: Task_DefineRotatingObject (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00438DF0-0x0043908E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 671 bytes, 1 block, 200 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 200 instructions / 671 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent (0x00438DF0-0x0043908E; the ret is at 0x0043908E and 0x00439090 is the next function,
//   which this body uses as its second handler); `ps` at 0x0053C1F4 for the name literal (byte-identical to "RotatingObject"); `px 4` at
//   0x0057BE90 and 0x0057BE98 (both image value 0). A push-by-push simulation of the whole body: `sub esp,0x200`, every push and every `add esp` in
//   order, closing exactly with the final `add esp` after the last append; every `lea reg,[esp+N]` was resolved to frame+0x00 for the second
//   argument and to the recorded delta for the third. The seven link sites and the twelve append deltas were generated mechanically with
//   assertions and then re-checked against retail. Every callee was resolved by sweeping its VA against the tree's existing transcriptions before
//   being declared, and each was re-read at its own VA; all of them are defined inside `extern "C" {` blocks in their own files, so these
//   `extern "C"` declarations match their linkage.
// Behavior/CFG: one straight-line body. It allocates a 0x200-byte frame with `sub esp,0x200` (no stack probe) and registers the "RotatingObject"
//   task type of size 0x200 with parent Sys_GetGlobal16_4C48C0() -> id at 0x0057BE98, which it stores before linking. It links SEVEN handlers:
//   e0->0x00438C30 through 0x00401400, e1->0x00439090 and e2->0x00438DA0 through 0x00401530, then 0x004F1A70->0x004395D0,
//   0x00416840->0x00439750, 0x004E6610->0x00439770 and 0x004F1A60->0x00439790 through 0x00401400. It then allocates a TWELVE-entry parameter
//   container through Script_TaskContainer_Alloc(0xC) into 0x0057BE90 and appends twelve records through 0x004F1A80 with recorded deltas 0x188,
//   0x160, 0x110, 0xF0, 0x120, 0x124, 0x128, 0x12C, 0x130, 0x134, 0x138 and 0x1AC. One basic block, no branch, no loop, 38 call sites over
//   16 distinct callees. There is NO 0x004B8810 script-name registration in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Both globals hold image value 0, so they are zero-initialised data (`B`); the id slot 0x0057BE98
//   is 16-bit and the container slot 0x0057BE90 is a dword. The container is allocated with count 0xC = 12 and there are exactly 12 appends.
//   **Six consecutive appends use the same getter (Sys_GetGlobal32_4F0F30) and their deltas are consecutive dwords** (0x120, 0x124, 0x128,
//   0x12C, 0x130, 0x134), so these are six neighbouring dword fields of the record rather than six copies of one field; the remaining deltas are
//   spaced by other amounts (0x188, 0x160, 0x110, 0xF0, 0x138, 0x1AC), which is why each site's delta was resolved individually instead of being
//   extrapolated from the run. Arg6 of 0x004F1A80 is 0 at all twelve sites here (checked per site). Deferred pushes: at every link site the
//   handler immediate is pushed before the event-id getter call and consumed as the link's third argument. Handler immediates 0x00438C30,
//   0x00439090, 0x00438DA0, 0x004395D0, 0x00439750, 0x00439770 and 0x00439790 are raw code immediates with no implementation in the tree yet.
//   Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the
//   caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced
//   implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (three times), 0x10 and
//   the final append cleanup are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port,
//   which cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the
//   phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: `mov ax,[0x54B...]; ret` (parent type; sys/native_4c48c0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_4E6610(void);       // 0x004E6610: argument-less byte getter (core/native_004e6610.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F30(void);            // 0x004F0F30: `mov eax,[0xA70CA4]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057BE98: the "RotatingObject" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57be98 = 0U;

// 0x0057BE90: the twelve-entry parameter container. Image value 0, so it is zero-initialised data (`B`).
std::uint32_t g_57be90 = 0U;

void Task_DefineRotatingObject()
{
    // 0x00438DF0: sub esp,0x200 - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00438DF6: push 0 (TaskType_Register's fourth argument, pushed first); 0x00438DF8: push 0 (the parent getter's inert argument);
    // 0x00438DFA: call 0x4C48C0; 0x00438DFF: push eax; 0x00438E00: push 0x200; 0x00438E05: push "RotatingObject"; 0x00438E0A: call 0x401900.
    const std::uint16_t rotating_object_id = TaskType_Register("RotatingObject",   // 0x00438E05: push 0x0053C1F4
                                                               0x200U,              // 0x00438E00: push 0x200 (object size, equal to the frame)
                                                               Sys_GetGlobal16_4C48C0(), // 0x00438DFA: call 0x4C48C0 (parent type)
                                                               0U);                 // 0x00438DF8: push 0 (fourth argument)
    ::g_57be98 = rotating_object_id; // 0x00438E17: mov word [0x57BE98],ax - between the pushes of the first link

    // 0x438E0F: push 0x00438C30 (handler, deferred argument 3); 0x438E14: push 0x0 (argument 2); 0x438E1D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be98), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438C30U)));

    // 0x438E28: push 0x00439090 (handler, deferred argument 3); 0x438E2D: push 0x1 (argument 2); 0x438E30: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be98), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439090U)));

    // 0x438E3C: push 0x00438DA0 (handler, deferred argument 3); 0x438E41: push 0x2 (argument 2); 0x438E44: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be98), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438DA0U)));

    // 0x438E4C: push 0x004395D0 (handler, deferred argument 3); 0x438E51: call 0x4F1A70 -> argument 2; 0x438E5F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be98), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004395D0U)));

    // 0x438E67: push 0x00439750 (handler, deferred argument 3); 0x438E6C: call 0x416840 -> argument 2; 0x438E79: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be98), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439750U)));

    // 0x438E81: push 0x00439770 (handler, deferred argument 3); 0x438E86: call 0x4E6610 -> argument 2; 0x438E94: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be98), static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439770U)));

    // 0x438E9C: push 0x00439790 (handler, deferred argument 3); 0x438EA1: call 0x4F1A60 -> argument 2; 0x438EAF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be98), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439790U)));

    // 0x00438EB4: push 0xC (the container entry count, exactly the number of appends below); 0x00438EB6: call 0x4F1950;
    // 0x00438EBB: add esp,0x10 - the last link's three pushes (0xC) plus this count push; 0x00438EBE: mov [0x57BE90],eax.
    ::g_57be90 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xC)); // 0x00438EB4: push 0xC

    // 0x438EC3: push $0x0 (arg6); 0x438EC7: call Sys_GetGlobal32_4F1010 (arg4); 0x438EE1: call 0x004F1A80 (frame+0x00, frame+0x188).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x188,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x438EE9: push $0x0 (arg6); 0x438EED: call Sys_GetGlobal32_4F0F70 (arg4); 0x438F07: call 0x004F1A80 (frame+0x00, frame+0x160).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x160,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x438F0F: push $0x0 (arg6); 0x438F13: call Core_GetRenderWordB_4F0FD0 (arg4); 0x438F2D: call 0x004F1A80 (frame+0x00, frame+0x110).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x110,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x438F35: push $0x0 (arg6); 0x438F39: call Core_GetRenderWordB_4F0FD0 (arg4); 0x438F53: call 0x004F1A80 (frame+0x00, frame+0xF0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x438F5B: push $0x0 (arg6); 0x438F5F: call Sys_GetGlobal32_4F0F30 (arg4); 0x438F79: call 0x004F1A80 (frame+0x00, frame+0x120).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x120,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x438F81: push $0x0 (arg6); 0x438F85: call Sys_GetGlobal32_4F0F30 (arg4); 0x438F9F: call 0x004F1A80 (frame+0x00, frame+0x124).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x124,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x438FA7: push $0x0 (arg6); 0x438FAB: call Sys_GetGlobal32_4F0F30 (arg4); 0x438FC5: call 0x004F1A80 (frame+0x00, frame+0x128).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x128,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x438FCD: push $0x0 (arg6); 0x438FD1: call Sys_GetGlobal32_4F0F30 (arg4); 0x438FEB: call 0x004F1A80 (frame+0x00, frame+0x12C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x12C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x438FF3: push $0x0 (arg6); 0x438FF7: call Sys_GetGlobal32_4F0F30 (arg4); 0x439011: call 0x004F1A80 (frame+0x00, frame+0x130).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x130,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x439019: push $0x0 (arg6); 0x43901D: call Sys_GetGlobal32_4F0F30 (arg4); 0x439037: call 0x004F1A80 (frame+0x00, frame+0x134).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x134,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x43903F: push $0x0 (arg6); 0x439043: call Camera_Get_4F1000 (arg4); 0x43905D: call 0x004F1A80 (frame+0x00, frame+0x138).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x138,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x439065: push $0x0 (arg6); 0x439069: call Sys_GetGlobal32_4F0FC0 (arg4); 0x439083: call 0x004F1A80 (frame+0x00, frame+0x1AC).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be90)),
                                append_frame, append_frame + 0x1AC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
} // end of Task_DefineRotatingObject

} // extern "C"
