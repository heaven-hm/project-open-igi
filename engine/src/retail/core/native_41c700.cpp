// Native: Task_DefineAsciiBox_41C700 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041C700-0x0041C8CB (460 bytes, 1 block, 0 edges, 132 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041C700 (460 bytes, 1 block, 0 edges, 132 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041C8CB, the bytes 0x0041C8CC-0x0041C8CF are four 0x90 alignment nops (not part of the routine), and 0x0041C8D0 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C0A `e8 f1 7a 01 00` = `call 0x0041C700`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B5F8 = "AsciiBox" (file bytes 41 73 63 69 69 42 6f 78 00) and `pxw` at 0x0053B5F4 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBBC is above the file-backed end of .data and holds image value 0. A stack replay of all 132 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041C732 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041C774 (three links of three pushes each), 0xC at each of 0x0041C78E, 0x0041C7A9, 0x0041C7C4, 0x0041C7DE, 0x0041C7F9 and 0x0041C814 (one link of three pushes each), 0x10 at 0x0041C835 (the last link plus the container count), 0x18 at each of 0x0041C85C, 0x0041C881 and 0x0041C8A3 (appends 1-3, six pushes each) and 0x110 at 0x0041C8C5 (the 0xF8 frame plus append 4's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x00424890 10, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FE0 32, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x8, 0x004F1A80 x4, 0x00401400 x3, 0x004F0FA0 x2, 0x004F0FE0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x00424890, 0x004E7530, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 30 call instructions in all.
// Behavior/CFG: one straight-line block, a 0xF8-byte frame and no saved register. Reads the parent type through 0x00424850, registers "AsciiBox" with object size 0xF8 through 0x00401900 and stores the returned 16-bit id at 0x0053B5F4. Then makes ELEVEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041CD20, the event word from 0x004E7530 -> 0x0041CDF0, event 2 -> 0x0041C940, the word from 0x00424870 -> 0x00424300, the word from 0x00424880 -> 0x0041A120, the word from 0x004F1210 -> 0x0041D010 and the word from 0x004F1220 -> 0x0041D030, the word from 0x00424890 -> 0x0041C9D0; through the context link 0x00401400 - event 0 -> 0x0041C8D0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041D000 and the byte from 0x004F1A70 -> 0x0041CC40. Then allocates a FOUR-entry parameter container through 0x004F1950 into 0x0057BBBC and makes four six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x54, frame+0x94, frame+0x28 and frame+0x2C, the first two with the argument-less load 0x004F0FE0 as their fourth argument and the last two with 0x004F0FA0, all four with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the eleven handler addresses 0x0041CD20, 0x0041CDF0, 0x0041C940, 0x0041C8D0, 0x0041D000, 0x0041CC40, 0x00424300, 0x0041A120, 0x0041D010, 0x0041D030 and 0x0041C9D0 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x00424890/0x004F1210/0x004F1220/0x004F0FE0 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. There is no stack probe here: the frame is 0xF8, far below the 4KB page threshold, so the prologue is a plain `sub esp,0xF8` and 0x004A54A0 has no call site in this body (contrast core/native_41aaf0.cpp, whose 0x2244 frame does probe). THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL FOUR APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041C83D, 0x0041C85F, 0x0041C884, 0x0041C8A6 and 7 at 0x0041C83F, 0x0041C861, 0x0041C886, 0x0041C8A8) - two immediates, not a slot address - so, as in core/native_41aaf0.cpp and unlike core/native_4199f0.cpp, this unit has no load-bearing pointer in arg6; the order is re-derived from each call's own push sequence rather than carried over from an earlier unit. The getters 0x00424870, 0x00424880, 0x00424890, 0x004F0FA0, 0x004F0FE0, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The four append addresses are retail frame addresses and the callee stores only their differences (0x54, 0x94, 0x28 and 0x2C), so the transcription passes equivalent addresses inside a local 0xF8 scratch range, which is never read; the two high offsets 0x54 and 0x94 are measured per site rather than derived from a pattern. 0x0053B5F4 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041C727 `mov %ax`, 0x0041C740 `mov %ax`, 0x0041C74C `mov %cx`, 0x0041C760 `mov %dx`, 0x0041C782 `mov %ax`, 0x0041C79B `mov %cx`, 0x0041C7B6 `mov %dx`, 0x0041C7D2 `mov %ax`, 0x0041C7EB `mov %cx`, 0x0041C806 `mov %dx`, 0x0041C822 `mov %ax`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B5F8 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41c700.o libdeps_41c700.a` exit 0, all 16 symbols this file references defined exactly once in the merged object, every one of them by a tree native (no toolchain symbol is needed here, because the 0xF8 frame does not reach the probe threshold - contrast core/native_41aaf0.cpp); archive /tmp/q218/link/libdeps_41c700.a size 58696 bytes mtime 2026-10-25T15:17:03+05:30; object sha256 877e2b2522aa22709a448487b9370d8a427d04d4707e47ff9469974e8f56853d; source sha256 0e4b6b400f8056d5873d1407af642ef3bf44979f90934659c76b746646e46409, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41c700.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 08bc8b42c060daad53cac33bdf1f3dd691d124e232b649f31e9aada96f2cc1ff, size 14474624 bytes, mtime 2026-10-25T15:38:24, which POSTDATES this file and CONTAINS it: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineAsciiBox_41C700$'` = 1, measured, and this file is present in the build target's source list. This sentence previously recorded 0 against binary c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d (size 14453619 bytes, mtime 2026-10-25T15:08:24), which predated this file and did not contain it; a count of 0 from a binary the file was not in measures the build, not the file. Re-measured and corrected 2026-10-25 against the hash named above.
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

// 0x00424870 / 0x00424880 / 0x00424890: argument-less 32-bit loads (`[0x53B914]`, `[0x53B918]`, `[0x53B91C]`), transcribed in
// engine/src/retail/sys/native_424870.cpp, native_424880.cpp and native_424890.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424870(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424880(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424890(void);

// 0x004F0FA0: `mov eax,[0xA70C7C]; ret`, transcribed in engine/src/retail/core/native_4f0fa0.cpp as Core_GetRenderWordA_4F0FA0.
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);

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

// 0x0053B5F4: the "AsciiBox" class id word this registrar writes (0x0041C727). Image value 0x0181 (file bytes 81 01 at
// 0x0053B5F4, immediately before the name literal at 0x0053B5F8); no other translation unit owns it.
std::uint16_t g_53b5f4 = 0x0181U;

// 0x0057BBBC: the parameter container pointer this body writes (0x0041C838). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbbc = 0U;

void NATIVE_DECL Task_DefineAsciiBox_41C700()
{
    // 0x0041C700: `sub $0xF8,%esp`. The frame is the range the four appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0xf8];

    // 0x0041C706/0x0041C708: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041C70F-0x0041C72D: `push %eax; push $0xF8; push $0x0053B5F8; call 0x00401900; mov %ax,0x0053B5F4`. The name bytes at
    // 0x0053B5F8 are 41 73 63 69 69 42 6f 78 00 = "AsciiBox", and the registered size 0xF8 is this body's frame.
    g_53b5f4 = TaskType_Register("AsciiBox", 0xf8U, parent, 0U);

    // 0x0041C71F/0x0041C724/0x0041C726: `push $0x0041CD20; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041CD20U)));

    // 0x0041C735/0x0041C73A: `push $0x0041CDF0; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041C73F-0x0041C747 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041CDF0U)));

    // 0x0041C753-0x0041C75B: `push $0x0041C940; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C940U)));

    // 0x0041C767-0x0041C76F: `push $0x0041C8D0; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041C8D0.
    QTask_LinkQTaskEventHandler(g_53b5f4, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C8D0U)));

    // 0x0041C777/0x0041C77C: `push $0x0041D000; call 0x004F1A60` (argument-less byte getter); 0x0041C781-0x0041C789 links it to
    // 0x0041D000 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b5f4, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D000U)));

    // 0x0041C791/0x0041C796: `push $0x0041CC40; call 0x004F1A70` (the second byte getter); 0x0041C79B-0x0041C7A4 links it to
    // 0x0041CC40 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b5f4, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041CC40U)));

    // 0x0041C7AC/0x0041C7B1: `push $0x00424300; call 0x00424870` (argument-less load); 0x0041C7B6-0x0041C7BF links it to
    // 0x00424300 through 0x00401530 (the same handler word core/native_4199f0.cpp links to this getter's value).
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00424300U)));

    // 0x0041C7C7/0x0041C7CC: `push $0x0041A120; call 0x00424880`; 0x0041C7D1-0x0041C7D9 links it to 0x0041A120 through 0x00401530
    // (the fifth unit to link this handler to this getter's value).
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041A120U)));

    // 0x0041C7E1/0x0041C7E6: `push $0x0041D010; call 0x004F1210`; 0x0041C7EB-0x0041C7F4 links it to 0x0041D010 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D010U)));

    // 0x0041C7FC/0x0041C801: `push $0x0041D030; call 0x004F1220`; 0x0041C806-0x0041C80F links it to 0x0041D030 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041D030U)));

    // 0x0041C817/0x0041C81C: `push $0x0041C9D0; call 0x00424890`; 0x0041C821-0x0041C829 links it to 0x0041C9D0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b5f4, Sys_GetGlobal32_424890(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C9D0U)));

    // 0x0041C82E/0x0041C830/0x0041C838: `push $0x4; call 0x004F1950; mov %eax,0x0057BBBC` - a FOUR-entry container, matching the
    // four appends below.
    g_57bbbc = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(4)));

    // The four appends address this body's frame: retail passes frame+0x00 and frame+0x54/0x94/0x28/0x2C and 0x004F1A80 stores
    // only the differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041C83D/0x0041C83F: `push $0x0; push $0x7; call 0x004F0FE0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041C846: `lea 0x5C(%esp)` =
    // frame+0x54; 0x0041C850: `lea 0xC(%esp)` = frame+0x00; 0x0041C857: call 0x004F1A80; 0x0041C85C: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbbc)),
                                parameter_base, parameter_base + 0x54,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 2: 0x0041C85F/0x0041C861: `push $0x0; push $0x7; call 0x004F0FE0`; 0x0041C868: `lea 0x9C(%esp)` = frame+0x94;
    // 0x0041C875: `lea 0xC(%esp)` = frame+0x00; 0x0041C87C: call 0x004F1A80; 0x0041C881: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbbc)),
                                parameter_base, parameter_base + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 3: 0x0041C884/0x0041C886: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041C88D: `lea 0x30(%esp)` = frame+0x28;
    // 0x0041C897: `lea 0xC(%esp)` = frame+0x00; 0x0041C89E: call 0x004F1A80; 0x0041C8A3: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbbc)),
                                parameter_base, parameter_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 4: 0x0041C8A6/0x0041C8A8: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041C8AF: `lea 0x34(%esp)` = frame+0x2C;
    // 0x0041C8B9: `lea 0xC(%esp)` = frame+0x00; 0x0041C8C0: call 0x004F1A80. This append has no cleanup of its own: 0x0041C8C5
    // `add $0x110,%esp` covers the 0xF8 frame plus its six pushes, and 0x0041C8CB returns with EAX still holding the append's
    // return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbbc)),
                                parameter_base, parameter_base + 0x2C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);
}

} // extern "C"
