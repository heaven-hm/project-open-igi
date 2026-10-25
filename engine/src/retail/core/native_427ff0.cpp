// Native: Missile_Register_427FF0 (placeholder name; retail symbol unknown; the class-name string it registers is "Missile" at 0x0053BC80). retail VA 0x00427FF0-0x00428266 (631 bytes, 1 block, 0 edges, 171 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00427FF0; the two listings agree. Extent: the entry begins with `sub esp,0x898`, the body is straight-line, and `c3` sits at 0x00428266 with 0x90 from 0x00428267 onward, so the extent is 631 bytes. Stack accounting replayed with esp simulated from the entry and reconciling exactly: 85 pushes (340 bytes) plus the 0x898-byte frame = 2540 bytes, and the fifteen incremental cleanups total exactly 2540 (`add esp,0x38` at 0x00428049 = 14 dwords; `add esp,0xC` at each of 0x00428064, 0x0042807E, 0x00428099, 0x004280B4, 0x004280CE, 0x004280E9, 0x00428104 and 0x0042811E = 3 dwords each; `add esp,0x18` at 0x0042814A; `add esp,0x10` at 0x0042816B; `add esp,0x18` at 0x00428195 and 0x004281B7; `add esp,0x40` at 0x00428223; `add esp,0x8BC` at 0x00428260 = the 0x898 frame plus 9 dwords), so the simulated esp is back to its entry value at the `ret` with zero slack. Call census from the 0xE8 enumeration: 0x004F45F0 x1, 0x00401900 x1, 0x00401400 x12, 0x00401530 x2, 0x004F1A60 x1, 0x004F1A70 x1, 0x004EE0E0 x1, 0x004EE0F0 x1, 0x00413B20 x1, 0x00413B10 x1, 0x00413B00 x1, 0x00416830 x1, 0x004FC840 x1, 0x004071D0 x1, 0x004F1950 x1, 0x004F1010 x1, 0x004F0F70 x1, 0x004F0FD0 x1, 0x004F1A80 x3, 0x004B8810 x9, 0x004B4720 x1; 43 call instructions, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004F45F0 (`mov ax,[0x0054E200]; ret`) - a FOURTH parent source, distinct from 0x00424850, 0x004018F0 and 0x004D9FA0, and the same one native_42db00.cpp uses for the base class. It registers "Missile" with a 0x898-byte object through 0x00401900 and stores the 16-bit id at 0x0053BB8C (image word 0x0181, the not-registered sentinel). Then FOURTEEN handler links on that id, in retail order: 0x00401400 event 0 -> 0x00428270; 0x00401530 event 1 -> 0x00428390; 0x00401530 event 2 -> 0x00428CA0; 0x00401400 the byte from 0x004F1A60 -> 0x00429890; 0x00401400 the byte from 0x004F1A70 -> 0x00429540; 0x00401400 the byte from 0x004EE0E0 -> 0x00429730; 0x00401400 the byte from 0x004EE0F0 -> 0x00429720; 0x00401400 the byte from 0x00413B20 -> 0x004298A0; 0x00401400 the byte from 0x00413B10 -> 0x004298E0; 0x00401400 the byte from 0x00413B00 -> 0x00429910; 0x00401400 the byte from 0x00416830 -> 0x00427FC0; 0x00401400 the byte from 0x004FC840 -> 0 (NULL); 0x00401400 literal event 0xC -> 0x0045F920; 0x00401400 the byte from 0x004071D0 -> 0x00429920. Every handler immediate is pushed BEFORE its argument-less getter, so it is not that getter's argument; it stays on the stack and the link consumes it as its third argument, which is why those cleanups are 3 dwords and not 2. Then it allocates a THREE-entry parameter container at 0x0057BC94 through 0x004F1950 and appends three records through 0x004F1A80, each with arg5 = 7 and arg6 = 0 and the same frame+0x00 as arg2, differing only in the third argument's frame offset and the getter: 0x004F1010 at frame+0x168, 0x004F0F70 at frame+0x70, 0x004F0FD0 at frame+0x154 (retail order, deliberately NOT sorted, and all three are 32-bit getters where the other registrars use 8-bit ones). Finally it makes NINE 0x004B8810 name registrations - "TASKTYPE_MISSILE" (0x0053BC6C) against the id, then "MISSILE_SMOKE_NONE" (0x0053BC58) -> 0, "MISSILE_SMOKE_NORMAL" (0x0053BC40) -> 1, "MISSILE_TYPE_MISSILE_DIRECT" (0x0053BC24) -> 0, "MISSILE_TYPE_MISSILE_BALLISTIC" (0x0053BC04) -> 2, "MISSILE_TYPE_MISSILE_ANTITANK" (0x0053BBE4) -> 3, "MISSILE_TYPE_MISSILE_KEYBOARD" (0x0053BBC4) -> 4, "MISSILE_TYPE_BOMB" (0x0053BBB0) -> 5, "MISSILE_TYPE_HIND_HELICOPTER" (0x0053BB90) -> 1 - and closes with 0x004B4720 on the three-dword block at 0x0057BC88. The id is masked to 16 bits before the TASKTYPE registration only.
// ONE MEASURED NULL HANDLER: the 0x004FC840 link pushes the literal immediate 0 before the getter, so the 0 survives as the handler and the link registers a null. It is a registered null, not an untranscribed address.
// THE ONLY FRAMED REGISTRAR IN THIS BAND, and the only one with a container: every other registrar I walked is frameless. The frame is 0x898 and the object size registered through 0x00401900 is also 0x898; those are two different quantities that happen to be equal, exactly as in 0x00424E80 (0x128) and 0x00425C80 (0x84). It is recorded as a coincidence, never as an identity.
// Status/limits: TRANSCRIBED; NOT LINKABLE YET, and the reason is a CALLEE gap, not a handler gap. Two of the fourteen event getters are not transcribed anywhere in the tree - 0x00413B00 (`mov al,[0x005391AC]; ret`) and 0x00413B10 (`mov al,[0x005391AD]; ret`) - and unlike handlers, which retail stores as data and the port passes as cast integers, these must actually be CALLED to produce the event id, so they are declared as NATIVE GAPs and this file cannot link until they land. They are the first two bytes of the seven-byte event-id block 0x005391AC..0x005391B2 owned by the registrar 0x00413DF0, whose sibling 0x00413B20 is transcribed as Core_GetFlagByte_413B20 in core/native_413b20.cpp; the gap names here follow that sibling's shape and are explicitly placeholders, not retail symbols (docs/DISCOVERED_NATIVES_RECORD.md lists 0x00413B00 as HumanPlayer_CheckGround_10, which is a candidate name from another project and is NOT used here). The twelve remaining handler addresses are raw code immediates with no implementation in the tree and are passed as cast integers, creating no link dependency: 0x00428270, 0x00428390, 0x00428CA0, 0x00429890, 0x00429540, 0x00429730, 0x00429720, 0x004298A0, 0x004298E0, 0x00429910, 0x00427FC0, 0x00429920. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries.
// Ownership notes (per the Lead's two-variable trap, and the mirror rule about naming): 0x0053BB8C, 0x0057BC88 and 0x0057BC94 are owned by this file and emitted with C++ linkage inside `namespace igi::retail::core`; no other file in the tree references any of the three. The id holds the sentinel word 0x0181, not zero. Every other callee is referenced by the name the tree already defines for it, checked by grepping each address before use.
// Domain: core/, per the Lead's ruling that these files are task-type registrars of the guard 0x00402930 family. This file is one of the 24 registrars the guard calls in descending address order at 0x00402B79-0x00402C32, and it is the FIRST call of that sequence; its teardown is 0x00427FB0.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

// 0x0053BB8C: the Missile class id. Image value 0x0181, the "not registered" sentinel, NOT zero.
std::uint16_t g_53bb8c = 0x0181U; // 0x0053BB8C

// 0x0057BC88: a three-dword block (0x0057BC88-0x0057BC93) written by Sys_StoreThreeWords_4B4720 at
// +0x00/+0x04/+0x08. Past the file-backed end of .data, so the image value is zero.
std::uint32_t g_57bc88[3] = {}; // 0x0057BC88

// 0x0057BC94: the three-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_57bc94 = 0U; // 0x0057BC94

} // namespace igi::retail::core

extern "C" {

// Argument-less getters. Each is referenced by the name the tree already defines for its address.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4F45F0(void); // 0x004F45F0: `mov ax,[0x54E200]; ret` (parent type; sys/native_4f45f0.cpp)
extern std::uint8_t NATIVE_DECL Script_PhaseIdGetE4_4F1A60(void); // 0x004F1A60: argument-less byte getter
extern std::uint8_t NATIVE_DECL Script_PhaseIdGetE5_4F1A70(void); // 0x004F1A70: argument-less byte getter
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EE0E0(void);   // 0x004EE0E0
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4EE0F0(void);   // 0x004EE0F0
extern std::uint8_t NATIVE_DECL Core_GetFlagByte_413B20(void); // 0x00413B20: `mov al,[0x5391AE]; ret` (core/native_413b20.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416830(void); // 0x00416830: `mov al,[0x539547]; ret`
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4FC840(void);   // 0x004FC840: argument-less byte getter (sys/native_4fc840.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_4071D0(void); // 0x004071D0: argument-less byte getter (sys/native_4071d0.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1010(void); // 0x004F1010: argument-less dword getter
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F70(void); // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern std::uint32_t NATIVE_DECL Core_GetRenderWordB_4F0FD0(void); // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)

// 0x00413B00 and 0x00413B10: NATIVE GAPS. Both are six-byte argument-less byte readers of the first two
// bytes of the event-id block 0x005391AC..0x005391B2 and neither is transcribed anywhere in the tree, so
// these two declarations cannot resolve and this file will not link until they land. They are CALLED, not
// stored, so they cannot be passed as cast integers the way an unwritten handler can.
extern std::uint8_t NATIVE_DECL Core_GetFlagByte_413B00(void); // 0x00413B00: `mov al,[0x5391AC]; ret` - NATIVE GAP
extern std::uint8_t NATIVE_DECL Core_GetFlagByte_413B10(void); // 0x00413B10: `mov al,[0x5391AD]; ret` - NATIVE GAP

extern void* Script_TaskContainer_Alloc(std::int32_t count);   // 0x004F1950 (script/native_4f1950.cpp)
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80
extern void Sys_StoreThreeWords_4B4720(void* target);          // 0x004B4720: stores three dwords at target+0x00/+0x04/+0x08
extern void Script_QHashNodeRegister(void* arg1, void* value);  // 0x004B8810 (declared in native_402ff0.cpp)

void NATIVE_DECL Missile_Register_427FF0()
{
    NATIVE_LOG_ENTER("Missile_Register_427FF0", "0x00427FF0");
    // 0x00427FF0: sub esp,0x898 - the only framed registrar in this band. The frame exists to hold the
    // three parameter records appended at the end; its size and the registered object size are equal by
    // coincidence, not identity.
    std::uint8_t append_frame[0x898];

    // 0x00427FF6/0x00427FF8: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004F45F0, the fourth parent source.
    const std::uint16_t parent = Sys_GetGlobal16_4F45F0();                    // 0x00427FFA
    const std::uint16_t class_id =
        TaskType_Register("Missile", 0x898U, parent, 0U);                     // 0x0042800A
    igi::retail::core::g_53bb8c = class_id;                                   // 0x00428017
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00428270)); // 0x0042801D
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00428390)); // 0x00428030
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00428CA0)); // 0x00428044
    // The handler immediate is pre-pushed before each argument-less getter and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429890));       // 0x0042805F
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429540));       // 0x00428079
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0E0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429730));       // 0x00428094
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4EE0F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429720));       // 0x004280AF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Core_GetFlagByte_413B20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004298A0));       // 0x004280C9
    // 0x004280D6/0x004280F1: two NATIVE GAP callees - the events 0x005391AD and 0x005391AC. Recorded gaps.
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Core_GetFlagByte_413B10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004298E0));       // 0x004280E4
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Core_GetFlagByte_413B00()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429910));       // 0x004280FF
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetSettingByte_416830()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00427FC0));       // 0x00428119
    // 0x00428121: the pre-pushed value is the literal 0, so this link registers a NULL handler.
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4FC840()),
        static_cast<igi::retail::core::FrameHandler>(nullptr));               // 0x00428131
    // 0x0042813D: literal event 0xC with a fixed handler immediate.
    QTask_LinkQTaskEventHandler(
        class_id, 0xCU, reinterpret_cast<igi::retail::core::FrameHandler>(0x0045F920)); // 0x00428145
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetSettingByte_4071D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00429920));       // 0x0042815F
    igi::retail::core::g_57bc94 = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(3)));     // 0x00428166: push 3
    // Each append: the two pre-pushed dwords arrive as arg6 (0) and arg5 (7). The arg3 frame offsets are
    // retail order (0x168, 0x70, 0x154) and are deliberately NOT sorted.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc94),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x00428189: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x168)), // 0x0042817C: lea ecx,[esp+0x170]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);                       // 0x00428177
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc94),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004281AB: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x70)), // 0x004281A1: lea ecx,[esp+0x78]
        static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);                       // 0x0042819C
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(igi::retail::core::g_57bc94),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x00)), // 0x004281D0: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(append_frame + 0x154)), // 0x004281C3: lea ecx,[esp+0x15C]
        static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);                   // 0x004281BE
    // Nine name registrations. 0x004B8810 takes (arg1 = name, value = id); retail pushes the value first.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_MISSILE"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(class_id))); // 0x004281EE
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_SMOKE_NONE"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U)));       // 0x004281FA
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_SMOKE_NORMAL"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));       // 0x00428206
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_MISSILE_DIRECT"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U)));       // 0x00428212
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_MISSILE_BALLISTIC"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U)));       // 0x0042821E
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_MISSILE_ANTITANK"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U)));       // 0x0042822D
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_MISSILE_KEYBOARD"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U)));       // 0x00428239
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_BOMB"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(5U)));       // 0x00428245
    Script_QHashNodeRegister(const_cast<char*>("MISSILE_TYPE_HIND_HELICOPTER"),
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));       // 0x00428251
    Sys_StoreThreeWords_4B4720(&igi::retail::core::g_57bc88[0]);              // 0x0042825B: push 0x0057BC88
}

} // extern "C"
