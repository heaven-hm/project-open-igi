// Native: Task_DefineHumanPlayerInput (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0045DB70-0x0045DCA4.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 309 bytes (0x135), 1 block, 0 edges, 90
//   instructions, no branch and no loop; the extent is closed by the `ret` at 0x0045DCA4, followed by the eleven-byte nop pad 0x0045DCA5-0x0045DCAF
//   and the next function (the event-0 handler 0x0045DCB0).
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - the single block is 0x0045DB70 + 309 bytes with no successors -
//   and the objdump listing build/retail-dis.txt was read over the whole extent, giving 18 call sites over 11 distinct callees. Every call site was
//   decoded against its callee's own transcribed signature. Image reads: 0x0053F2F8 holds "HumanPlayerInput" - the string immediately after the
//   "HumanSoldier" literal at 0x0053F2E0 that 0x0045CAB0 registers; 0x005BDAC4 and 0x005BDAC8 lie past `.data`'s raw size, so their image value is
//   the zero fill and the port's initialisers are 0 (this image has no `.bss`; `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000).
// Behavior/CFG: one straight-line body with a 0x1E4-byte frame (`sub esp,0x1e4`, no saved-register pushes, and no stack probe because 484 bytes is far
//   below the 4 KB probe threshold - so there is no probe call site to transcribe). In retail order it registers the "HumanPlayerInput" type of object
//   size 0x1E4 - the same 0x1E4 as the frame - with the parent type from 0x004018F0 (NOT the 0x00460BE0 the soldier registrars use), storing the
//   returned id at 0x005BDAC4; links handler 0x0045DCB0 for event 0 through 0x00401400 and handlers 0x0045DCF0 and 0x0045DE20 for events 1 and 2
//   through 0x00401530; links 0x0045DF40 on the event from 0x004F1A60 and 0x0045DF50 on the event from 0x004F1A70, both through 0x00401400;
//   allocates the container with capacity 4 through 0x004F1950 into 0x005BDAC8; and appends four records through 0x004F1A80, whose frame offsets are
//   0x80, 0x81, 0x82 and 0x194, with argument 4 taken from 0x004F1000 twice, then 0x004F0FF0, then 0x004F0FC0, and argument 5 = 7 and argument 6 = 0
//   at all four. The four offsets are consistent with the event-0 handler 0x0045DCB0, which clears the three bytes at +0x80, +0x81, +0x82 and the four
//   dwords at +0x184..+0x190 before calling 0x004F1320 with +0x194 - the same fields this body appends.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 90-instruction listing; runtime UNOBSERVED. Census: 18 call sites / 11 distinct both
//   in retail and in the port object, every target 1:1 - 0x00401400 x3, 0x00401530 x2, 0x004018F0 x1, 0x00401900 x1, 0x004F0FC0 x1, 0x004F0FF0 x1,
//   0x004F1000 x2, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x4.
//   Cleanup reconciliation: `add esp,0x38` at 0x0045DBC9 = 14 dwords = the two deferred zeros plus the three registration pushes plus the three of
//   each of the first three links; `add esp,0xc` at 0x0045DBE4 = link 4's three; `add esp,0x10` at 0x0045DC05 = 4 dwords = link 5's three PLUS the
//   capacity push of 0x004F1950, which is why that one add is one dword larger than the others; `add esp,0x18` at 0x0045DC2F, 0x0045DC54 and 0x0045DC79
//   = the first three appends' six each; and the final `add esp,0x1fc` at 0x0045DC9E = 0x1E4 frame + 0x18 = the fourth append's six plus the whole
//   frame. 45 pushes = 180 bytes; 180 + 484 frame = 664 = 0x38 + 0xC + 0x10 + 3 * 0x18 + 0x1FC, and esp is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x38; the port's four-parameter signature emits
//   one zero for the task-list argument; (2) the 0x1E4-byte frame is reproduced as one addressable dword whose address plus 0x80, 0x81, 0x82 and
//   0x194 stands in for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither,
//   so the recorded values are record sizes rather than frame pointers; (3) the parent type comes from 0x004018F0 here, as retail calls it, while the
//   four soldier registrars call 0x00460BE0.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDAC4 at six sites - 0x0045DB99,
//   0x0045DBA4, 0x0045DBB8, 0x0045DBD9 and 0x0045DBF4 (this body) and 0x0045E04E (another body); 0x005BDAC8 at seven sites - 0x0045DC09, 0x0045DC1F,
//   0x0045DC44, 0x0045DC69 and 0x0045DC87 (this body) plus 0x0045DF46 (inside handler 0x0045DF40) and 0x0045E041 (another body). Both definitions are
//   owned here because this body writes them first, and the other bodies must declare them `extern`.
//   Handlers 0x0045DCB0, 0x0045DCF0, 0x0045DE20, 0x0045DF40 and 0x0045DF50 are raw code immediates in retail and are not transcribed here. Domain:
//   core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {

std::uint8_t Script_PhaseIdGetE4_4F1A60(void);                                      // 0x004F1A60: argument-less byte getter
std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                                      // 0x004F1A70: argument-less byte getter
void* Script_TaskContainer_Alloc(std::int32_t const count);                         // 0x004F1950 (script/native_4f1950.cpp)
void Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                 std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80
std::uint32_t Camera_Get_4F1000(void);                                              // 0x004F1000 (render/native_4f1000.cpp)
std::uint32_t Sys_GetGlobal32_4F0FF0(void);                                         // 0x004F0FF0 (sys/native_4f0ff0.cpp)
std::uint32_t Sys_GetGlobal32_4F0FC0(void);                                         // 0x004F0FC0 (sys/native_4f0fc0.cpp)

// 0x005BDAC4: the "HumanPlayerInput" task type id (word). Zero in the image (past `.data`'s raw size). Six reference sites; the definition is owned
// here and the other bodies that read it must declare it `extern`.
std::uint16_t g_5bdac4 = 0U;

// 0x005BDAC8: the "HumanPlayerInput" task container this body allocates with capacity 4. Zero in the image. Also read by handler 0x0045DF40 and by
// 0x0045E041.
std::uint32_t g_5bdac8 = 0U;

void Task_DefineHumanPlayerInput()
{
    // 0x0045DB70: sub esp,0x1e4 - the frame, 484 bytes, no probe because it is far below 4 KB.
    // 0x0045DB76: push 0; 0x0045DB78: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. The frame is reproduced as one addressable dword whose address stands in for the frame base, because 0x004F1A80 needs the frame
    // offsets 0x80, 0x81, 0x82 and 0x194 and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0045DB7A: call 0x004018F0 (parent type); 0x0045DB7F: push %eax; 0x0045DB80: push 0x1e4; 0x0045DB85: push 0x0053F2F8; 0x0045DB8A: call 0x00401900.
    const std::uint16_t player_input_id = TaskType_Register("HumanPlayerInput", // 0x0045DB85: push 0x0053F2F8
                                                            0x1E4U,             // 0x0045DB80: push 0x1e4 (object size, equal to the frame)
                                                            QTask_GetCurrentType(), // 0x0045DB7A: call 0x004018F0 (parent type)
                                                            0U);                // 0x0045DB78: push 0 (task-list argument)

    // 0x0045DB8F: push 0x0045DCB0 (handler); 0x0045DB94: push 0x0 (argument 2, literal event id); 0x0045DB96: push %eax (argument 1, the id);
    // 0x0045DB97: mov word [0x005BDAC4],ax; 0x0045DB9D: call 0x00401400. The store precedes the link call in retail and is kept before it here.
    ::g_5bdac4 = player_input_id; // 0x0045DB97
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(player_input_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45DCB0U)));

    // 0x0045DBA2: mov word [0x005BDAC4],%ax; 0x0045DBA8: push 0x0045DCF0 (handler); 0x0045DBAD: push 0x1 (argument 2, literal event id);
    // 0x0045DBAF: push %eax (argument 1, the id RELOADED from the global); 0x0045DBB0: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdac4), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45DCF0U)));

    // 0x0045DBB5: mov word [0x005BDAC4],%cx; 0x0045DBBC: push 0x0045DE20 (handler); 0x0045DBC1: push 0x2 (argument 2, literal event id);
    // 0x0045DBC3: push %ecx (argument 1); 0x0045DBC4: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdac4), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45DE20U)));

    // 0x0045DBCC: push 0x0045DF40 (handler, deferred argument 3); 0x0045DBD1: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x0045DBD6: mov word [0x005BDAC4],%dx; 0x0045DBDD: push %eax; 0x0045DBDE: push %edx; 0x0045DBDF: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdac4), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45DF40U)));

    // 0x0045DBE7: push 0x0045DF50 (handler, deferred argument 3); 0x0045DBEC: call 0x004F1A70 (supplies argument 2, the event id);
    // 0x0045DBF1: push %eax; 0x0045DBF2: mov word [0x005BDAC4],%ax; 0x0045DBF8: push %eax; 0x0045DBF9: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdac4), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45DF50U)));

    // 0x0045DBFE: push 0x4 (the capacity, argument 1 of 0x004F1950); 0x0045DC00: call 0x004F1950; 0x0045DC05: add esp,0x10 - link 5's three
    // pushes and this capacity push; 0x0045DC08: mov [0x005BDAC8],eax.
    ::g_5bdac8 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(4))); // 0x0045DC00: call 0x004F1950

    // 0x0045DC0D: push 0 (argument 6); 0x0045DC0F: push 7 (argument 5); 0x0045DC11: call 0x004F1000 (argument 4); 0x0045DC16: lea 0x88(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0x80 (argument 3); 0x0045DC1E: mov [0x005BDAC8],%eax (argument 1); 0x0045DC23: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045DC2A: call 0x004F1A80; 0x0045DC2F: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdac8)),
                                append_frame + 0x00,   // 0x0045DC23: argument 2, the frame base
                                append_frame + 0x80,   // 0x0045DC16: argument 3, frame+0x80
                                static_cast<std::int32_t>(Camera_Get_4F1000()), // 0x0045DC11: argument 4
                                7,                     // 0x0045DC0F: argument 5
                                0);                    // 0x0045DC0D: argument 6

    // 0x0045DC32: push 0 (argument 6); 0x0045DC34: push 7 (argument 5); 0x0045DC36: call 0x004F1000 (argument 4); 0x0045DC3B: lea 0x89(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0x81 (argument 3); 0x0045DC43: mov [0x005BDAC8],%eax (argument 1); 0x0045DC48: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045DC4F: call 0x004F1A80; 0x0045DC54: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdac8)),
                                append_frame + 0x00,   // 0x0045DC48: argument 2, the frame base
                                append_frame + 0x81,   // 0x0045DC3B: argument 3, frame+0x81
                                static_cast<std::int32_t>(Camera_Get_4F1000()), // 0x0045DC36: argument 4
                                7,                     // 0x0045DC34: argument 5
                                0);                    // 0x0045DC32: argument 6

    // 0x0045DC57: push 0 (argument 6); 0x0045DC59: push 7 (argument 5); 0x0045DC5B: call 0x004F0FF0 (argument 4); 0x0045DC60: lea 0x8a(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0x82 (argument 3); 0x0045DC68: mov [0x005BDAC8],%eax (argument 1); 0x0045DC6D: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045DC74: call 0x004F1A80; 0x0045DC79: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdac8)),
                                append_frame + 0x00,   // 0x0045DC6D: argument 2, the frame base
                                append_frame + 0x82,   // 0x0045DC60: argument 3, frame+0x82
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), // 0x0045DC5B: argument 4
                                7,                     // 0x0045DC59: argument 5
                                0);                    // 0x0045DC57: argument 6

    // 0x0045DC7C: push 0 (argument 6); 0x0045DC7E: push 7 (argument 5); 0x0045DC80: call 0x004F0FC0 (argument 4); 0x0045DC85: push %eax;
    // 0x0045DC86: mov [0x005BDAC8],%eax (argument 1); 0x0045DC8B: lea 0x1a0(%esp),%ecx - with esp at frame-12 there, that is frame+0x194
    // (argument 3); 0x0045DC92: lea 0xc(%esp),%edx - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045DC99: call 0x004F1A80.
    // Retail does not clean this call's six pushes separately: the final `add esp,0x1fc` at 0x0045DC9E covers them together with the 0x1E4 frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdac8)),
                                append_frame + 0x00,   // 0x0045DC92: argument 2, the frame base
                                append_frame + 0x194,  // 0x0045DC8B: argument 3, frame+0x194
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), // 0x0045DC80: argument 4
                                7,                     // 0x0045DC7E: argument 5
                                0);                    // 0x0045DC7C: argument 6

    // 0x0045DC9E: add esp,0x1fc (frame + the six pushes above); 0x0045DCA4: ret.
} // end of Task_DefineHumanPlayerInput

} // extern "C"
