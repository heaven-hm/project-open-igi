// Native: Task_DefineHumanSoldierFemale (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00454970-0x00454B01.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 402 bytes (0x192), 1 block, 0 edges, 109
//   instructions, no branch and no loop; the extent is closed by the `ret` at 0x00454B01, followed by the fourteen-byte nop pad 0x00454B02-0x00454B0F
//   and the next function at 0x00454B10.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - the single block is 0x00454970 + 402 bytes with no
//   successors, and the objdump listing build/retail-dis.txt was read over the whole extent, giving 26 call sites over 16 distinct callees. Every
//   call site was decoded against its callee's own transcribed signature, and this body is the sibling of the already-transcribed registrar
//   0x0040FE30 (core/native_40fe30.cpp), which uses the same callee set in the same shapes. Image reads: 0x0053E64C holds 48 75 6d 61 6e 53 6f 6c 64
//   69 65 72 46 65 6d 61 6c 65 00 00 = "HumanSoldierFemale"; 0x005BDA84, 0x005BDA88 and 0x005BDA90 lie past `.data`'s raw size, so their image
//   value is the zero fill and the port's initialisers are 0 (this image has no `.bss`; `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000).
// Behavior/CFG: one straight-line body with a 0xD50-byte frame (`sub esp,0xd50`, no saved-register pushes, and no stack probe because 3408 bytes is
//   below the 4 KB probe threshold - so there is no probe call site to transcribe). In retail order it registers the "HumanSoldierFemale" type of
//   object size 0xD50 - the same 0xD50 as the frame - with the parent type Core_GetGlobal16_460BE0() returns, storing the returned id at 0x005BDA88;
//   sets the inherited flags to 1 through 0x00401FA0; links handler 0x00454B10 for event 0 through 0x00401400 and handlers 0x00454EE0 and 0x00459EC0
//   for events 1 and 2 through 0x00401530; then links seven further 0x00401400 events in retail order - 0x00454F00 on the event from 0x004F1A60,
//   0x00454F10 on the event from 0x004F1A70, 0x00455070 on the event from 0x004F40D0, 0x004553E0 on the event from 0x00416840, 0x00455570 on the
//   event from 0x00416860, 0x00455780 on the event from 0x00460C40 and 0x004D3710 on the event from 0x0047E990; grows the container
//   Core_GetGlobal32_460C50() by 2 into 0x005BDA84; appends two records through 0x004F1A80 whose frame offsets are 0xCF8 and 0xCFC with argument 5 = 7
//   and argument 6 = 0; and finally clears the dword at 0x005BDA90.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 109-instruction listing; runtime UNOBSERVED. Census: 26 call sites / 16 distinct
//   both in retail and in the port object, every target 1:1 - 0x00401400 x8, 0x00401530 x2, 0x004018F0-equivalent 0x00460BE0 x1, 0x00401900 x1,
//   0x00401FA0 x1, 0x00416840 x1, 0x00416860 x1, 0x00460C40 x1, 0x00460C50 x1, 0x0047E990 x1, 0x004F0FA0 x2, 0x004F1980 x1, 0x004F1A60 x1,
//   0x004F1A70 x1, 0x004F1A80 x2, 0x004F40D0 x1.
//   Cleanup reconciliation: `add esp,0x40` at 0x004549D8 = 16 dwords = the two deferred zeros plus the three registration pushes plus the two of
//   0x00401FA0 plus the three of each of the first three links; seven `add esp,0xc` = the seven remaining 0x00401400 links; `add esp,0x8` at
//   0x00454AA2 = the grow's two (the count pushed before the argument-less 0x00460C50 and that call's return); `add esp,0x18` at 0x00454ACC = the
//   first append's six; and the final `add esp,0xd68` at 0x00454AFB = 0xD50 frame + 0x18 = the second append's six plus the whole frame. 51 pushes =
//   204 bytes; 204 + 3408 frame = 3612 = 0x40 + 7 * 0xC + 0x8 + 0x18 + 0xD68, and esp is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x40; the port's four-parameter signature emits
//   one zero for the task-list argument; (2) the 0xD50-byte frame is reproduced as one addressable dword whose address plus 0xCF8 and 0xCFC stands in
//   for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither, so the recorded
//   values are record sizes rather than frame pointers; (3) the parent type here comes from 0x00460BE0 and not from 0x004018F0, which the other
//   registrars in this lane use - the call is transcribed at the address retail calls.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDA84 is referenced at 0x00454AA6
//   (this body's store), 0x00454ABC and 0x00454AE1 (this body's two append container loads), 0x00454F06 (inside handler 0x00454F00) and 0x00455961
//   (inside another body), so those must declare it `extern`; 0x005BDA88 at thirteen sites, eight of them in this body (0x00454994, 0x0045499F,
//   0x004549B3, 0x004549C7, 0x004549E8, 0x00454A02, 0x00454A1D, 0x00454A38) and the rest in the handlers this body links and in other bodies;
//   0x005BDA90 at 0x00454AF3 only, so this body owns it outright. The definitions below are owned here because this body writes them first.
//   Handlers 0x00454B10, 0x00454EE0, 0x00459EC0, 0x00454F00, 0x00454F10, 0x00455070, 0x004553E0, 0x00455570, 0x00455780 and 0x004D3710 are raw code
//   immediates in retail and are not transcribed here; 0x004D3710 is the most-shared handler in the image (163 reference sites). Domain: core/,
//   because it is one of the 0x00402930 startup registrars.
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

// 0x005BDA84: the "HumanSoldierFemale" task container this body grows. Zero in the image (past `.data`'s raw size). Also read by handler 0x00454F00
// and by 0x00455961, which must declare it `extern`.
std::uint32_t g_5bda84 = 0U;

// 0x005BDA88: the "HumanSoldierFemale" task type id (word). Zero in the image - unlike the other registrars in this lane, whose id slots hold the
// 0x0181 unregistered sentinel in the image, this one is a zero-fill address. Thirteen reference sites; the definitions are owned here.
std::uint16_t g_5bda88 = 0U;

// 0x005BDA90: cleared by this body and referenced nowhere else in `.text`. Zero in the image.
std::uint32_t g_5bda90 = 0U;

void Task_DefineHumanSoldierFemale()
{
    // 0x00454970: sub esp,0xd50 - the frame, 3408 bytes, no probe because it is below 4 KB.
    // 0x00454976: push 0; 0x00454978: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. The frame is reproduced as one addressable dword whose address stands in for the frame base, because 0x004F1A80 needs the frame
    // offsets 0xCF8 and 0xCFC and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0045497A: call 0x00460BE0 (parent type); 0x0045497F: push %eax; 0x00454980: push 0xd50; 0x00454985: push 0x0053E64C; 0x0045498A: call 0x00401900.
    const std::uint16_t soldier_female_id = TaskType_Register("HumanSoldierFemale", // 0x00454985: push 0x0053E64C
                                                             0xD50U,                // 0x00454980: push 0xd50 (object size, equal to the frame)
                                                             Core_GetGlobal16_460BE0(), // 0x0045497A: call 0x00460BE0 (parent type)
                                                             0U);                   // 0x00454978: push 0 (task-list argument)

    // 0x0045498F: push 0x1 (argument 2, the flags); 0x00454991: push %eax (argument 1, the id); 0x00454992: mov word [0x005BDA88],ax;
    // 0x00454998: call 0x00401FA0. The store precedes the call in retail and is kept before it here.
    ::g_5bda88 = soldier_female_id; // 0x00454992
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(soldier_female_id), 0x1U);

    // 0x004549A3: push 0x00454B10 (handler); 0x004549A8: push 0x0 (argument 2, literal event id); 0x004549AA: push %eax (argument 1, the id
    // RELOADED from the global); 0x004549AB: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454B10U)));

    // 0x004549B7: push 0x00454EE0 (handler); 0x004549BC: push 0x1 (argument 2, literal event id); 0x004549BE: push %ecx (argument 1);
    // 0x004549BF: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bda88), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454EE0U)));

    // 0x004549CB: push 0x00459EC0 (handler); 0x004549D0: push 0x2 (argument 2, literal event id); 0x004549D2: push %edx (argument 1);
    // 0x004549D3: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bda88), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459EC0U)));

    // 0x004549DB: push 0x00454F00 (handler, deferred argument 3); 0x004549E0: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x004549E5: push %eax; 0x004549E6: mov word [0x005BDA88],%ax; 0x004549EC: push %eax; 0x004549ED: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454F00U)));

    // 0x004549F5: push 0x00454F10 (handler, deferred argument 3); 0x004549FA: call 0x004F1A70 (supplies argument 2, the event id);
    // 0x004549FF: mov word [0x005BDA88],%cx; 0x00454A06: push %eax; 0x00454A07: push %ecx; 0x00454A08: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454F10U)));

    // 0x00454A10: push 0x00455070 (handler, deferred argument 3); 0x00454A15: call 0x004F40D0 (supplies argument 2, the event id);
    // 0x00454A1A: mov word [0x005BDA88],%dx; 0x00454A21: push %eax; 0x00454A22: push %edx; 0x00454A23: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x455070U)));

    // 0x00454A2B: push 0x004553E0 (handler, deferred argument 3); 0x00454A30: call 0x00416840 (supplies argument 2, the event id);
    // 0x00454A35: push %eax; 0x00454A36: mov word [0x005BDA88],%ax; 0x00454A3C: push %eax; 0x00454A3D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4553E0U)));

    // 0x00454A45: push 0x00455570 (handler, deferred argument 3); 0x00454A4A: call 0x00416860 (supplies argument 2, the event id);
    // 0x00454A4F: mov word [0x005BDA88],%cx; 0x00454A56: push %eax; 0x00454A57: push %ecx; 0x00454A58: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x455570U)));

    // 0x00454A60: push 0x00455780 (handler, deferred argument 3); 0x00454A65: call 0x00460C40 (supplies argument 2, the event id);
    // 0x00454A6A: mov word [0x005BDA88],%dx; 0x00454A71: push %eax; 0x00454A72: push %edx; 0x00454A73: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Core_GetGlobal8_460C40()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x455780U)));

    // 0x00454A7B: push 0x004D3710 (handler, deferred argument 3); 0x00454A80: call 0x0047E990 (supplies argument 2, the event id);
    // 0x00454A85: push %eax; 0x00454A86: mov word [0x005BDA88],%ax; 0x00454A8C: push %eax; 0x00454A8D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda88), static_cast<std::uint32_t>(Core_GetGlobal8_47E990()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x00454A95: push 0x2 (inert for the argument-less 0x00460C50 and the grow count for 0x004F1980); 0x00454A97: call 0x00460C50;
    // 0x00454A9C: push %eax; 0x00454A9D: call 0x004F1980; 0x00454AA5: mov [0x005BDA84],eax.
    const std::uint32_t container = Core_GetGlobal32_460C50(); // 0x00454A97: call 0x00460C50
    ::g_5bda84 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(container)), 2))); // 0x00454A9D: call 0x004F1980

    // 0x00454AAA: push 0 (argument 6); 0x00454AAC: push 7 (argument 5); 0x00454AAE: call 0x004F0FA0 (argument 4); 0x00454AB3: lea 0xd00(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCF8 (argument 3); 0x00454ABB: mov [0x005BDA84],%eax (argument 1); 0x00454AC0: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x00454AC7: call 0x004F1A80; 0x00454ACC: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bda84)),
                                append_frame + 0x00,   // 0x00454AC0: argument 2, the frame base
                                append_frame + 0xCF8,  // 0x00454AB3: argument 3, frame+0xCF8
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x00454AAE: argument 4
                                7,                     // 0x00454AAC: argument 5
                                0);                    // 0x00454AAA: argument 6

    // 0x00454ACF: push 0 (argument 6); 0x00454AD1: push 7 (argument 5); 0x00454AD3: call 0x004F0FA0 (argument 4); 0x00454AD8: lea 0xd04(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCFC (argument 3); 0x00454AE0: mov [0x005BDA84],%eax (argument 1); 0x00454AE5: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x00454AEC: call 0x004F1A80. Retail does not clean this call's six pushes
    // separately: the final `add esp,0xd68` at 0x00454AFB covers them together with the 0xD50 frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bda84)),
                                append_frame + 0x00,   // 0x00454AE5: argument 2, the frame base
                                append_frame + 0xCFC,  // 0x00454AD8: argument 3, frame+0xCFC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x00454AD3: argument 4
                                7,                     // 0x00454AD1: argument 5
                                0);                    // 0x00454ACF: argument 6

    // 0x00454AF1: movl $0x0,0x005BDA90. Then 0x00454AFB: add esp,0xd68 (frame + the six pushes above); 0x00454B01: ret.
    ::g_5bda90 = 0U;
} // end of Task_DefineHumanSoldierFemale

} // extern "C"
