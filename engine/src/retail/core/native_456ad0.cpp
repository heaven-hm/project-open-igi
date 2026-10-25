// Native: Task_DefineHumanSoldierRPG (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00456AD0-0x00456C61.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 402 bytes (0x192), 1 block, 0 edges, 109
//   instructions, no branch and no loop; the extent is closed by the `ret` at 0x00456C61, followed by the fourteen-byte nop pad 0x00456C62-0x00456C6F
//   and the next function (the event-0 handler 0x00456C70).
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - the single block is 0x00456AD0 + 402 bytes with no successors -
//   and the objdump listing build/retail-dis.txt was read over the whole extent, giving 26 call sites over 16 distinct callees, the same census shape
//   as the sibling registrar 0x00454970 (core/native_454970.cpp). Every call site was decoded against its callee's own transcribed signature. Image
//   reads: 0x0053E8A8 holds "HumanSoldierRPG"; 0x005BDA94, 0x005BDA98 and 0x005BDAA0 lie past `.data`'s raw size, so their image value is the zero
//   fill and the port's initialisers are 0 (this image has no `.bss`; `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000).
// Behavior/CFG: one straight-line body with a 0xD40-byte frame (`sub esp,0xd40`, no saved-register pushes, and no stack probe because 3392 bytes is
//   below the 4 KB probe threshold - so there is no probe call site to transcribe). In retail order it registers the "HumanSoldierRPG" type of object
//   size 0xD40 - the same 0xD40 as the frame - with the parent type Core_GetGlobal16_460BE0() returns, storing the returned id at 0x005BDA94; sets the
//   inherited flags to 1 through 0x00401FA0; links handler 0x00456C70 for event 0 through 0x00401400 and handlers 0x00454EE0 and 0x00459EC0 for
//   events 1 and 2 through 0x00401530; then links seven further 0x00401400 events in retail order - 0x00456E40 on the event from 0x004F1A60,
//   0x00456E50 on the event from 0x004F1A70, 0x00456FB0 on the event from 0x004F40D0, 0x004572D0 on the event from 0x00416840, 0x00457430 on the
//   event from 0x00416860, 0x00457620 on the event from 0x00460C40 and 0x004D3710 on the event from 0x0047E990; grows the container
//   Core_GetGlobal32_460C50() by 2 into 0x005BDA98; appends two records through 0x004F1A80 whose frame offsets are 0xCF8 and 0xCFC with argument 5 = 7
//   and argument 6 = 0; and finally clears the dword at 0x005BDAA0.
//   Handlers 0x00454EE0 and 0x00459EC0 are SHARED with 0x00454970, which links them on the same events 1 and 2; 0x004D3710 is the most-shared
//   handler in the image (163 reference sites). Both facts are the bytes, not an inference from similar names.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 109-instruction listing; runtime UNOBSERVED. Census: 26 call sites / 16 distinct both
//   in retail and in the port object, every target 1:1 - 0x00401400 x8, 0x00401530 x2, 0x00460BE0 x1, 0x00401900 x1, 0x00401FA0 x1, 0x00416840 x1,
//   0x00416860 x1, 0x00460C40 x1, 0x00460C50 x1, 0x0047E990 x1, 0x004F0FA0 x2, 0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x2,
//   0x004F40D0 x1.
//   Cleanup reconciliation: `add esp,0x40` at 0x00456B38 = 16 dwords = the two deferred zeros plus the three registration pushes plus the two of
//   0x00401FA0 plus the three of each of the first three links; seven `add esp,0xc` = the seven remaining 0x00401400 links; `add esp,0x8` at
//   0x00456C02 = the grow's two (the count pushed before the argument-less 0x00460C50 and that call's return); `add esp,0x18` at 0x00456C2C = the
//   first append's six; and the final `add esp,0xd58` at 0x00456C5B = 0xD40 frame + 0x18 = the second append's six plus the whole frame. 51 pushes =
//   204 bytes; 204 + 3392 frame = 3596 = 0x40 + 7 * 0xC + 0x8 + 0x18 + 0xD58, and esp is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x40; the port's four-parameter signature emits
//   one zero for the task-list argument; (2) the 0xD40-byte frame is reproduced as one addressable dword whose address plus 0xCF8 and 0xCFC stands in
//   for the frame base, which is sound because 0x004F1A80 reads arguments 2 and 3 only as their difference and dereferences neither, so the recorded
//   values are record sizes rather than frame pointers; (3) the parent type comes from 0x00460BE0 and not from 0x004018F0, as retail calls it.
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDA94 at thirteen sites, eight in
//   this body (0x00456AF4, 0x00456AFF, 0x00456B13, 0x00456B27, 0x00456B48, 0x00456B62, 0x00456B7D, 0x00456B98) and the rest in the handlers this
//   body links and in other bodies, which must declare it `extern`; 0x005BDA98 at 0x00456C06, 0x00456C1C and 0x00456C41 (this body), 0x00456E46
//   (inside handler 0x00456E40) and 0x004577E1 (another body); 0x005BDAA0 at 0x00456C53 only, so this body owns it outright. The definitions below are
//   owned here because this body writes them first.
//   Handlers 0x00456C70, 0x00454EE0, 0x00459EC0, 0x00456E40, 0x00456E50, 0x00456FB0, 0x004572D0, 0x00457430, 0x00457620 and 0x004D3710 are raw code
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

// 0x005BDA94: the "HumanSoldierRPG" task type id (word). Zero in the image (past `.data`'s raw size). Thirteen reference sites; the definition is
// owned here and the handlers this body links must declare it `extern`.
std::uint16_t g_5bda94 = 0U;

// 0x005BDA98: the "HumanSoldierRPG" task container this body grows. Zero in the image. Also read by handler 0x00456E40 and by 0x004577E1.
std::uint32_t g_5bda98 = 0U;

// 0x005BDAA0: cleared by this body and referenced nowhere else in `.text`. Zero in the image.
std::uint32_t g_5bdaa0 = 0U;

void Task_DefineHumanSoldierRPG()
{
    // 0x00456AD0: sub esp,0xd40 - the frame, 3392 bytes, no probe because it is below 4 KB.
    // 0x00456AD6: push 0; 0x00456AD8: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. The frame is reproduced as one addressable dword whose address stands in for the frame base, because 0x004F1A80 needs the frame
    // offsets 0xCF8 and 0xCFC and reads its two frame arguments only as their difference.
    std::int32_t append_frame_base = 0;
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00456ADA: call 0x00460BE0 (parent type); 0x00456ADF: push %eax; 0x00456AE0: push 0xd40; 0x00456AE5: push 0x0053E8A8; 0x00456AEA: call 0x00401900.
    const std::uint16_t soldier_rpg_id = TaskType_Register("HumanSoldierRPG", // 0x00456AE5: push 0x0053E8A8
                                                           0xD40U,              // 0x00456AE0: push 0xd40 (object size, equal to the frame)
                                                           Core_GetGlobal16_460BE0(), // 0x00456ADA: call 0x00460BE0 (parent type)
                                                           0U);                 // 0x00456AD8: push 0 (task-list argument)

    // 0x00456AEF: push 0x1 (argument 2, the flags); 0x00456AF1: push %eax (argument 1, the id); 0x00456AF2: mov word [0x005BDA94],ax;
    // 0x00456AF8: call 0x00401FA0. The store precedes the call in retail and is kept before it here.
    ::g_5bda94 = soldier_rpg_id; // 0x00456AF2
    TaskType_SetInheritedFlags_401FA0(static_cast<std::uint32_t>(soldier_rpg_id), 0x1U);

    // 0x00456B03: push 0x00456C70 (handler); 0x00456B08: push 0x0 (argument 2, literal event id); 0x00456B0A: push %eax (argument 1, the id
    // RELOADED from the global); 0x00456B0B: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456C70U)));

    // 0x00456B17: push 0x00454EE0 (handler - the same handler 0x00454970 links on this event); 0x00456B1C: push 0x1 (argument 2, literal event id);
    // 0x00456B1E: push %ecx (argument 1); 0x00456B1F: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bda94), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x454EE0U)));

    // 0x00456B2B: push 0x00459EC0 (handler - likewise shared with 0x00454970); 0x00456B30: push 0x2 (argument 2, literal event id);
    // 0x00456B32: push %edx (argument 1); 0x00456B33: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bda94), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x459EC0U)));

    // 0x00456B3B: push 0x00456E40 (handler, deferred argument 3); 0x00456B40: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x00456B45: push %eax; 0x00456B46: mov word [0x005BDA94],%ax; 0x00456B4C: push %eax; 0x00456B4D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456E40U)));

    // 0x00456B55: push 0x00456E50 (handler, deferred argument 3); 0x00456B5A: call 0x004F1A70 (supplies argument 2, the event id);
    // 0x00456B5F: mov word [0x005BDA94],%cx; 0x00456B66: push %eax; 0x00456B67: push %ecx; 0x00456B68: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456E50U)));

    // 0x00456B70: push 0x00456FB0 (handler, deferred argument 3); 0x00456B75: call 0x004F40D0 (supplies argument 2, the event id);
    // 0x00456B7A: mov word [0x005BDA94],%dx; 0x00456B81: push %eax; 0x00456B82: push %edx; 0x00456B83: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x456FB0U)));

    // 0x00456B8B: push 0x004572D0 (handler, deferred argument 3); 0x00456B90: call 0x00416840 (supplies argument 2, the event id);
    // 0x00456B95: push %eax; 0x00456B96: mov word [0x005BDA94],%ax; 0x00456B9C: push %eax; 0x00456B9D: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4572D0U)));

    // 0x00456BA5: push 0x00457430 (handler, deferred argument 3); 0x00456BAA: call 0x00416860 (supplies argument 2, the event id);
    // 0x00456BAF: mov word [0x005BDA94],%cx; 0x00456BB6: push %eax; 0x00456BB7: push %ecx; 0x00456BB8: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Sys_GetSettingByte_416860()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x457430U)));

    // 0x00456BC0: push 0x00457620 (handler, deferred argument 3); 0x00456BC5: call 0x00460C40 (supplies argument 2, the event id);
    // 0x00456BCA: mov word [0x005BDA94],%dx; 0x00456BD1: push %eax; 0x00456BD2: push %edx; 0x00456BD3: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Core_GetGlobal8_460C40()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x457620U)));

    // 0x00456BDB: push 0x004D3710 (handler, deferred argument 3); 0x00456BE0: call 0x0047E990 (supplies argument 2, the event id);
    // 0x00456BE5: push %eax; 0x00456BE6: mov word [0x005BDA94],%ax; 0x00456BEC: push %eax; 0x00456BED: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bda94), static_cast<std::uint32_t>(Core_GetGlobal8_47E990()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x00456BF5: push 0x2 (inert for the argument-less 0x00460C50 and the grow count for 0x004F1980); 0x00456BF7: call 0x00460C50;
    // 0x00456BFC: push %eax; 0x00456BFD: call 0x004F1980; 0x00456C05: mov [0x005BDA98],eax.
    const std::uint32_t container = Core_GetGlobal32_460C50(); // 0x00456BF7: call 0x00460C50
    ::g_5bda98 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Grow(
        reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(container)), 2))); // 0x00456BFD: call 0x004F1980

    // 0x00456C0A: push 0 (argument 6); 0x00456C0C: push 7 (argument 5); 0x00456C0E: call 0x004F0FA0 (argument 4); 0x00456C13: lea 0xd00(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCF8 (argument 3); 0x00456C1B: mov [0x005BDA98],%eax (argument 1); 0x00456C20: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x00456C27: call 0x004F1A80; 0x00456C2C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bda98)),
                                append_frame + 0x00,   // 0x00456C20: argument 2, the frame base
                                append_frame + 0xCF8,  // 0x00456C13: argument 3, frame+0xCF8
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x00456C0E: argument 4
                                7,                     // 0x00456C0C: argument 5
                                0);                    // 0x00456C0A: argument 6

    // 0x00456C2F: push 0 (argument 6); 0x00456C31: push 7 (argument 5); 0x00456C33: call 0x004F0FA0 (argument 4); 0x00456C38: lea 0xd04(%esp),%ecx
    // - with esp at frame-8 there, that is frame+0xCFC (argument 3); 0x00456C40: mov [0x005BDA98],%eax (argument 1); 0x00456C45: lea 0xc(%esp),%edx
    // - with esp at frame-12 there, that is frame+0x00 (argument 2); 0x00456C4C: call 0x004F1A80. Retail does not clean this call's six pushes
    // separately: the final `add esp,0xd58` at 0x00456C5B covers them together with the 0xD40 frame.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bda98)),
                                append_frame + 0x00,   // 0x00456C45: argument 2, the frame base
                                append_frame + 0xCFC,  // 0x00456C38: argument 3, frame+0xCFC
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), // 0x00456C33: argument 4
                                7,                     // 0x00456C31: argument 5
                                0);                    // 0x00456C2F: argument 6

    // 0x00456C51: movl $0x0,0x005BDAA0. Then 0x00456C5B: add esp,0xd58 (frame + the six pushes above); 0x00456C61: ret.
    ::g_5bdaa0 = 0U;
} // end of Task_DefineHumanSoldierRPG

} // extern "C"
