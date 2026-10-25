// Native: Task_DefineEnvironmentSound (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00436B10-0x00436C51.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 322 bytes, 1 block, 102 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 102 instructions / 322 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00436B10-0x00436C51; the ret is at 0x00436C51 and 0x00436C60 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C0A4 for the name literal (byte-identical to "EnvironmentSound");
//   `px 4` at 0x0057BDA8 and 0x0057BDB0 (both image value 0); 0x004018F0 re-read at its own VA (`mov ax,[0x54F928]; ret`, image 0) for the
//   parent. A push-by-push simulation of the whole body: `sub esp,0x5C`, every push, every `add esp` in order, ending with the final
//   `add esp,0x74` = 0x5C frame + the last append's six pushes (0x18), and every `lea reg,[esp+N]` resolved to frame+0x00 for the second argument
//   and to the recorded delta for the third. The five deltas 0x20, 0x21, 0x22, 0x23 and 0x24 were generated mechanically with assertions and then
//   re-checked against retail; they are CONSECUTIVE, which is why the push order of one append had to be read from its own instructions: appends
//   1, 2, 3 and 5 compute the third argument's address BEFORE pushing the fourth argument's value, while append 4 pushes the fourth argument
//   first, so the same textual `lea 0x2f(%esp)` would be a different frame address if the push order were generalised across sites.
// Behavior/CFG: one straight-line body. It allocates a 0x5C-byte frame with `sub esp,0x5C` (no stack probe), registers the "EnvironmentSound"
//   task type of size 0x5C with parent QTask_GetCurrentType() -> id at 0x0057BDB0, links five handlers - e0->0x00436C60 through 0x00401400,
//   e1->0x00436D10 and e2->0x004D3710 through 0x00401530, 0x004F1A60->0x004374C0 and 0x004F1A70->0x004374D0 through 0x00401400 - then
//   allocates a FIVE-entry parameter container through Script_TaskContainer_Alloc(5) into 0x0057BDA8 and appends five records through
//   0x004F1A80 with recorded deltas 0x20, 0x21, 0x22, 0x23 and 0x24, using Camera_Get_4F1000 for the first four and
//   Core_GetRenderWordB_4F0FD0 for the fifth. One basic block, no branch, no loop, 20 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Both globals hold image value 0, so they are zero-initialised data (`B`); the id slot 0x0057BDB0
//   is 16-bit and the container slot 0x0057BDA8 is a dword. The container is allocated with count 5 and there are exactly 5 appends. Arg6 of
//   0x004F1A80 is 0 at all five sites here (checked per site). **This body has NO tail registration call at all**: it does not call 0x004B8810 and
//   it does not call 0x00416930, so nothing registers a script-visible name in this routine and the task type is reachable only through its id -
//   recorded as an observation, not filled in. 0x004D3710 (`QTask_NoOpHandler`) is used as the e2 handler, pushed as a raw immediate like every
//   other handler here. **The five deltas are single bytes apart** (0x20..0x24), so these fifth/sixth fields are byte-sized neighbours in the
//   record, unlike the 0x4330F0/0x436780 bodies whose deltas are spaced by dword and float groups; a future reader must not assume the record
//   layout of one registrar transfers to another. Deferred pushes: at all five link sites the handler immediate is pushed before the getter call
//   and consumed as the link's third argument. Handler immediates 0x00436C60, 0x00436D10, 0x004374C0 and 0x004374D0 are raw code immediates with
//   no implementation in the tree yet; 0x004D3710 is the tree's no-op handler. Documented shape differences, all value-preserving: (1) every getter
//   callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so
//   those inert pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit
//   `add esp` arithmetic; the retail sizes 0x38, 0xC, 0x18, 0x10 and 0x74 are recorded so a future reviewer can check the arithmetic.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057BDB0: the "EnvironmentSound" task type id. Image value 0, so it is zero-initialised data (`B`); all accesses are 16-bit.
std::uint16_t g_57bdb0 = 0U;

// 0x0057BDA8: the five-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_57bda8 = 0U;

void Task_DefineEnvironmentSound()
{
    // 0x00436B10: sub esp,0x5C - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00436B13: push 0 (TaskType_Register's fourth argument, pushed first); 0x00436B15: push 0 (the parent getter's inert argument);
    // 0x00436B17: call 0x4018F0; 0x00436B1C: push eax; 0x00436B1D: push 0x5C; 0x00436B1F: push "EnvironmentSound"; 0x00436B24: call 0x401900.
    const std::uint16_t environment_sound_id = TaskType_Register("EnvironmentSound",     // 0x00436B1F: push 0x0053C0A4
                                                                 0x5CU,                  // 0x00436B1D: push 0x5C (object size, equal to the frame)
                                                                 QTask_GetCurrentType(), // 0x00436B17: call 0x4018F0 (parent type)
                                                                 0U);                    // 0x00436B13: push 0 (fourth argument)
    ::g_57bdb0 = environment_sound_id; // 0x00436B31: mov word [0x57BDB0],ax - between the pushes of the first link

    // 0x436B29: push 0x00436C60 (handler, deferred argument 3); 0x436B2E: push 0x0 (argument 2); 0x436B37: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bdb0), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436C60U)));

    // 0x436B42: push 0x00436D10 (handler, deferred argument 3); 0x436B47: push 0x1 (argument 2); 0x436B4A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bdb0), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00436D10U)));

    // 0x436B56: push 0x004D3710 (handler, deferred argument 3); 0x436B5B: push 0x2 (argument 2); 0x436B5E: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bdb0), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004D3710U)));

    // 0x436B66: push 0x004374C0 (handler, deferred argument 3); 0x436B6B: call 0x4F1A60 -> argument 2; 0x436B79: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bdb0), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004374C0U)));

    // 0x436B81: push 0x004374D0 (handler, deferred argument 3); 0x436B86: call 0x4F1A70 -> argument 2; 0x436B93: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bdb0), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004374D0U)));

    // 0x00436B98: push 5 (the container entry count, exactly the number of appends below); 0x00436B9A: call 0x4F1950;
    // 0x00436B9F: add esp,0x10 - the last link's three pushes (0xC) plus this count push; 0x00436BA2: mov [0x57BDA8],eax.
    ::g_57bda8 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(5)); // 0x00436B98: push 5

    // 0x436BA7: push $0x0 (arg6); 0x436BAB: call Camera_Get_4F1000 (arg4); 0x436BC1: call 0x004F1A80 (frame+0x00, frame+0x20).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bda8)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x436BC9: push $0x0 (arg6); 0x436BCD: call Camera_Get_4F1000 (arg4); 0x436BE3: call 0x004F1A80 (frame+0x00, frame+0x21).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bda8)),
                                append_frame, append_frame + 0x21,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x436BEB: push $0x0 (arg6); 0x436BEF: call Camera_Get_4F1000 (arg4); 0x436C05: call 0x004F1A80 (frame+0x00, frame+0x22).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bda8)),
                                append_frame, append_frame + 0x22,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x436C0D: push $0x0 (arg6); 0x436C11: call Camera_Get_4F1000 (arg4); 0x436C27: call 0x004F1A80 (frame+0x00, frame+0x23).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bda8)),
                                append_frame, append_frame + 0x23,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x436C2F: push $0x0 (arg6); 0x436C33: call Core_GetRenderWordB_4F0FD0 (arg4); 0x436C49: call 0x004F1A80 (frame+0x00, frame+0x24).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bda8)),
                                append_frame, append_frame + 0x24,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
    // 0x00436C4E: add esp,0x74 - the 0x5C frame plus the last append's six pushes (0x18). This body has NO tail call after the appends.
} // end of Task_DefineEnvironmentSound

} // extern "C"
