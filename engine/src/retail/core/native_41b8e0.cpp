// Native: Task_DefineScrollListBox_41B8E0 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041B8E0-0x0041BB0E (559 bytes, 1 block, 0 edges, 162 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041B8E0 (559 bytes, 1 block, 0 edges, 162 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041BB0E, 0x0041BB0F is a single 0x90 alignment nop (not part of the routine), and 0x0041BB10 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C05 `e8 d6 7c 01 00` = `call 0x0041B8E0`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B5A0 = "ScrollListBox" (file bytes 53 63 72 6f 6c 6c 4c 69 73 74 42 6f 78 00) and `pxw` at 0x0053B59C = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBB4 is above the file-backed end of .data and holds image value 0. FRAME: 0x0041B8E0 `mov eax,0x13FC` / 0x0041B8E5 `call 0x004A54A0` - the MSVC 6 4KB stack probe, which lowers esp by 0x13FC itself and returns through `push eax; ret`, so the frame base is set inside the probe and there is no `sub esp` here; the probe is a compiler-emitted CRT helper and is never hand-transcribed (precedent: core/native_4170a0.cpp, core/native_4310f0.cpp, and core/native_41aaf0.cpp in this same band). This prologue pushes no callee-saved register, so there is no displacement shift and every `lea reg,[esp+N]` below resolves directly against the probe-set base. A stack replay of all 162 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041B916 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041B958 (links 2, 3 and 4, three pushes each), 0xC at each of 0x0041B972, 0x0041B98D, 0x0041B9A8, 0x0041B9C2 and 0x0041B9DD (one link of three pushes each), 0x10 at 0x0041B9FF (the last link plus the container count), 0x18 at each of 0x0041BA2A, 0x0041BA50, 0x0041BA76, 0x0041BA9C, 0x0041BABF, 0x0041BAE2 and 0x0041BB08 - wait, the last is 0x1414 at 0x0041BB08 (the 0x13FC probe frame plus append 7's six pushes, which has no cleanup of its own) - the 0x18 groups are the first six appends, one per append. Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004A54A0 34, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F0FE0 32, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x7, 0x004F1A80 x7, 0x004F0FC0 x4, 0x00401400 x3, 0x004F0FA0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004A54A0, 0x004E7530, 0x004F0FE0, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 35 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x13FC-byte probed frame and no saved register. Reads the parent type through 0x00424850, registers "ScrollListBox" with object size 0x13FC through 0x00401900 and stores the returned 16-bit id at 0x0053B59C. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041BDD0, the event word from 0x004E7530 -> 0x0041C080, event 2 -> 0x0041BC00, the word from 0x00424870 -> 0x0041C310, the word from 0x00424880 -> 0x0041C500, the word from 0x004F1210 -> 0x0041C5A0 and the word from 0x004F1220 -> 0x0041C600; through the context link 0x00401400 - event 0 -> 0x0041BB10 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041C300 and the byte from 0x004F1A70 -> 0x0041BCA0. Then allocates a SEVEN-entry parameter container through 0x004F1950 into 0x0057BBB4 and makes seven six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x125C, frame+0x12B0, frame+0x1300, frame+0x1350, frame+0x28, frame+0x2C and frame+0x13A0, with the argument-less loads 0x004F0FE0 (first), 0x004F0FC0 (second, third, fourth and seventh) and 0x004F0FA0 (fifth and sixth) as their fourth argument, all seven with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the ten handler addresses 0x0041BDD0, 0x0041C080, 0x0041BC00, 0x0041BB10, 0x0041C300, 0x0041BCA0, 0x0041C310, 0x0041C500, 0x0041C5A0 and 0x0041C600 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every retail callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F0FE0/0x004F1210/0x004F1220 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/); 0x004A54A0 is deliberately NOT declared as a callee, because it is the compiler-emitted MSVC 6 stack probe rather than a retail routine - the C++ frame replaces it, as in core/native_41aaf0.cpp. THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL SEVEN APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041BA07, 0x0041BA2D, 0x0041BA53, 0x0041BA79, 0x0041BA9F, 0x0041BAC2, 0x0041BAE5 and 7 at 0x0041BA09, 0x0041BA2F, 0x0041BA55, 0x0041BA7B, 0x0041BAA1, 0x0041BAC4, 0x0041BAE7) - two immediates, not a slot address - so, as in core/native_41aaf0.cpp and core/native_41c700.cpp and unlike core/native_4199f0.cpp, this unit has no load-bearing pointer in arg6; each site's order is re-derived from its own push sequence. THE `lea` ORDERING HERE IS THE FOURTH DISTINCT SHAPE IN THIS BAND: this body pushes the handler result FIRST and computes both frame pointers afterwards (`push %eax` at 0x0041BA10, then `lea %eax,[%esp+0x1268]` and `lea %ecx,[%esp+0xC]` at esp = frame-0x0C), whereas core/native_41c700.cpp and core/native_4199f0.cpp compute their pointers before the handler push and core/native_41aaf0.cpp's append 2 computes the low pointer after it. Taking any earlier unit's arithmetic as a formula would shift every offset here by 0xC, so each of the seven was resolved from its own esp at its own instruction; the deltas are 0x125C, 0x12B0, 0x1300, 0x1350, 0x28, 0x2C and 0x13A0 and are not a progression. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004F0FE0, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The seven append addresses are retail frame addresses and the callee stores only their differences, so the transcription passes equivalent addresses inside a local 0x13FC scratch range, which is never read. 0x0053B59C is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041B90B `mov %ax`, 0x0041B924 `mov %ax`, 0x0041B930 `mov %cx`, 0x0041B944 `mov %dx`, 0x0041B966 `mov %ax`, 0x0041B97F `mov %cx`, 0x0041B99A `mov %dx`, 0x0041B9B6 `mov %ax`, 0x0041B9CF `mov %cx`, 0x0041B9EA `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B5A0 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41b8e0.o libdeps_41b8e0.a <libgcc.a>` exit 0, all 17 symbols this file references defined exactly once in the merged object - 16 of them by tree natives and the seventeenth (`___chkstk_ms`) by the toolchain CRT, which the i686 compiler emits for this file's 0x13FC-byte frame (same mechanism as core/native_41aaf0.cpp, absent in core/native_41c700.cpp whose 0xF8 frame is below the threshold); archive /tmp/q218/link/libdeps_41b8e0.a size 58696 bytes mtime 2026-10-25T15:20:10+05:30; libgcc /opt/homebrew/Cellar/mingw-w64/14.0.0_3/toolchain-i686/lib/gcc/i686-w64-mingw32/16.2.0/libgcc.a; object sha256 19a3d93550a676f8dce853dc822f06b27e8fc66c6dd0565a8d18e339b1652af5; source sha256 5aa4e16243116d383ffcd5d3c3a5b5fcbf66670e0e1adebd85365940bc47275e, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41b8e0.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d, size 14453619 bytes, mtime 2026-10-25T15:08:24, which PREDATES this file: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineScrollListBox_41B8E0$'` = 0, because this file is not yet in the build target's source list. That zero is expected and is not a defect in the file; it must be re-measured to 1 after the source-list addition, and this record updated then. `python3 scripts/check_native_linkable.py engine/src/retail/core/native_41b8e0.cpp` = `linkable (17 undefined, all defined in tree)`.
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

// 0x0053B59C: the "ScrollListBox" class id word this registrar writes (0x0041B90B). Image value 0x0181 (file bytes 81 01 at
// 0x0053B59C, immediately before the name literal at 0x0053B5A0); no other translation unit owns it.
std::uint16_t g_53b59c = 0x0181U;

// 0x0057BBB4: the parameter container pointer this body writes (0x0041BA02). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbb4 = 0U;

void NATIVE_DECL Task_DefineScrollListBox_41B8E0()
{
    // 0x0041B8E0/0x0041B8E5: `mov eax,0x13FC; call 0x004A54A0` - the compiler-emitted MSVC 6 stack probe (CRT, never
    // hand-transcribed). This array is the frame it allocates; the probe's esp drop is what sets the frame base, and every
    // offset below is relative to that base. The 0x13FC bytes are also the registered object size.
    std::uint8_t frame_storage[0x13fc];
    const std::int32_t frame_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame_storage[0]));

    // 0x0041B8EA/0x0041B8EC: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041B8F3-0x0041B911: `push %eax; push $0x13FC; push $0x0053B5A0; call 0x00401900; mov %ax,0x0053B59C`. The name bytes at
    // 0x0053B5A0 are 53 63 72 6f 6c 6c 4c 69 73 74 42 6f 78 00 = "ScrollListBox", and the registered size 0x13FC is this
    // body's probed frame.
    g_53b59c = TaskType_Register("ScrollListBox", 0x13fcU, parent, 0U);

    // 0x0041B903/0x0041B908/0x0041B90A: `push $0x0041BDD0; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041BDD0U)));

    // 0x0041B919/0x0041B91E: `push $0x0041C080; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041B923-0x0041B92B pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C080U)));

    // 0x0041B937-0x0041B93F: `push $0x0041BC00; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041BC00U)));

    // 0x0041B94B-0x0041B953: `push $0x0041BB10; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041BB10.
    QTask_LinkQTaskEventHandler(g_53b59c, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041BB10U)));

    // 0x0041B95B/0x0041B960: `push $0x0041C300; call 0x004F1A60` (argument-less byte getter); 0x0041B965-0x0041B96D links it to
    // 0x0041C300 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b59c, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C300U)));

    // 0x0041B975/0x0041B97A: `push $0x0041BCA0; call 0x004F1A70` (the second byte getter); 0x0041B97F-0x0041B988 links it to
    // 0x0041BCA0 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b59c, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041BCA0U)));

    // 0x0041B990/0x0041B995: `push $0x0041C310; call 0x00424870` (argument-less load); 0x0041B99A-0x0041B9A3 links it to
    // 0x0041C310 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C310U)));

    // 0x0041B9AB/0x0041B9B0: `push $0x0041C500; call 0x00424880`; 0x0041B9B5-0x0041B9BD links it to 0x0041C500 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C500U)));

    // 0x0041B9C5/0x0041B9CA: `push $0x0041C5A0; call 0x004F1210`; 0x0041B9CF-0x0041B9D8 links it to 0x0041C5A0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C5A0U)));

    // 0x0041B9E0/0x0041B9E5: `push $0x0041C600; call 0x004F1220`; 0x0041B9EA-0x0041B9F3 links it to 0x0041C600 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b59c, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041C600U)));

    // 0x0041B9F8/0x0041B9FA/0x0041BA02: `push $0x7; call 0x004F1950; mov %eax,0x0057BBB4` - a SEVEN-entry container, matching the
    // seven appends below.
    g_57bbb4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(7)));

    // Append 1: 0x0041BA07/0x0041BA09: `push $0x0; push $0x7; call 0x004F0FE0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041BA10: `push %eax` (the
    // handler result, pushed BEFORE the two pointer computations - this body's ordering, unlike core/native_41c700.cpp);
    // 0x0041BA17: `lea %eax,[%esp+0x1268]` at esp = frame-0x0C, i.e. frame+0x125C; 0x0041BA1E: `lea %ecx,[%esp+0xC]` = frame+0x00;
    // 0x0041BA25: call 0x004F1A80; 0x0041BA2A: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x125C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 2: 0x0041BA2D/0x0041BA2F: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041BA3D: `lea %eax,[%esp+0x12BC]` at
    // esp = frame-0x0C, i.e. frame+0x12B0; 0x0041BA44: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BA4B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x12B0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 3: 0x0041BA53/0x0041BA55: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041BA63: `lea %eax,[%esp+0x130C]` = frame+0x1300;
    // 0x0041BA6A: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BA71: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x1300,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 4: 0x0041BA79/0x0041BA7B: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041BA89: `lea %eax,[%esp+0x135C]` = frame+0x1350;
    // 0x0041BA90: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BA97: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x1350,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x0041BA9F/0x0041BAA1: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041BAAF: `lea %eax,[%esp+0x34]` = frame+0x28;
    // 0x0041BAB3: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BABA: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 6: 0x0041BAC2/0x0041BAC4: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041BAD2: `lea %eax,[%esp+0x38]` = frame+0x2C;
    // 0x0041BAD6: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BADD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x2C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 7: 0x0041BAE5/0x0041BAE7: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041BAF5: `lea %eax,[%esp+0x13AC]` = frame+0x13A0;
    // 0x0041BAFC: `lea %ecx,[%esp+0xC]` = frame+0x00; 0x0041BB03: call 0x004F1A80. This append has no cleanup of its own:
    // 0x0041BB08 `add $0x1414,%esp` covers the 0x13FC probed frame plus its six pushes, and 0x0041BB0E returns with EAX still
    // holding the append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbb4)),
                                frame_base, frame_base + 0x13A0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
