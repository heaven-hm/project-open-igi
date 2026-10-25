// Native: Task_DefineListBox_41E690 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041E690-0x0041E8BD (558 bytes, 1 block, 0 edges, 161 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041E690 (558 bytes, 1 block, 0 edges, 161 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction, and the 34 call instructions below are counted from the ndisasm instruction boundaries, not from a byte scan. The ret is at 0x0041E8BD, the bytes 0x0041E8BE-0x0041E8BF are two 0x90 alignment nops (not part of the routine), and 0x0041E8C0 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C2D `e8 5e 6e 01 00` = `call 0x0041E690`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B698 = "ListBox" (file bytes 4c 69 73 74 42 6f 78 00) and `pxw` at 0x0053B694 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBDC is above the file-backed end of .data and holds image value 0. INSTRUMENT NOTE: a byte-level E8 scan of this extent reports a seventeenth callee at 0x00ABE73F, which does not exist. The byte at 0x0041E6F9 is the second byte of the immediate of `68 C0 E8 41 00` (`push 0x0041E8C0`, the event-0 handler) and was decoded as a call opcode: 0x0041E6F9 + 5 + rel32 = 0x00ABE73F. It is recorded here because the contamination is systematic - any handler address containing an 0xE8 byte manufactures a phantom callee - and the real count of 34 is the instruction-boundary count. A stack replay of all 161 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041E6C2 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041E704 (links 2, 3 and 4, three pushes each), 0xC at each of 0x0041E71E, 0x0041E739, 0x0041E754, 0x0041E76E and 0x0041E789 (one link of three pushes each), 0x10 at 0x0041E7AB (the last link plus the container count), 0x18 at each of 0x0041E7D3, 0x0041E7F9, 0x0041E81F, 0x0041E845, 0x0041E86B and 0x0041E891 (appends 1-6, six pushes each) and 0xD50 at 0x0041E8B7 (the 0xD38 frame plus append 7's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F0FE0 32, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x7, 0x004F1A80 x7, 0x004F0FC0 x4, 0x00401400 x3, 0x004F0FE0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F0FA0, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 34 call instructions in all.
// Behavior/CFG: one straight-line block, a 0xD38-byte frame and no saved register. Reads the parent type through 0x00424850, registers "ListBox" with object size 0xD38 through 0x00401900 and stores the returned 16-bit id at 0x0053B694. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041EC70, the event word from 0x004E7530 -> 0x0041ED70, event 2 -> 0x0041E9D0, the word from 0x00424870 -> 0x0041F030, the word from 0x00424880 -> 0x0041F140, the word from 0x004F1210 -> 0x0041F1E0 and the word from 0x004F1220 -> 0x0041F240; through the context link 0x00401400 - event 0 -> 0x0041E8C0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041F020 and the byte from 0x004F1A70 -> 0x0041EA70. Then allocates a SEVEN-entry parameter container through 0x004F1950 into 0x0057BBDC and makes seven six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x54, frame+0x94, frame+0xBEC, frame+0xBF8, frame+0xC48, frame+0xC98 and frame+0xCE8, with the argument-less loads 0x004F0FE0 (first and second), 0x004F0FA0 (third) and 0x004F0FC0 (fourth to seventh) as their fourth argument, all seven with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the ten handler addresses 0x0041EC70, 0x0041ED70, 0x0041E9D0, 0x0041E8C0, 0x0041F020, 0x0041EA70, 0x0041F030, 0x0041F140, 0x0041F1E0 and 0x0041F240 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F0FE0/0x004F1210/0x004F1220 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. There is no stack probe here: the frame is 0xD38 = 3384, below the 4096-byte page threshold, so the prologue is a plain `sub esp,0xD38` and 0x004A54A0 has no call site in this body - the frame is the largest in my band that does NOT probe, which is the boundary case that fixes the threshold from below (0x13FC probes, 0xD38 does not). THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL SEVEN APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041E7B3, 0x0041E7D6, 0x0041E7FC, 0x0041E822, 0x0041E848, 0x0041E86E, 0x0041E894 and 7 at 0x0041E7B5, 0x0041E7D8, 0x0041E7FE, 0x0041E824, 0x0041E84A, 0x0041E870, 0x0041E896) - two immediates, not a slot address - so, as in core/native_41aaf0.cpp, core/native_41c700.cpp, core/native_41b8e0.cpp and core/native_41d080.cpp and unlike core/native_4199f0.cpp, this unit has no load-bearing pointer in arg6; each site's order is re-derived from its own push sequence. THE `lea` ORDERING MATCHES core/native_41b8e0.cpp AND core/native_41d080.cpp: the handler result is pushed first (0x0041E7BC, 0x0041E7E5, 0x0041E80B, 0x0041E831, 0x0041E857, 0x0041E87D, 0x0041E8A3) and both frame pointers are computed afterwards at esp = frame-0x0C. The seven deltas are 0x54, 0x94, 0xBEC, 0xBF8, 0xC48, 0xC98 and 0xCE8: the last five rise by exactly 0x50 each, which is the first near-progression in this band, but the first two rise by 0x40 and the step from 0x94 to 0xBEC is not part of it, so the values are still taken per site rather than extrapolated. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004F0FE0, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The seven append addresses are retail frame addresses and the callee stores only their differences, so the transcription passes equivalent addresses inside a local 0xD38 scratch range, which is never read. 0x0053B694 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041E6B7 `mov %ax`, 0x0041E6D0 `mov %ax`, 0x0041E6DC `mov %cx`, 0x0041E6F0 `mov %dx`, 0x0041E712 `mov %ax`, 0x0041E72B `mov %cx`, 0x0041E746 `mov %dx`, 0x0041E762 `mov %ax`, 0x0041E77B `mov %cx`, 0x0041E796 `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B698 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41e690.o libdeps_41e690.a` exit 0, all 16 symbols this file references defined exactly once in the merged object, every one of them by a tree native (no toolchain symbol is needed here, because the 0xD38 frame is below the probe threshold - it is the largest frame in this band that does not probe, against core/native_41b8e0.cpp's 0x13FC which does); archive /tmp/q218/link/libdeps_41e690.a size 58696 bytes mtime 2026-10-25T15:25:14+05:30; object sha256 7ac4ef7792b5265f1bdba103c99688246843f4a672362703554c94f390182a53; source sha256 718bd25e09cbba4e90549e33fecbc515015d0e156d69108caa2245b1c255e10d, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41e690.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 08bc8b42c060daad53cac33bdf1f3dd691d124e232b649f31e9aada96f2cc1ff, size 14474624 bytes, mtime 2026-10-25T15:38:24, which POSTDATES this file and CONTAINS it: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineListBox_41E690$'` = 1, measured, and this file is present in the build target's source list. This sentence previously recorded 0 against binary c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d (size 14453619 bytes, mtime 2026-10-25T15:08:24), which predated this file and did not contain it; a count of 0 from a binary the file was not in measures the build, not the file. Re-measured and corrected 2026-10-25 against the hash named above. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_41e690.cpp` = `linkable (16 undefined, all defined in tree)`.
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

// 0x0053B694: the "ListBox" class id word this registrar writes (0x0041E6B7). Image value 0x0181 (file bytes 81 01 at
// 0x0053B694, immediately before the name literal at 0x0053B698); no other translation unit owns it.
std::uint16_t g_53b694 = 0x0181U;

// 0x0057BBDC: the parameter container pointer this body writes (0x0041E7AE). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbdc = 0U;

void NATIVE_DECL Task_DefineListBox_41E690()
{
    // 0x0041E690: `sub $0xD38,%esp`. The frame is the range the seven appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0xd38];

    // 0x0041E696/0x0041E698: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041E69F-0x0041E6BD: `push %eax; push $0xD38; push $0x0053B698; call 0x00401900; mov %ax,0x0053B694`. The name bytes at
    // 0x0053B698 are 4c 69 73 74 42 6f 78 00 = "ListBox", and the registered size 0xD38 is this body's frame.
    g_53b694 = TaskType_Register("ListBox", 0xd38U, parent, 0U);

    // 0x0041E6AF/0x0041E6B4/0x0041E6B6: `push $0x0041EC70; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041EC70U)));

    // 0x0041E6C5/0x0041E6CA: `push $0x0041ED70; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041E6CF-0x0041E6D7 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041ED70U)));

    // 0x0041E6E3-0x0041E6EB: `push $0x0041E9D0; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E9D0U)));

    // 0x0041E6F7-0x0041E6FF: `push $0x0041E8C0; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041E8C0. This push's immediate is the source of the phantom
    // callee documented in the provenance block (its second byte is 0xE8).
    QTask_LinkQTaskEventHandler(g_53b694, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E8C0U)));

    // 0x0041E707/0x0041E70C: `push $0x0041F020; call 0x004F1A60` (argument-less byte getter); 0x0041E711-0x0041E719 links it to
    // 0x0041F020 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b694, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F020U)));

    // 0x0041E721/0x0041E726: `push $0x0041EA70; call 0x004F1A70` (the second byte getter); 0x0041E72B-0x0041E734 links it to
    // 0x0041EA70 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b694, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041EA70U)));

    // 0x0041E73C/0x0041E741: `push $0x0041F030; call 0x00424870` (argument-less load); 0x0041E746-0x0041E74F links it to
    // 0x0041F030 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F030U)));

    // 0x0041E757/0x0041E75C: `push $0x0041F140; call 0x00424880`; 0x0041E761-0x0041E769 links it to 0x0041F140 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F140U)));

    // 0x0041E771/0x0041E776: `push $0x0041F1E0; call 0x004F1210`; 0x0041E77B-0x0041E784 links it to 0x0041F1E0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F1E0U)));

    // 0x0041E78C/0x0041E791: `push $0x0041F240; call 0x004F1220`; 0x0041E796-0x0041E79F links it to 0x0041F240 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b694, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041F240U)));

    // 0x0041E7A4/0x0041E7A6/0x0041E7AE: `push $0x7; call 0x004F1950; mov %eax,0x0057BBDC` - a SEVEN-entry container, matching the
    // seven appends below.
    g_57bbdc = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(7)));

    // The seven appends address this body's frame: retail passes frame+0x00 and the seven offsets below and 0x004F1A80 stores
    // only their differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041E7B3/0x0041E7B5: `push $0x0; push $0x7; call 0x004F0FE0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041E7BC: `push %eax` (the
    // handler result, pushed BEFORE the pointer computations - this body's ordering); 0x0041E7BD: `lea %eax,[%esp+0x60]` at
    // esp = frame-0x0C, i.e. frame+0x54; 0x0041E7C7: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E7CE: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0x54,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 2: 0x0041E7D6/0x0041E7D8: `push $0x0; push $0x7; call 0x004F0FE0`; 0x0041E7E6: `lea %eax,[%esp+0xA0]` at
    // esp = frame-0x0C, i.e. frame+0x94; 0x0041E7ED: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E7F4: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 3: 0x0041E7FC/0x0041E7FE: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041E80C: `lea %eax,[%esp+0xBF8]` = frame+0xBEC;
    // 0x0041E813: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E81A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0xBEC,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 4: 0x0041E822/0x0041E824: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041E832: `lea %eax,[%esp+0xC04]` = frame+0xBF8;
    // 0x0041E839: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E840: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0xBF8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x0041E848/0x0041E84A: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041E858: `lea %eax,[%esp+0xC54]` = frame+0xC48;
    // 0x0041E85F: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E866: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0xC48,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 6: 0x0041E86E/0x0041E870: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041E87E: `lea %eax,[%esp+0xCA4]` = frame+0xC98;
    // 0x0041E885: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E88C: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0xC98,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 7: 0x0041E894/0x0041E896: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041E8A4: `lea %eax,[%esp+0xCF4]` = frame+0xCE8;
    // 0x0041E8AB: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041E8B2: call 0x004F1A80. This append has no cleanup of its own:
    // 0x0041E8B7 `add $0xD50,%esp` covers the 0xD38 frame plus its six pushes, and 0x0041E8BD returns with EAX still holding the
    // append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbdc)),
                                parameter_base, parameter_base + 0xCE8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
