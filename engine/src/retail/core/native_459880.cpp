// Native: Task_DefineHumanSoldierPistol (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00459880-0x00459A11.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 402 bytes (0x192), 1 block, 0 edges, 109
//   instructions, no branch and no loop; the extent is closed by the `ret` at 0x00459A11, followed by the fourteen-byte nop pad 0x00459A12-0x00459A1F
//   and the next function (the event-0 handler 0x00459A20).
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - the single block is 0x00459880 + 402 bytes with no successors -
//   and the objdump listing build/retail-dis.txt was read over the whole extent, giving 26 call sites over 16 distinct callees, the same census shape
//   as the sibling registrars 0x00454970 (core/native_454970.cpp) and 0x00456AD0 (core/native_456ad0.cpp). Every call site was decoded against its
//   callee's own transcribed signature. Image reads: 0x0053EBF8 holds "HumanSoldierPistol"; 0x005BDAA4, 0x005BDAA8 and 0x005BDAB0 lie past
//   `.data`'s raw size, so their image value is the zero fill and the port's initialisers are 0 (this image has no `.bss`; `.rdata` ends at 0x005358F6
//   and `.data` starts at 0x00536000).
// Behavior/CFG: one straight-line body with a 0xD50-byte frame (`sub esp,0xd50`, no saved-register pushes, and no stack probe because 3408 bytes is
//   below the 4 KB probe threshold - so there is no probe call site to transcribe). In retail order it registers the "HumanSoldierPistol" type of
//   object size 0xD50 - the same 0xD50 as the frame - with the parent type Core_GetGlobal16_460BE0() returns, storing the returned id at 0x005BDAA8;
//   sets the inherited flags to 1 through 0x00401FA0; links handler 0x00459A20 for event 0 through 0x00401400 and handlers 0x00454EE0 and 0x00459EC0
//   for events 1 and 2 through 0x00401530; then links seven further 0x00401400 events in retail order - 0x00459F00 on the event from 0x004F1A60,
//   0x00456E50 on the event from 0x004F1A70, 0x00459F10 on the event from 0x004F40D0, 0x0045A230 on the event from 0x00416840, 0x0045A3D0 on the
//   event from 0x00416860, 0x0045A5E0 on the event from 0x00460C40 and 0x004D3710 on the event from 0x0047E990; grows the container
//   Core_GetGlobal32_460C50() by 2 into 0x005BDAA4; appends two records through 0x004F1A80 whose frame offsets are 0xCF8 and 0xCFC with argument 5 = 7
//   and argument 6 = 0; and finally clears the dword at 0x005BDAB0.
//   Handler sharing, as bytes rather than as similarity: 0x00454EE0 and 0x00459EC0 on events 1 and 2 are the same two handlers 0x00454970 and
//   0x00456AD0 link, and 0x00456E50 on the 0x004F1A70 event is the same handler 0x00456AD0 links on that event - while this body's own 0x004F1A70
//   event uses 0x00456E50 and its 0x004F1A60 event uses 0x00459F00. 0x004D3710 is the most-shared handler in the image (163 reference sites).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 109-instruction listing; runtime UNOBSERVED. Census: 26 call sites / 16 distinct both
//   in retail and in the port object, every target 1:1 - 0x00401400 x8, 0x00401530 x2, 0x00460BE0 x1, 0x00401900 x1, 0x00401FA0 x1, 0x00416840 x1,
//   0x00416860 x1, 0x00460C40 x1, 0x00460C50 x1, 0x0047E990 x1, 0x004F0FA0 x2, 0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x2,
//   0x004F40D0 x1.
//   Cleanup reconciliation: `add esp,0x40` at 0x004598E8 = 16 dwords = the two deferred zeros plus the three registration pushes plus the two of
//   0x00401FA0 plus the three of each of the first three links; seven `add esp,0xc` = the seven remaining 0x00401400 links; `add esp,0x8` at
//   0x004599B2 = the grow's two (the count pushed before the argument-less 0x00460C50 and that call's return); `add esp,0x18` at 0x004599DC = the
//   first append's six; and the final `add esp,0xd68` at 0x00459A0B = 0xD50 frame + 0x18 = the second append's six plus the whole frame. 51 pushes =
//   204 bytes; 204 + 3408 frame = 3612 = 0x40 + 7 * 0xC + 0x8 + 0x18 + 0xD68, and esp is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x40; the port's four-parameter signature emits
//   one zero for the task-list argument; (2) the 0xD50-byte frame is reproduced as one addressable dword whose address plus 0xCF8 and 0xCFC stands in
//   for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither, so the recorded
//   values are record sizes rather than frame pointers; (3) the parent type comes from 0x00460BE0 and not from 0x004018F0, as retail calls it.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDAA8 at thirteen sites, eight
//   in this body (0x004598A4, 0x004598AF, 0x004598C3, 0x004598D7, 0x004598F8, 0x00459912, 0x0045992D, 0x00459948) and the rest in the handlers this
//   body links and in other bodies, which must declare it `extern`; 0x005BDAA4 at 0x004599B6, 0x004599CC and 0x004599F1 (this body), 0x00459F06
//   (inside handler 0x00459F00) and 0x0045A7C1 (another body); 0x005BDAB0 at 0x00459A03 only, so this body owns it outright. The definitions below are
//   owned here because this body writes them first.
//   Handlers 0x00459A20, 0x00454EE0, 0x00459EC0, 0x00459F00, 0x00456E50, 0x00459F10, 0x0045A230, 0x0045A3D0, 0x0045A5E0 and 0x004D3710 are raw code
//   immediates in retail and are not transcribed here. Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {

std::uint16_t Core_GetGlobal16_460BE0(void);                                        // 0x00460BE0: argument-less word getter (parent type)
void TaskType_SetInheritedFlags_401FA0(std::uint32_t type_id, std::uint32_t flags); // 0x00401FA0 (core/native_00401fa0.cpp)
std::uint8_t Script_PhaseIdGetE4_4F1A60(void);                                      // 0x004F1A60: argument-less byte getter
std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                                      // 0x004F1A70: argument-less byte getter
std::uint8_t Sys_GetGlobal8_4F40D0(void);                                           // 0x004F40D0: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416840(void);                                       // 0x00416840: argument-less byte getter
std::uint8_t Sys_GetSettingByte_416860(void);                                       // 0x00416860: argument-less byte getter
std::uint8_t Core_GetGlobal8_460C40(void);                                          // 0x00460C40: argument-less byte getter
std::uint8_t Core_GetGlobal8_47E990(void);                                          // 0x0047E990: argument-less byte getter
std::uint32_t Core_GetGlobal32_460C50(void);                                        // 0x00460C50: argument-less dword getter (the task container)
void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980 (script/native_4f1980.cpp)
void Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                 std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80
std::uint32_t Core_GetRenderWordA_4F0FA0(void);                                     // 0x004F0FA0 (core/native_4f0fa0.cpp)

// 0x005BDAA8: the "HumanSoldierPistol" task type id (word). Zero in the image (past `.data`'s raw size). Thirteen reference sites; the definition is
// owned here and the handlers this body links must declare it `extern`.
std::uint16_t g_5bdaa8 = 0U;

// 0x005BDAA4: the "HumanSoldierPistol" task container this body grows. Zero in the image. Also read by handler 0x00459F00 and by 0x0045A7C1.
std::uint32_t g_5bdaa4 = 0U;

// 0x005BDAB0: cleared by this body and referenced nowhere else in `.text`. Zero in the image.
std::uint32_t g_5bdab0 = 0U;

void Task_DefineHumanSoldierPistol()
{
    // 0x00459880: sub esp,0xd50 - the frame, 3408 bytes, no probe because it is below 4 KB.
    // 0x00459886: push 0; 0x00459888: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. The frame is reproduced as one addressable dword whose address stands in for the frame base, because 0x004F1A80 needs the frame
    // offsets 0xCF8 and 0xCFC and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0045988A: call 0x00460BE0 (parent type); 0x0045988F: push %eax; 0x00459890: push 0xd50; 0x00459895: push 0x0053EBF8; 0x0045989A: call 0x00401900.
    const std::uint16_t soldier_pistol_id = TaskType_Register("HumanSoldierPistol", // 0x00459895: push 0x0053EBF8
                                                              0xD50U,               // 0x00459890: push 0xd50 (object size, equal to the frame)
                                                              Core_GetGlobal16_460BE0(), // 0x0045988A: call 0x00460BE0 (parent type)
                                                              0U);                  // 0x00459888: push 0 (task-list argument)

    // 0x0045989F: push 0x1 (argument 2, the flags); 0x004598A1: push %eax (argument 1, the id); 0x004598A2: mov word [0x005BDAA8],ax;
    // 0x004598A8: call 0x00401FA0. The store precedes the call in retail and is kept before it here.
    ::g_5bdaa8 = soldier_pistol_id; // 0x004598A2
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(soldier_pistol_id), 0x1U);

    // 0x004598B3: push 0x00459A20 (handler); 0x004598B8: push 0x0 (argument 2, literal event id); 0x004598BA: push %eax (argument 1, the id
    // RELOADED from the global); 0x004598BB: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459A20U)));

    // 0x004598C7: push 0x00454EE0 (handler - the same handler 0x00454970 and 0x00456AD0 link on this event); 0x004598CC: push 0x1 (argument 2,
    // literal event id); 0x004598CE: push %ecx (argument 1); 0x004598CF: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdaa8), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454EE0U)));

    // 0x004598DB: push 0x00459EC0 (handler - likewise shared with the other two registrars); 0x004598E0: push 0x2 (argument 2, literal event id);
    // 0x004598E2: push %edx (argument 1); 0x004598E3: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdaa8), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459EC0U)));

    // 0x004598EB: push 0x00459F00 (handler, deferred argument 3); 0x004598F0: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x004598F5: push %eax; 0x004598F6: mov word [0x005BDAA8],%ax; 0x004598FC: push %eax; 0x004598FD: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459F00U)));

    // 0x00459905: push 0x00456E50 (handler - the same handler 0x00456AD0 links on this event); 0x0045990A: call 0x004F1A70 (supplies argument 2,
    // the event id); 0x0045990F: mov word [0x005BDAA8],%cx; 0x00459916: push %eax; 0x00459917: push %ecx; 0x00459918: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456E50U)));

    // 0x00459920: push 0x00459F10 (handler, deferred argument 3); 0x00459925: call 0x004F40D0 (supplies argument 2, the event id);
    // 0x0045992A: mov word [0x005BDAA8],%dx; 0x00459931: push %eax; 0x00459932: push %edx; 0x00459933: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459F10U)));

    // 0x0045993B: push 0x0045A230 (handler, deferred argument 3); 0x00459940: call 0x00416840 (supplies argument 2, the event id);
    // 0x00459945: push %eax; 0x00459946: mov word [0x005BDAA8],%ax; 0x0045994C: push %eax; 0x0045994D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45A230U)));

    // 0x00459955: push 0x0045A3D0 (handler, deferred argument 3); 0x0045995A: call 0x00416860 (supplies argument 2, the event id);
    // 0x0045995F: mov word [0x005BDAA8],%cx; 0x00459966: push %eax; 0x00459967: push %ecx; 0x00459968: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45A3D0U)));

    // 0x00459970: push 0x0045A5E0 (handler, deferred argument 3); 0x00459975: call 0x00460C40 (supplies argument 2, the event id);
    // 0x0045997A: mov word [0x005BDAA8],%dx; 0x00459981: push %eax; 0x00459982: push %edx; 0x00459983: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Core_GetGlobal8_460C40()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45A5E0U)));

    // 0x0045998B: push 0x004D3710 (handler, deferred argument 3); 0x00459990: call 0x0047E990 (supplies argument 2, the event id);
    // 0x00459995: push %eax; 0x00459996: mov word [0x005BDAA8],%ax; 0x0045999C: push %eax; 0x0045999D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdaa8), static_cast<std::uint32_t>(Core_GetGlobal8_47E990()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x004599A5: push 0x2 (inert for the argument-less 0x00460C50 and the grow count for 0x004F1980); 0x004599A7: call 0x00460C50;
    // 0x004599AC: push %eax; 0x004599AD: call 0x004F1980; 0x004599B5: mov [0x005BDAA4],eax.
    const std::uint32_t container = Core_GetGlobal32_460C50(); // 0x004599A7: call 0x00460C50
    ::g_5bdaa4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(container)), 2))); // 0x004599AD: call 0x004F1980

    // 0x004599BA: push 0 (argument 6); 0x004599BC: push 7 (argument 5); 0x004599BE: call 0x004F0FA0 (argument 4); 0x004599C3: lea 0xd00(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCF8 (argument 3); 0x004599CB: mov [0x005BDAA4],%eax (argument 1); 0x004599D0: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x004599D7: call 0x004F1A80; 0x004599DC: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdaa4)),
                                append_frame + 0x00,   // 0x004599D0: argument 2, the frame base
                                append_frame + 0xCF8,  // 0x004599C3: argument 3, frame+0xCF8
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x004599BE: argument 4
                                7,                     // 0x004599BC: argument 5
                                0);                    // 0x004599BA: argument 6

    // 0x004599DF: push 0 (argument 6); 0x004599E1: push 7 (argument 5); 0x004599E3: call 0x004F0FA0 (argument 4); 0x004599E8: lea 0xd04(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCFC (argument 3); 0x004599F0: mov [0x005BDAA4],%eax (argument 1); 0x004599F5: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x004599FC: call 0x004F1A80. Retail does not clean this call's six pushes
    // separately: the final `add esp,0xd68` at 0x00459A0B covers them together with the 0xD50 frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdaa4)),
                                append_frame + 0x00,   // 0x004599F5: argument 2, the frame base
                                append_frame + 0xCFC,  // 0x004599E8: argument 3, frame+0xCFC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x004599E3: argument 4
                                7,                     // 0x004599E1: argument 5
                                0);                    // 0x004599DF: argument 6

    // 0x00459A01: movl $0x0,0x005BDAB0. Then 0x00459A0B: add esp,0xd68 (frame + the six pushes above); 0x00459A11: ret.
    ::g_5bdab0 = 0U;
} // end of Task_DefineHumanSoldierPistol

} // extern "C"
