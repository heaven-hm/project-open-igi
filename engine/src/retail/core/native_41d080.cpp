// Native: Task_DefineInputBox_41D080 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041D080-0x0041D287 (520 bytes, 1 block, 0 edges, 149 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041D080 (520 bytes, 1 block, 0 edges, 149 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041D287, the bytes 0x0041D288-0x0041D28F are eight 0x90 alignment nops (not part of the routine), and 0x0041D290 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C28 `e8 53 6e 01 00` = `call 0x0041D080`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B644 = "InputBox" (file bytes 49 6e 70 75 74 42 6f 78 00) and `pxw` at 0x0053B640 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBC4 is above the file-backed end of .data and holds image value 0. A stack replay of all 149 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041D0B2 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041D0F4 (links 2, 3 and 4, three pushes each), 0xC at each of 0x0041D10E, 0x0041D129, 0x0041D144, 0x0041D15E and 0x0041D179 (one link of three pushes each), 0x10 at 0x0041D19B (the last link plus the container count), 0x18 at each of 0x0041D1C3, 0x0041D1E9, 0x0041D20F, 0x0041D235 and 0x0041D25B (appends 1-5, six pushes each) and 0x1C0 at 0x0041D281 (the 0x1A8 frame plus append 6's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F1000 78, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x7, 0x004F1A80 x6, 0x00401400 x3, 0x004F0FC0 x3, 0x004F0FA0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F1000, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 32 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x1A8-byte frame and no saved register. Reads the parent type through 0x00424850, registers "InputBox" with object size 0x1A8 through 0x00401900 and stores the returned 16-bit id at 0x0053B640. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041D450, the event word from 0x004E7530 -> 0x0041D720, event 2 -> 0x0041D330, the word from 0x00424870 -> 0x0041D9E0, the word from 0x00424880 -> 0x0041DA90, the word from 0x004F1210 -> 0x0041DB30 and the word from 0x004F1220 -> 0x0041DB90; through the context link 0x00401400 - event 0 -> 0x0041D290 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041D9D0 and the byte from 0x004F1A70 -> 0x0041D3A0. Then allocates a SIX-entry parameter container through 0x004F1950 into 0x0057BBC4 and makes six six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x28, frame+0x1A4, frame+0xA4, frame+0xF4, frame+0x144 and frame+0x196, with the argument-less loads 0x004F0FA0 (first and second), 0x004F0FC0 (third, fourth and fifth) and 0x004F1000 (sixth) as their fourth argument, all six with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the ten handler addresses 0x0041D450, 0x0041D720, 0x0041D330, 0x0041D290, 0x0041D9D0, 0x0041D3A0, 0x0041D9E0, 0x0041DA90, 0x0041DB30 and 0x0041DB90 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F1210/0x004F1220 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1000 in render/native_4f1000.cpp, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. There is no stack probe here: the frame is 0x1A8, far below the 4KB page threshold, so the prologue is a plain `sub esp,0x1A8` and 0x004A54A0 has no call site in this body (contrast core/native_41aaf0.cpp and core/native_41b8e0.cpp, whose 0x2244 and 0x13FC frames do probe). THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL SIX APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041D1A3, 0x0041D1C6, 0x0041D1EC, 0x0041D212, 0x0041D238, 0x0041D25E and 7 at 0x0041D1A5, 0x0041D1C8, 0x0041D1EE, 0x0041D214, 0x0041D23A, 0x0041D260) - two immediates, not a slot address - so, as in core/native_41aaf0.cpp, core/native_41c700.cpp and core/native_41b8e0.cpp and unlike core/native_4199f0.cpp, this unit has no load-bearing pointer in arg6; each site's order is re-derived from its own push sequence. THE `lea` ORDERING MATCHES core/native_41b8e0.cpp, NOT core/native_41c700.cpp: the handler result is pushed first (`push %eax` at 0x0041D1AC, 0x0041D1D5, 0x0041D1FB, 0x0041D221, 0x0041D247, 0x0041D26D) and the two frame pointers are computed afterwards at esp = frame-0x0C, which is why the high deltas are 0x1A4/0xA4/0xF4/0x144/0x196 rather than the +0xC values an earlier unit's arithmetic would predict. The placement of `mov %edx,[0x57BBC4]` varies between sites (after the high `lea` in append 1, before the handler push in appends 2-6) and does not touch esp; it is reproduced in retail order rather than normalised. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004F1000, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The six append addresses are retail frame addresses and the callee stores only their differences (0x28, 0x1A4, 0xA4, 0xF4, 0x144 and 0x196), so the transcription passes equivalent addresses inside a local 0x1A8 scratch range, which is never read; the order is not a progression and the deltas are measured per site. 0x0053B640 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041D0A7 `mov %ax`, 0x0041D0C0 `mov %ax`, 0x0041D0CC `mov %cx`, 0x0041D0E0 `mov %dx`, 0x0041D102 `mov %ax`, 0x0041D11B `mov %cx`, 0x0041D136 `mov %dx`, 0x0041D152 `mov %ax`, 0x0041D16B `mov %cx`, 0x0041D186 `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B644 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41d080.o libdeps_41d080.a` exit 0, all 16 symbols this file references defined exactly once in the merged object, every one of them by a tree native (no toolchain symbol is needed here, because the 0x1A8 frame does not reach the probe threshold - contrast core/native_41aaf0.cpp and core/native_41b8e0.cpp); archive /tmp/q218/link/libdeps_41d080.a size 58688 bytes mtime 2026-10-25T15:23:04+05:30; object sha256 1edd3c5048b81fdeec629cafec561daeee464ebb61d6c83f4972d233fa003ad8; source sha256 a5c1c179e9cdcd2967346a564bd0a2f69422e05acfbce2a3ad54c99c4ec5f0ee, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41d080.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 08bc8b42c060daad53cac33bdf1f3dd691d124e232b649f31e9aada96f2cc1ff, size 14474624 bytes, mtime 2026-10-25T15:38:24, which POSTDATES this file and CONTAINS it: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineInputBox_41D080$'` = 1, measured, and this file is present in the build target's source list. This sentence previously recorded 0 against binary c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d (size 14453619 bytes, mtime 2026-10-25T15:08:24), which predated this file and did not contain it; a count of 0 from a binary the file was not in measures the build, not the file. Re-measured and corrected 2026-10-25 against the hash named above. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_41d080.cpp` = `linkable (16 undefined, all defined in tree)`.
#include "dev_log.hpp"
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

// 0x0053B640: the "InputBox" class id word this registrar writes (0x0041D0A7). Image value 0x0181 (file bytes 81 01 at
// 0x0053B640, immediately before the name literal at 0x0053B644); no other translation unit owns it.
std::uint16_t g_53b640 = 0x0181U;

// 0x0057BBC4: the parameter container pointer this body writes (0x0041D19E). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbc4 = 0U;

void NATIVE_DECL Task_DefineInputBox_41D080()
{
    NATIVE_LOG_ENTER("Task_DefineInputBox_41D080", "0x0041D080");
    // 0x0041D080: `sub $0x1A8,%esp`. The frame is the range the six appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0x1a8];

    // 0x0041D086/0x0041D088: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041D08F-0x0041D0AD: `push %eax; push $0x1A8; push $0x0053B644; call 0x00401900; mov %ax,0x0053B640`. The name bytes at
    // 0x0053B644 are 49 6e 70 75 74 42 6f 78 00 = "InputBox", and the registered size 0x1A8 is this body's frame.
    g_53b640 = TaskType_Register("InputBox", 0x1a8U, parent, 0U);

    // 0x0041D09F/0x0041D0A4/0x0041D0A6: `push $0x0041D450; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D450U)));

    // 0x0041D0B5/0x0041D0BA: `push $0x0041D720; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041D0BF-0x0041D0C7 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D720U)));

    // 0x0041D0D3-0x0041D0DB: `push $0x0041D330; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D330U)));

    // 0x0041D0E7-0x0041D0EF: `push $0x0041D290; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041D290.
    QTask_LinkQTaskEventHandler(g_53b640, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D290U)));

    // 0x0041D0F7/0x0041D0FC: `push $0x0041D9D0; call 0x004F1A60` (argument-less byte getter); 0x0041D101-0x0041D109 links it to
    // 0x0041D9D0 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b640, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D9D0U)));

    // 0x0041D111/0x0041D116: `push $0x0041D3A0; call 0x004F1A70` (the second byte getter); 0x0041D11B-0x0041D124 links it to
    // 0x0041D3A0 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b640, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D3A0U)));

    // 0x0041D12C/0x0041D131: `push $0x0041D9E0; call 0x00424870` (argument-less load); 0x0041D136-0x0041D13F links it to
    // 0x0041D9E0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D9E0U)));

    // 0x0041D147/0x0041D14C: `push $0x0041DA90; call 0x00424880`; 0x0041D151-0x0041D159 links it to 0x0041DA90 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DA90U)));

    // 0x0041D161/0x0041D166: `push $0x0041DB30; call 0x004F1210`; 0x0041D16B-0x0041D174 links it to 0x0041DB30 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DB30U)));

    // 0x0041D17C/0x0041D181: `push $0x0041DB90; call 0x004F1220`; 0x0041D186-0x0041D18F links it to 0x0041DB90 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b640, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DB90U)));

    // 0x0041D194/0x0041D196/0x0041D19E: `push $0x6; call 0x004F1950; mov %eax,0x0057BBC4` - a SIX-entry container, matching the
    // six appends below.
    g_57bbc4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(6)));

    // The six appends address this body's frame: retail passes frame+0x00 and the six offsets below and 0x004F1A80 stores only
    // their differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041D1A3/0x0041D1A5: `push $0x0; push $0x7; call 0x004F0FA0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041D1AC: `push %eax` (the
    // handler result, pushed BEFORE the pointer computations - this body's ordering, as in core/native_41b8e0.cpp);
    // 0x0041D1AD: `lea %eax,[%esp+0x34]` at esp = frame-0x0C, i.e. frame+0x28; 0x0041D1B7: `lea %ecx,[%esp+0xC]` = frame+0x00;
    // 0x0041D1BE: call 0x004F1A80; 0x0041D1C3: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 2: 0x0041D1C6/0x0041D1C8: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041D1D5: `push %eax`; 0x0041D1D6:
    // `lea %eax,[%esp+0x1B0]` at esp = frame-0x0C, i.e. frame+0x1A4; 0x0041D1DD: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0x1A4,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 3: 0x0041D1EC/0x0041D1EE: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041D1FB: `push %eax`; 0x0041D1FC:
    // `lea %eax,[%esp+0xB0]` = frame+0xA4; 0x0041D203: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0xA4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 4: 0x0041D212/0x0041D214: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041D221: `push %eax`; 0x0041D222:
    // `lea %eax,[%esp+0x100]` = frame+0xF4; 0x0041D229: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0xF4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x0041D238/0x0041D23A: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041D247: `push %eax`; 0x0041D248:
    // `lea %eax,[%esp+0x150]` = frame+0x144; 0x0041D24F: `lea %ecx,[%esp+0xC]` = frame+0x00.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0x144,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 6: 0x0041D25E/0x0041D260: `push $0x0; push $0x7; call 0x004F1000` (the camera dword, not 0x004F0FC0);
    // 0x0041D26D: `push %eax`; 0x0041D26E: `lea %eax,[%esp+0x1A2]` = frame+0x196; 0x0041D275: `lea %ecx,[%esp+0xC]` = frame+0x00;
    // 0x0041D27C: call 0x004F1A80. This append has no cleanup of its own: 0x0041D281 `add $0x1C0,%esp` covers the 0x1A8 frame plus
    // its six pushes, and 0x0041D287 returns with EAX still holding the append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbc4)),
                                parameter_base, parameter_base + 0x196,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);
}

} // extern "C"
