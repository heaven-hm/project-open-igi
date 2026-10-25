// Native: Task_DefineSlideBar_41F340 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041F340-0x0041F593 (596 bytes, 1 block, 0 edges, 173 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041F340 (596 bytes, 1 block, 0 edges, 173 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction, and the 36 call instructions below are counted from the ndisasm instruction boundaries, not from a byte scan. The ret is at 0x0041F593, the bytes 0x0041F594-0x0041F59F are twelve 0x90 alignment nops (not part of the routine), and 0x0041F5A0 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C32 `e8 09 6d 01 00` = `call 0x0041F340`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B6C0 = "SlideBar" (file bytes 53 6c 69 64 65 42 61 72 00) and `pxw` at 0x0053B6BC = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBE4 is above the file-backed end of .data and holds image value 0. A byte-level E8 scan of this extent reports no phantom callee - unlike core/native_41e690.cpp, whose event-0 handler immediate `68 C0 E8 41 00` contains an 0xE8 - so the instruction-boundary count of 36 and the byte count agree here, and the agreement is itself the check that the byte scan is only unsafe when a handler immediate contains 0xE8. A stack replay of all 173 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041F372 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041F3B4 (links 2, 3 and 4, three pushes each), 0xC at each of 0x0041F3CE, 0x0041F3E9, 0x0041F404, 0x0041F41E and 0x0041F439 (one link of three pushes each), 0x10 at 0x0041F45B (the last link plus the container count), 0x18 at each of 0x0041F483, 0x0041F4A9, 0x0041F4CF, 0x0041F4F5, 0x0041F51B, 0x0041F541 and 0x0041F567 (appends 1-7, six pushes each) and 0x268 at 0x0041F58D (the 0x250 frame plus append 8's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F0FE0 32, 0x004F1000 78, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x004F1A80 x8, 0x00401530 x7, 0x004F0FC0 x5, 0x00401400 x3 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F0FA0, 0x004F0FE0, 0x004F1000, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 36 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x250-byte frame and no saved register. Reads the parent type through 0x00424850, registers "SlideBar" with object size 0x250 through 0x00401900 and stores the returned 16-bit id at 0x0053B6BC. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041FA10, the event word from 0x004E7530 -> 0x0041FCE0, event 2 -> 0x0041F6C0, the word from 0x00424870 -> 0x0041FF30, the word from 0x00424880 -> 0x00420020, the word from 0x004F1210 -> 0x004200C0 and the word from 0x004F1220 -> 0x00420140; through the context link 0x00401400 - event 0 -> 0x0041F5A0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041FF20 and the byte from 0x004F1A70 -> 0x0041F740. Then allocates an EIGHT-entry parameter container through 0x004F1950 into 0x0057BBE4 and makes eight six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x54, frame+0xB4, frame+0xAC, frame+0xB8, frame+0x108, frame+0x158, frame+0x1A8 and frame+0x1F8, with the argument-less loads 0x004F0FE0 (first), 0x004F1000 (second, the camera dword), 0x004F0FA0 (third) and 0x004F0FC0 (fourth to eighth) as their fourth argument, all eight with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the ten handler addresses 0x0041FA10, 0x0041FCE0, 0x0041F6C0, 0x0041F5A0, 0x0041FF20, 0x0041F740, 0x0041FF30, 0x00420020, 0x004200C0 and 0x00420140 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F0FE0/0x004F1210/0x004F1220 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1000 in render/native_4f1000.cpp, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. There is no stack probe here: the frame is 0x250, below the 4096-byte page threshold, so the prologue is a plain `sub esp,0x250` and 0x004A54A0 has no call site in this body. THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL EIGHT APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041F463, 0x0041F486, 0x0041F4AC, 0x0041F4D2, 0x0041F4F8, 0x0041F51E, 0x0041F544, 0x0041F56A and 7 at 0x0041F465, 0x0041F488, 0x0041F4AE, 0x0041F4D4, 0x0041F4FA, 0x0041F520, 0x0041F546, 0x0041F56C) - two immediates, not a slot address - so, as in core/native_41aaf0.cpp, core/native_41c700.cpp, core/native_41b8e0.cpp, core/native_41d080.cpp and core/native_41e690.cpp and unlike core/native_4199f0.cpp, this unit has no load-bearing pointer in arg6; each site's order is re-derived from its own push sequence. THE `lea` ORDERING MATCHES core/native_41b8e0.cpp, core/native_41d080.cpp AND core/native_41e690.cpp: the handler result is pushed first (0x0041F46C, 0x0041F495, 0x0041F4BB, 0x0041F4E1, 0x0041F507, 0x0041F52D, 0x0041F553, 0x0041F579) and both frame pointers are computed afterwards at esp = frame-0x0C, so the ordering tally in this band is now 4-1 in favour of handler-first. THE EIGHT DELTAS ARE 0x54, 0xB4, 0xAC, 0xB8, 0x108, 0x158, 0x1A8 AND 0x1F8, AND THE MIDDLE THREE ARE NOT MONOTONIC: append 3's 0xAC is LOWER than append 2's 0xB4, and append 4's 0xB8 sits between them. The last four rise by exactly 0x50 each, which is the same near-progression core/native_41e690.cpp showed and the same reason not to extrapolate it - here a five-value 0x50 run is contradicted one unit earlier by a run that decreases. The values are taken per site. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004F0FE0, 0x004F1000, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The eight append addresses are retail frame addresses and the callee stores only their differences, so the transcription passes equivalent addresses inside a local 0x250 scratch range, which is never read. 0x0053B6BC is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041F367 `mov %ax`, 0x0041F380 `mov %ax`, 0x0041F38C `mov %cx`, 0x0041F3A0 `mov %dx`, 0x0041F3C2 `mov %ax`, 0x0041F3DB `mov %cx`, 0x0041F3F6 `mov %dx`, 0x0041F412 `mov %ax`, 0x0041F42B `mov %cx`, 0x0041F446 `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B6C0 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41f340.o libdeps_41f340.a` exit 0, all 17 symbols this file references defined exactly once in the merged object, every one of them by a tree native - the 17 are _Camera_Get_4F1000, _Core_GetEventWord_4E7530, _Core_GetRenderWordA_4F0FA0, _QTask_LinkQTaskEventHandler, _QTask_LinkQTaskEventHandlerNoContextFn, _Script_PhaseIdGetE4_4F1A60, _Script_PhaseIdGetE5_4F1A70, _Script_TaskContainer_Alloc, _Script_TaskContainer_Append, _Sys_GetGlobal16_424850, _Sys_GetGlobal32_424870, _Sys_GetGlobal32_424880, _Sys_GetGlobal32_4F0FC0, _Sys_GetGlobal32_4F0FE0, _Sys_GetGlobal32_4F1210, _Sys_GetGlobal32_4F1220 and _TaskType_Register, and no toolchain symbol is needed because the 0x250 frame is far below the probe threshold; archive /tmp/q218/link/libdeps_41f340.a size 59306 bytes mtime 2026-10-25T15:27:46+05:30 (larger than this band's other archives because it carries one more dependency object, render/native_4f1000.o); object sha256 6f76a744648f3aaf6e66af5770726e7089373c669219c30ee2dc187eda521539; source sha256 0e44a21a61dee56f25b0e8eec13815214a12de95b537584f95798930db67a3e0, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41f340.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d, size 14453619 bytes, mtime 2026-10-25T15:08:24, which PREDATES this file: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineSlideBar_41F340$'` = 0, because this file is not yet in the build target's source list. That zero is expected and is not a defect in the file; it must be re-measured to 1 after the source-list addition, and this record updated then. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_41f340.cpp` = `linkable (17 undefined, all defined in tree)`.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00424850: the parent-type getter (`mov ax,[0x53B90C]; ret`), transcribed in engine/src/retail/sys/native_424850.cpp.
extern std::uint16_t NATIVE_DECL Sys_GetGlobal16_424850(void);

// 0x00424870 / 0x00424880: argument-less 32-bit loads (`[0x53B914]`, `[0x53B918]`), transcribed in
// engine/src/retail/sys/native_424870.cpp and native_424880.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424870(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424880(void);

// 0x004F0FA0: `mov eax,[0xA70C7C]; ret`, transcribed in engine/src/retail/core/native_4f0fa0.cpp as Core_GetRenderWordA_4F0FA0.
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);

// 0x004F0FC0: `mov eax,[0xA70C9C]; ret`, transcribed in engine/src/retail/sys/native_4f0fc0.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void);

// 0x004F0FE0: `mov eax,[0xA71888]; ret`, transcribed in engine/src/retail/sys/native_4f0fe0.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FE0(void);

// 0x004F1000: `mov eax,[0xA70C90]; ret`, transcribed in engine/src/retail/render/native_4f1000.cpp as Camera_Get_4F1000.
extern std::uint32_t Camera_Get_4F1000(void);

// 0x004F1210 / 0x004F1220: argument-less 32-bit loads, transcribed in engine/src/retail/sys/native_4f1210.cpp and native_4f1220.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void);

// 0x004E7530: the event word (`mov eax,[0x5488C8]; ret`), transcribed in engine/src/retail/core/native_4e7530.cpp.
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);

// 0x004F1950 / 0x004F1A80: the task parameter container, transcribed in engine/src/retail/script/.
extern void* Script_TaskContainer_Alloc(std::int32_t const count);
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

// 0x0053B6BC: the "SlideBar" class id word this registrar writes (0x0041F367). Image value 0x0181 (file bytes 81 01 at
// 0x0053B6BC, immediately before the name literal at 0x0053B6C0); no other translation unit owns it.
std::uint16_t g_53b6bc = 0x0181U;

// 0x0057BBE4: the parameter container pointer this body writes (0x0041F45E). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbe4 = 0U;

void NATIVE_DECL Task_DefineSlideBar_41F340()
{
    // 0x0041F340: `sub $0x250,%esp`. The frame is the range the eight appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0x250];

    // 0x0041F346/0x0041F348: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041F34F-0x0041F36D: `push %eax; push $0x250; push $0x0053B6C0; call 0x00401900; mov %ax,0x0053B6BC`. The name bytes at
    // 0x0053B6C0 are 53 6c 69 64 65 42 61 72 00 = "SlideBar", and the registered size 0x250 is this body's frame.
    g_53b6bc = TaskType_Register("SlideBar", 0x250U, parent, 0U);

    // 0x0041F35F/0x0041F364/0x0041F366: `push $0x0041FA10; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041FA10U)));

    // 0x0041F375/0x0041F37A: `push $0x0041FCE0; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041F37F-0x0041F387 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041FCE0U)));

    // 0x0041F393-0x0041F39B: `push $0x0041F6C0; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F6C0U)));

    // 0x0041F3A7-0x0041F3AF: `push $0x0041F5A0; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041F5A0.
    QTask_LinkQTaskEventHandler(g_53b6bc, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F5A0U)));

    // 0x0041F3B7/0x0041F3BC: `push $0x0041FF20; call 0x004F1A60` (argument-less byte getter); 0x0041F3C1-0x0041F3C9 links it to
    // 0x0041FF20 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b6bc, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041FF20U)));

    // 0x0041F3D1/0x0041F3D6: `push $0x0041F740; call 0x004F1A70` (the second byte getter); 0x0041F3DB-0x0041F3E4 links it to
    // 0x0041F740 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b6bc, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F740U)));

    // 0x0041F3EC/0x0041F3F1: `push $0x0041FF30; call 0x00424870` (argument-less load); 0x0041F3F6-0x0041F3FF links it to
    // 0x0041FF30 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041FF30U)));

    // 0x0041F407/0x0041F40C: `push $0x00420020; call 0x00424880`; 0x0041F411-0x0041F419 links it to 0x00420020 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00420020U)));

    // 0x0041F421/0x0041F426: `push $0x004200C0; call 0x004F1210`; 0x0041F42B-0x0041F434 links it to 0x004200C0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004200C0U)));

    // 0x0041F43C/0x0041F441: `push $0x00420140; call 0x004F1220`; 0x0041F446-0x0041F44F links it to 0x00420140 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b6bc, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00420140U)));

    // 0x0041F454/0x0041F456/0x0041F45E: `push $0x8; call 0x004F1950; mov %eax,0x0057BBE4` - an EIGHT-entry container, matching
    // the eight appends below.
    g_57bbe4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(8)));

    // The eight appends address this body's frame: retail passes frame+0x00 and the eight offsets below and 0x004F1A80 stores
    // only their differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041F463/0x0041F465: `push $0x0; push $0x7; call 0x004F0FE0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041F46C: `push %eax` (the
    // handler result, pushed BEFORE the pointer computations - this body's ordering); 0x0041F46D: `lea %eax,[%esp+0x60]` at
    // esp = frame-0x0C, i.e. frame+0x54; 0x0041F477: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041F47E: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0x54,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 2: 0x0041F486/0x0041F488: `push $0x0; push $0x7; call 0x004F1000` (the camera dword, not 0x004F0FC0); 0x0041F495:
    // `push %eax`; 0x0041F496: `lea %eax,[%esp+0xC0]` at esp = frame-0x0C, i.e. frame+0xB4; 0x0041F49D: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0xB4,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // Append 3: 0x0041F4AC/0x0041F4AE: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041F4BB: `push %eax`; 0x0041F4BC:
    // `lea %eax,[%esp+0xB8]` = frame+0xAC - LOWER than append 2's 0xB4, so the offsets are not monotonic; 0x0041F4C3:
    // `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0xAC,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 4: 0x0041F4D2/0x0041F4D4: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041F4E1: `push %eax`; 0x0041F4E2:
    // `lea %eax,[%esp+0xC4]` = frame+0xB8; 0x0041F4E9: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0xB8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x0041F4F8/0x0041F4FA: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041F507: `push %eax`; 0x0041F508:
    // `lea %eax,[%esp+0x114]` = frame+0x108; 0x0041F50F: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 6: 0x0041F51E/0x0041F520: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041F52D: `push %eax`; 0x0041F52E:
    // `lea %eax,[%esp+0x164]` = frame+0x158; 0x0041F535: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0x158,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 7: 0x0041F544/0x0041F546: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041F553: `push %eax`; 0x0041F554:
    // `lea %eax,[%esp+0x1B4]` = frame+0x1A8; 0x0041F55B: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0x1A8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 8: 0x0041F56A/0x0041F56C: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041F579: `push %eax`; 0x0041F57A:
    // `lea %eax,[%esp+0x204]` = frame+0x1F8; 0x0041F581: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041F588: call 0x004F1A80. This
    // append has no cleanup of its own: 0x0041F58D `add $0x268,%esp` covers the 0x250 frame plus its six pushes, and
    // 0x0041F593 returns with EAX still holding the append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbe4)),
                                parameter_base, parameter_base + 0x1F8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
