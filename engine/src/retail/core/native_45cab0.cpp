// Native: Task_DefineHumanSoldier (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0045CAB0-0x0045CC41.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 402 bytes (0x192), 1 block, 0 edges, 109
//   instructions, no branch and no loop; the extent is closed by the `ret` at 0x0045CC41, followed by the fourteen-byte nop pad 0x0045CC42-0x0045CC4F
//   and the next function (the event-0 handler 0x0045CC50).
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - the single block is 0x0045CAB0 + 402 bytes with no successors -
//   and the objdump listing build/retail-dis.txt was read over the whole extent, giving 26 call sites over 16 distinct callees, the same census shape
//   as the sibling registrars 0x00454970, 0x00456AD0 and 0x00459880. Every call site was decoded against its callee's own transcribed signature.
//   Image reads: 0x0053F2E0 holds "HumanSoldier"; 0x005BDAB4, 0x005BDABC and 0x005BDAC0 lie past `.data`'s raw size, so their image value is the
//   zero fill and the port's initialisers are 0 (this image has no `.bss`; `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000).
// Behavior/CFG: one straight-line body with a 0xD50-byte frame (`sub esp,0xd50`, no saved-register pushes, and no stack probe because 3408 bytes is
//   below the 4 KB probe threshold - so there is no probe call site to transcribe). In retail order it registers the "HumanSoldier" type of object
//   size 0xD50 - the same 0xD50 as the frame - with the parent type Core_GetGlobal16_460BE0() returns, storing the returned id at 0x005BDABC; sets the
//   inherited flags to 1 through 0x00401FA0; links handler 0x0045CC50 for event 0 through 0x00401400 and handlers 0x0045CF90 and 0x00459EC0 for
//   events 1 and 2 through 0x00401530; then links seven further 0x00401400 events in retail order - 0x0045D030 on the event from 0x004F1A60,
//   0x00456E50 on the event from 0x004F1A70, 0x0045D040 on the event from 0x004F40D0, 0x0045D3C0 on the event from 0x00416840, 0x0045A3D0 on the
//   event from 0x00416860, 0x0045A5E0 on the event from 0x00460C40 and 0x004D3710 on the event from 0x0047E990; grows the container
//   Core_GetGlobal32_460C50() by 2 into 0x005BDAB4; appends two records through 0x004F1A80 whose frame offsets are 0xCF8 and 0xCFC with argument 5 = 7
//   and argument 6 = 0; and finally clears the dword at 0x005BDAC0.
//   Handler sharing, as bytes rather than as similarity: this body links its OWN 0x0045CC50 and 0x0045CF90 on events 0 and 1, while event 2 keeps the
//   shared 0x00459EC0 that 0x00454970, 0x00456AD0 and 0x00459880 also link; 0x00456E50 on the 0x004F1A70 event is the handler 0x00456AD0 and
//   0x00459880 link there; 0x0045A3D0 and 0x0045A5E0 on the 0x00416860 and 0x00460C40 events are the handlers 0x00459880 links there. 0x004D3710 is
//   the most-shared handler in the image (163 reference sites).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 109-instruction listing; runtime UNOBSERVED. Census: 26 call sites / 16 distinct both
//   in retail and in the port object, every target 1:1 - 0x00401400 x8, 0x00401530 x2, 0x00460BE0 x1, 0x00401900 x1, 0x00401FA0 x1, 0x00416840 x1,
//   0x00416860 x1, 0x00460C40 x1, 0x00460C50 x1, 0x0047E990 x1, 0x004F0FA0 x2, 0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x2,
//   0x004F40D0 x1.
//   Cleanup reconciliation: `add esp,0x40` at 0x0045CB18 = 16 dwords = the two deferred zeros plus the three registration pushes plus the two of
//   0x00401FA0 plus the three of each of the first three links; seven `add esp,0xc` = the seven remaining 0x00401400 links; `add esp,0x8` at
//   0x0045CBE2 = the grow's two (the count pushed before the argument-less 0x00460C50 and that call's return); `add esp,0x18` at 0x0045CC0C = the
//   first append's six; and the final `add esp,0xd68` at 0x0045CC3B = 0xD50 frame + 0x18 = the second append's six plus the whole frame. 51 pushes =
//   204 bytes; 204 + 3408 frame = 3612 = 0x40 + 7 * 0xC + 0x8 + 0x18 + 0xD68, and esp is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x40; the port's four-parameter signature emits
//   one zero for the task-list argument; (2) the 0xD50-byte frame is reproduced as one addressable dword whose address plus 0xCF8 and 0xCFC stands in
//   for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither, so the recorded
//   values are record sizes rather than frame pointers; (3) the parent type comes from 0x00460BE0 and not from 0x004018F0, as retail calls it.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDABC at thirteen sites, eight in
//   this body (0x0045CAD4, 0x0045CADF, 0x0045CAF3, 0x0045CB07, 0x0045CB28, 0x0045CB42, 0x0045CB5D, 0x0045CB78) and the rest in the handlers this body
//   links and in other bodies, which must declare it `extern`; 0x005BDAB4 at 0x0045CBE6, 0x0045CBFC and 0x0045CC21 (this body), 0x0045D036 (inside
//   handler 0x0045D030) and 0x0045D581 (another body); 0x005BDAC0 at 0x0045CC33 only, so this body owns it outright. The definitions below are owned
//   here because this body writes them first.
//   Handlers 0x0045CC50, 0x0045CF90, 0x00459EC0, 0x0045D030, 0x00456E50, 0x0045D040, 0x0045D3C0, 0x0045A3D0, 0x0045A5E0 and 0x004D3710 are raw code
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

// 0x005BDABC: the "HumanSoldier" task type id (word). Zero in the image (past `.data`'s raw size). Thirteen reference sites; the definition is owned
// here and the handlers this body links must declare it `extern`.
std::uint16_t g_5bdabc = 0U;

// 0x005BDAB4: the "HumanSoldier" task container this body grows. Zero in the image. Also read by handler 0x0045D030 and by 0x0045D581.
std::uint32_t g_5bdab4 = 0U;

// 0x005BDAC0: cleared by this body and referenced nowhere else in `.text`. Zero in the image.
std::uint32_t g_5bdac0 = 0U;

void Task_DefineHumanSoldier()
{
    // 0x0045CAB0: sub esp,0xd50 - the frame, 3408 bytes, no probe because it is below 4 KB.
    // 0x0045CAB6: push 0; 0x0045CAB8: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. The frame is reproduced as one addressable dword whose address stands in for the frame base, because 0x004F1A80 needs the frame
    // offsets 0xCF8 and 0xCFC and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0045CABA: call 0x00460BE0 (parent type); 0x0045CABF: push %eax; 0x0045CAC0: push 0xd50; 0x0045CAC5: push 0x0053F2E0; 0x0045CACA: call 0x00401900.
    const std::uint16_t soldier_id = TaskType_Register("HumanSoldier", // 0x0045CAC5: push 0x0053F2E0
                                                       0xD50U,          // 0x0045CAC0: push 0xd50 (object size, equal to the frame)
                                                       Core_GetGlobal16_460BE0(), // 0x0045CABA: call 0x00460BE0 (parent type)
                                                       0U);             // 0x0045CAB8: push 0 (task-list argument)

    // 0x0045CACF: push 0x1 (argument 2, the flags); 0x0045CAD1: push %eax (argument 1, the id); 0x0045CAD2: mov word [0x005BDABC],ax;
    // 0x0045CAD8: call 0x00401FA0. The store precedes the call in retail and is kept before it here.
    ::g_5bdabc = soldier_id; // 0x0045CAD2
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(soldier_id), 0x1U);

    // 0x0045CAE3: push 0x0045CC50 (handler); 0x0045CAE8: push 0x0 (argument 2, literal event id); 0x0045CAEA: push %eax (argument 1, the id
    // RELOADED from the global); 0x0045CAEB: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45CC50U)));

    // 0x0045CAF7: push 0x0045CF90 (handler - this body's own event-1 handler, not the shared 0x00454EE0); 0x0045CAFC: push 0x1 (argument 2, literal
    // event id); 0x0045CAFE: push %ecx (argument 1); 0x0045CAFF: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdabc), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45CF90U)));

    // 0x0045CB0B: push 0x00459EC0 (handler - the one shared event-2 handler); 0x0045CB10: push 0x2 (argument 2, literal event id);
    // 0x0045CB12: push %edx (argument 1); 0x0045CB13: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdabc), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459EC0U)));

    // 0x0045CB1B: push 0x0045D030 (handler, deferred argument 3); 0x0045CB20: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x0045CB25: push %eax; 0x0045CB26: mov word [0x005BDABC],%ax; 0x0045CB2C: push %eax; 0x0045CB2D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45D030U)));

    // 0x0045CB35: push 0x00456E50 (handler - the handler 0x00456AD0 and 0x00459880 link on this event); 0x0045CB3A: call 0x004F1A70 (supplies
    // argument 2, the event id); 0x0045CB3F: mov word [0x005BDABC],%cx; 0x0045CB46: push %eax; 0x0045CB47: push %ecx; 0x0045CB48: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456E50U)));

    // 0x0045CB50: push 0x0045D040 (handler, deferred argument 3); 0x0045CB55: call 0x004F40D0 (supplies argument 2, the event id);
    // 0x0045CB5A: mov word [0x005BDABC],%dx; 0x0045CB61: push %eax; 0x0045CB62: push %edx; 0x0045CB63: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45D040U)));

    // 0x0045CB6B: push 0x0045D3C0 (handler, deferred argument 3); 0x0045CB70: call 0x00416840 (supplies argument 2, the event id);
    // 0x0045CB75: push %eax; 0x0045CB76: mov word [0x005BDABC],%ax; 0x0045CB7C: push %eax; 0x0045CB7D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45D3C0U)));

    // 0x0045CB85: push 0x0045A3D0 (handler - the handler 0x00459880 links on this event); 0x0045CB8A: call 0x00416860 (supplies argument 2,
    // the event id); 0x0045CB8F: mov word [0x005BDABC],%cx; 0x0045CB96: push %eax; 0x0045CB97: push %ecx; 0x0045CB98: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45A3D0U)));

    // 0x0045CBA0: push 0x0045A5E0 (handler - likewise shared with 0x00459880); 0x0045CBA5: call 0x00460C40 (supplies argument 2, the event id);
    // 0x0045CBAA: mov word [0x005BDABC],%dx; 0x0045CBB1: push %eax; 0x0045CBB2: push %edx; 0x0045CBB3: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Core_GetGlobal8_460C40()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x45A5E0U)));

    // 0x0045CBBB: push 0x004D3710 (handler, deferred argument 3); 0x0045CBC0: call 0x0047E990 (supplies argument 2, the event id);
    // 0x0045CBC5: push %eax; 0x0045CBC6: mov word [0x005BDABC],%ax; 0x0045CBCC: push %eax; 0x0045CBCD: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdabc), static_cast<std::uint32_t>(Core_GetGlobal8_47E990()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x0045CBD5: push 0x2 (inert for the argument-less 0x00460C50 and the grow count for 0x004F1980); 0x0045CBD7: call 0x00460C50;
    // 0x0045CBDC: push %eax; 0x0045CBDD: call 0x004F1980; 0x0045CBE5: mov [0x005BDAB4],eax.
    const std::uint32_t container = Core_GetGlobal32_460C50(); // 0x0045CBD7: call 0x00460C50
    ::g_5bdab4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(container)), 2))); // 0x0045CBDD: call 0x004F1980

    // 0x0045CBEA: push 0 (argument 6); 0x0045CBEC: push 7 (argument 5); 0x0045CBEE: call 0x004F0FA0 (argument 4); 0x0045CBF3: lea 0xd00(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCF8 (argument 3); 0x0045CBFB: mov [0x005BDAB4],%eax (argument 1); 0x0045CC00: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045CC07: call 0x004F1A80; 0x0045CC0C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdab4)),
                                append_frame + 0x00,   // 0x0045CC00: argument 2, the frame base
                                append_frame + 0xCF8,  // 0x0045CBF3: argument 3, frame+0xCF8
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x0045CBEE: argument 4
                                7,                     // 0x0045CBEC: argument 5
                                0);                    // 0x0045CBEA: argument 6

    // 0x0045CC0F: push 0 (argument 6); 0x0045CC11: push 7 (argument 5); 0x0045CC13: call 0x004F0FA0 (argument 4); 0x0045CC18: lea 0xd04(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCFC (argument 3); 0x0045CC20: mov [0x005BDAB4],%eax (argument 1); 0x0045CC25: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x0045CC2C: call 0x004F1A80. Retail does not clean this call's six pushes
    // separately: the final `add esp,0xd68` at 0x0045CC3B covers them together with the 0xD50 frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdab4)),
                                append_frame + 0x00,   // 0x0045CC25: argument 2, the frame base
                                append_frame + 0xCFC,  // 0x0045CC18: argument 3, frame+0xCFC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x0045CC13: argument 4
                                7,                     // 0x0045CC11: argument 5
                                0);                    // 0x0045CC0F: argument 6

    // 0x0045CC31: movl $0x0,0x005BDAC0. Then 0x0045CC3B: add esp,0xd68 (frame + the six pushes above); 0x0045CC41: ret.
    ::g_5bdac0 = 0U;
} // end of Task_DefineHumanSoldier

} // extern "C"
