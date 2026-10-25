// Native: Train_Register_42ACC0 (placeholder name; retail symbol unknown; the class-name string it registers is "Train" at 0x0053BD1C). retail VA 0x0042ACC0-0x0042B025 (870 bytes, 1 block, 0 edges, 246 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x0042ACC0 plus capstone 5.0.7 aligned from the verified entry; the two listings agree instruction for instruction. Extent bounded by the recon's measured 870 bytes and confirmed independently: the entry is `sub esp,0x8E0`, the body is straight-line, and the `ret` sits at 0x0042B025, which is the final byte of 0x0042ACC0+870. THE BOUND MATTERS HERE: an unbounded `pd 260 @ 0x0042ACC0` prints TWO `ret` instructions, 0x0042B025 (line 246, this routine's) and 0x0042B03C (line 260, the next function's), so a routine bounded by "the first ret in a pd dump" silently absorbs its neighbour. Stack accounting replayed with esp simulated from the entry and reconciling exactly: 117 pushes (468 bytes) plus the 0x8E0-byte frame (2272 bytes) = 2740 bytes, and the twenty-four cleanups total exactly 2740 (0x2C at 0x0042AD05 = 11 dwords; 0xC at 0x0042AD20; 0x24 at 0x0042AD62 = 9; eleven 0xC cleanups at 0x0042AD7D, 0x0042AD97, 0x0042ADB2, 0x0042ADCD, 0x0042ADE7, 0x0042AE02, 0x0042AE1D, 0x0042AE37, 0x0042AE52, 0x0042AE6D = 3 dwords each; 0x10 at 0x0042AE8E = 4; nine 0x18 cleanups at 0x0042AECC, 0x0042AEF1, 0x0042AF16, 0x0042AF3B, 0x0042AF60, 0x0042AF85, 0x0042AFAA, 0x0042AFD2, 0x0042AFFA = 6 dwords each; and 0x8F8 at 0x0042B01F = the 0x8E0 frame plus the final append's six dwords), so the simulated esp is back to its entry value at the `ret` with zero slack. Call census re-measured from this body's own call instructions: 53 call sites over 24 distinct targets - 0x00401400 x15, 0x004F1A80 x10, 0x004F0F50 x6, 0x00401530 x2, and one each of 0x004F45F0, 0x00401900, 0x004F1A70, 0x004F1A60, 0x004E6610, 0x004F40D0, 0x004F40E0, 0x004F4110, 0x004F4120, 0x004F4130, 0x004F4140, 0x00416890, 0x004C5800, 0x004CEA20, 0x004CEA10, 0x004F1950, 0x004F0FA0, 0x004F0FF0, 0x004F1000, 0x004F0FC0. THE RECON'S CALL TABLE WAS CHECKED AGAINST THIS LIST RATHER THAN COPIED, AND SEVEN OF ITS ENTRIES DO NOT OCCUR IN THE BODY AT ALL: 0x004ECDB0, 0x004FB700, 0x004F4820, 0x004F4800, 0x004F4660, 0x004F4600 and 0x0042A2C0, while 0x004F1950 (x1) and 0x004F0F50 (x6) are missing from its table. The recon's prose count of 53 sites over 24 targets is right; its table is stale, and its open item naming 0x0042A2C0 as a callee of this routine is wrong - this routine does not call it.
// Behavior/CFG: one straight-line block, no branch, no loop. The parent getter is 0x004F45F0 (`mov ax,[0x0054E200]; ret`), the same fourth parent source 0x00427FF0 uses, and the two zeros pushed before it are surplus for an argument-less getter: the second supplies 0x00401900's fourth argument. It registers "Train" with an 0x8E0-byte object through 0x00401900 and stores the 16-bit id at 0x0057BCB8. Then SEVENTEEN handler links on that id, in retail order: 0x00401400 event 0 -> 0x00429F80; 0x00401530 event 2 -> 0x0042A100; 0x00401400 the byte from 0x004F1A70 -> 0x0042A150; 0x00401400 the byte from 0x004F1A60 -> 0x0042B030; 0x00401400 event 7 -> 0x0042B500; 0x00401530 event 1 -> 0x0042B700; 0x00401400 the byte from 0x004E6610 -> 0x0042B220; 0x00401400 the byte from 0x004F40D0 -> 0x0042B040; 0x00401400 the byte from 0x004F40E0 -> 0x0042B2C0; 0x00401400 the byte from 0x004F4110 -> 0x004EFF20; 0x00401400 the byte from 0x004F4120 -> 0x0042B240; 0x00401400 the byte from 0x004F4130 -> 0x0042B280; 0x00401400 the byte from 0x004F4140 -> 0x0042B2F0; 0x00401400 the byte from 0x00416890 -> 0x0042B930; 0x00401400 the byte from 0x004C5800 -> 0x0042AC20; 0x00401400 the byte from 0x004CEA20 -> 0x0042BBA0; 0x00401400 the byte from 0x004CEA10 -> 0x0042BB60. Every handler immediate is pushed BEFORE its argument-less getter, so it is not that getter's argument: it stays on the stack and the link consumes it as its third argument, which is why the 3-dword cleanups are 3 and not 2. WHICH LINK ROUTINE EACH SITE USES WAS READ FROM THE BYTES, NOT FROM A PATTERN, AND IT CORRECTS THE RECON: the recon paired 0x42B500/event 7 with 0x00401530, but the call at 0x0042AD49 is `e8 b2 66 fd ff` = 0x00401400, and the band's two 0x00401530 sites are 0x0042AD00 (handler 0x0042A100, event 2) and 0x0042AD5D (handler 0x0042B700, event 1). That distinction is state, not style: 0x00401400 increments the context handler count at 0x00567C6C and 0x00401530 the other counter at 0x00567C70, so swapping them changes what the program records. Then it allocates a TEN-entry parameter container at 0x0057BCBC through 0x004F1950 (the `push 0xA` at 0x0042AE87), stores the two float bit patterns 0xC0A00000 and 0x40A00000 at 0x0057BCE8 and 0x0057BCEC (IEEE-754 -5.0f and +5.0f; the instructions store the raw dwords and are transcribed as raw dwords), and appends TEN records through 0x004F1A80, each with arg2 = the same frame+0x00, arg5 = 7, and arg6 = 0 except the eighth and ninth, which pass the ADDRESS 0x0057BCE8: 0x004F0F50 at frame+0x76C, 0x004F0F50 at frame+0x758, 0x004F0FA0 at frame+0x77C, 0x004F0FF0 at frame+0x780, 0x004F0F50 at frame+0x750, 0x004F0F50 at frame+0x754, 0x004F1000 at frame+0x880, 0x004F0F50 at frame+0x770 with arg6 = &0x0057BCE8, 0x004F0F50 at frame+0x774 with arg6 = &0x0057BCE8, 0x004F0FC0 at frame+0x890. The ten destination offsets are retail order and are deliberately NOT sorted (0x76C, 0x758, 0x77C, 0x780, 0x750, 0x754, 0x880, 0x770, 0x774, 0x890); each was derived from its own `lea` by replaying esp at that instruction, not copied from a sibling, because this band has no canonical append shape. It is the SECOND container-owning framed registrar in the family and the one with the larger frame (0x8E0 against 0x00427FF0's 0x898); it shares 0x00427FF0's parent getter. NO name registrations through 0x004B8810 appear in this body.
// Status/limits: TRANSCRIBED; LINKABLE, because all 24 call targets are transcribed in the tree under the names used here and none is a gap: 0x004E6610 is core/native_004e6610.cpp (Task_GetCachedEventId_4E6610), which a glob for native_4e6610.cpp misses because the file name carries a leading zero. The seventeen handler addresses are raw code immediates with no implementation in the tree and are passed as cast integers, creating no link dependency: 0x00429F80, 0x0042A100, 0x0042A150, 0x0042B030, 0x0042B500, 0x0042B700, 0x0042B220, 0x0042B040, 0x0042B2C0, 0x004EFF20, 0x0042B240, 0x0042B280, 0x0042B2F0, 0x0042B930, 0x0042AC20, 0x0042BBA0, 0x0042BB60. STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile, zero warning lines). Runtime UNOBSERVED in both binaries. The four slots this body writes are owned by this file and emitted with C linkage inside `extern "C"`, per the ruling that native_42bc00.cpp declares 0x0057BCB8 and 0x0057BCBC the same way; the check that no other file in the tree references any of the four was run before defining them. 0x0057BCB8, 0x0057BCBC, 0x0057BCE8 and 0x0057BCEC all sit PAST the file-backed end of .data (raw ends at 0x00550000, virtual at 0x00C32558), so their image values are zero and not the 0x0181 sentinel that file-backed id slots carry. The frame and the registered object size are both 0x8E0; as in 0x00427FF0 those are two different quantities that happen to be equal, and that is recorded as a coincidence, never as an identity.
// Domain: core/, per the ruling that these files are task-type registrars of the guard 0x00402930 family. This file is the registrar the guard calls at 0x00402B97, and its teardown is 0x0042BC00, which was blocked on this file's two slots and is unblocked by it.
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

extern "C" {

// 0x0057BCB8: the Train class id, written once at 0x0042ACE7 as a 16-bit word and read 16-bit throughout.
// Past the file-backed end of .data, so the image value is zero.
std::uint16_t g_57bcb8 = 0U; // 0x0057BCB8

// 0x0057BCBC: the ten-entry parameter container handle, written at 0x0042AE91 from 0x004F1950's return.
std::uint32_t g_57bcbc = 0U; // 0x0057BCBC

// 0x0057BCE8 and 0x0057BCEC: two dwords written at 0x0042AE96/0x0042AEA0 with the raw immediates
// 0xC0A00000 and 0x40A00000 - the IEEE-754 encodings of -5.0f and +5.0f, stored as dwords because that is
// what the instructions store. 0x0042AFAD and 0x0042AFD5 push the ADDRESS of the pair as an append's arg6.
std::uint32_t g_57bce8 = 0U; // 0x0057BCE8
std::uint32_t g_57bcec = 0U; // 0x0057BCEC

// Argument-less getters and event-id sources, each referenced by the name the tree already defines for its
// address (checked by grepping every address before use).
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_4F45F0(void); // 0x004F45F0: `mov ax,[0x54E200]; ret` (parent type; sys/native_4f45f0.cpp)
extern std::uint8_t Script_PhaseIdGetE4_4F1A60();              // 0x004F1A60 (script/native_4f1a60.cpp)
extern std::uint8_t Script_PhaseIdGetE5_4F1A70();              // 0x004F1A70 (script/native_4f1a70.cpp)
extern std::uint8_t NATIVE_DECL Task_GetCachedEventId_4E6610(void); // 0x004E6610: lazy one-shot event-id allocation (core/native_004e6610.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F40D0(void);   // 0x004F40D0 (sys/native_4f40d0.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F40E0(void);   // 0x004F40E0 (sys/native_4f40e0.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F4110(void);   // 0x004F4110 (sys/native_4f4110.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F4120(void);   // 0x004F4120 (sys/native_4f4120.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F4130(void);   // 0x004F4130 (sys/native_4f4130.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetGlobal8_4F4140(void);   // 0x004F4140 (sys/native_4f4140.cpp)
extern std::uint8_t NATIVE_DECL Sys_GetSettingByte_416890(void); // 0x00416890 (config/native_416890.cpp)
extern std::uint8_t NATIVE_DECL DynObj_GetByte_4C5800(void);   // 0x004C5800 (dynobj/native_4c5800.cpp)
extern std::uint8_t QTask_PhaseIdGet548176_4CEA10();           // 0x004CEA10 (script/native_4cea10.cpp)
extern std::uint8_t QTask_PhaseIdGet548177_4CEA20();           // 0x004CEA20 (script/native_4cea20.cpp)

// Argument-less 32-bit getters: each append's fourth argument.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0F50(void); // 0x004F0F50 (sys/native_4f0f50.cpp)
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(); // 0x004F0FA0 (core/native_4f0fa0.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FF0(void); // 0x004F0FF0 (sys/native_4f0ff0.cpp)
extern std::uint32_t Camera_Get_4F1000();                      // 0x004F1000 (render/native_4f1000.cpp)
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void); // 0x004F0FC0 (sys/native_4f0fc0.cpp)

extern void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6); // 0x004F1A80

void NATIVE_DECL Train_Register_42ACC0()
{
    NATIVE_LOG_ENTER("Train_Register_42ACC0", "0x0042ACC0");
    // 0x0042ACC0: sub esp,0x8E0. The frame holds the append destinations; its size and the registered object
    // size are equal by coincidence, not identity, exactly as in 0x00427FF0.
    std::uint8_t object_frame[0x8E0];

    // 0x0042ACC6/0x0042ACC8: the two zeros are surplus for the argument-less parent getter and supply
    // 0x00401900's fourth argument. The parent getter is 0x004F45F0, the fourth parent source.
    const std::uint16_t parent = Sys_GetGlobal16_4F45F0();                    // 0x0042ACCA
    const std::uint16_t class_id =
        TaskType_Register("Train", 0x8E0U, parent, 0U);                       // 0x0042ACDA
    g_57bcb8 = class_id;                                                      // 0x0042ACE7

    // The seventeen links, retail order. Each handler immediate is pre-pushed and is consumed by the link.
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00429F80)); // 0x0042ACED
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0042A100)); // 0x0042AD00
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042A150));       // 0x0042AD1B
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B030));       // 0x0042AD36
    // 0x0042AD41 pushes handler 0x0042B500 and 0x0042AD46 pushes event 7; the call at 0x0042AD49 is 0x00401400.
    QTask_LinkQTaskEventHandler(
        class_id, 7U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B500)); // 0x0042AD49
    // The second 0x00401530 site in the body, at 0x0042AD5D, with handler 0x0042B700 and event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B700)); // 0x0042AD5D
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Task_GetCachedEventId_4E6610()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B220));       // 0x0042AD78
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F40D0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B040));       // 0x0042AD92
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F40E0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B2C0));       // 0x0042ADAD
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F4110()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x004EFF20));       // 0x0042ADC8
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F4120()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B240));       // 0x0042ADE2
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F4130()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B280));       // 0x0042ADFD
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetGlobal8_4F4140()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B2F0));       // 0x0042AE18
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Sys_GetSettingByte_416890()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042B930));       // 0x0042AE32
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(DynObj_GetByte_4C5800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042AC20));       // 0x0042AE4D
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042BBA0));       // 0x0042AE68
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x0042BB60));       // 0x0042AE82

    // 0x0042AE87: push 0xA; 0x0042AE89: call 0x004F1950 - a TEN-entry container, stored at 0x0057BCBC.
    g_57bcbc = static_cast<std::uint32_t>(
        reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(10)));
    g_57bce8 = 0xC0A00000U;                                                   // 0x0042AE96
    g_57bcec = 0x40A00000U;                                                   // 0x0042AEA0

    // The ten appends. arg2 is the frame base at every site; arg3 is the per-site destination offset, derived
    // from that site's own `lea` with esp replayed at the instruction. The offsets are retail order, unsorted.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AEC0: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x76C)), // 0x0042AEB3: lea ecx,[esp+0x774]
        Sys_GetGlobal32_4F0F50(), 7, 0);                                                  // 0x0042AEC7
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AEE5: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x758)), // 0x0042AED8: lea ecx,[esp+0x760]
        Sys_GetGlobal32_4F0F50(), 7, 0);                                                  // 0x0042AEEC
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AF0A: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x77C)), // 0x0042AEFD: lea ecx,[esp+0x784]
        Core_GetRenderWordA_4F0FA0(), 7, 0);                                              // 0x0042AF11
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AF2F: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x780)), // 0x0042AF22: lea ecx,[esp+0x788]
        Sys_GetGlobal32_4F0FF0(), 7, 0);                                                  // 0x0042AF36
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AF54: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x750)), // 0x0042AF47: lea ecx,[esp+0x758]
        Sys_GetGlobal32_4F0F50(), 7, 0);                                                  // 0x0042AF5B
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AF79: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x754)), // 0x0042AF6C: lea ecx,[esp+0x75C]
        Sys_GetGlobal32_4F0F50(), 7, 0);                                                  // 0x0042AF80
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AF9E: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x880)), // 0x0042AF91: lea ecx,[esp+0x888]
        Camera_Get_4F1000(), 7, 0);                                                       // 0x0042AFA5
    // 0x0042AFAD: `push 0x0057BCE8` - arg6 is the ADDRESS of the float pair, not 0.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AFC6: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x770)), // 0x0042AFB9: lea ecx,[esp+0x778]
        Sys_GetGlobal32_4F0F50(), 7,
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&g_57bce8)));          // 0x0042AFCD
    // 0x0042AFD5: `push 0x0057BCE8` again, with the same handler-style address as arg6.
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042AFEE: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x774)), // 0x0042AFE1: lea ecx,[esp+0x77C]
        Sys_GetGlobal32_4F0F50(), 7,
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&g_57bce8)));          // 0x0042AFF5
    Script_TaskContainer_Append(
        reinterpret_cast<std::int32_t*>(g_57bcbc),
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x00)), // 0x0042B013: lea edx,[esp+0xC]
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(object_frame + 0x890)), // 0x0042B006: lea ecx,[esp+0x898]
        Sys_GetGlobal32_4F0FC0(), 7, 0);                                                  // 0x0042B01A
    // 0x0042B01F: add esp,0x8F8 - the 0x8E0 frame plus the final append's six dwords. 0x0042B025: ret.
}

} // extern "C"
