// Native: Task_DefineTextBox_41AAF0 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041AAF0-0x0041AC94 (421 bytes, 1 block, 0 edges, 124 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041AAF0 (421 bytes, 1 block, 0 edges, 124 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041AC94, the bytes 0x0041AC95-0x0041AC9F are eleven 0x90 alignment nops (not part of the routine), and 0x0041ACA0 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C19 `e8 d2 6e 01 00` = `call 0x0041AAF0`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B584 = "TextBox" (file bytes 54 65 78 74 42 6f 78 00) and `pxw` at 0x0053B580 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BB98 is above the file-backed end of .data and holds image value 0. FRAME: 0x0041AAF0 `mov eax,0x2244` / 0x0041AAF5 `call 0x004A54A0` - the MSVC 6 4KB stack probe, which lowers esp by 0x2244 itself and returns through `push eax; ret`, so the frame base is set inside the probe and there is no `sub esp` here; the probe is a compiler-emitted CRT helper and is never hand-transcribed (the precedent is core/native_4170a0.cpp and core/native_4310f0.cpp, and scripts/native_coverage.py classifies it as such). This prologue pushes no callee-saved register, so there is no displacement shift and every `lea reg,[esp+N]` below resolves directly against the probe-set base. A stack replay of all 124 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041AB26 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x18 at 0x0041AB54 (links 2 and 3, three pushes each), 0xC at 0x0041AB6F (link 4), 0x18 at 0x0041AB9D (links 5 and 6, three pushes each), 0xC at 0x0041ABB8 (link 7), 0x10 at 0x0041ABD9 (link 8 plus the container count), 0x18 at each of 0x0041AC00, 0x0041AC22, 0x0041AC44 and 0x0041AC69 (appends 1-4, six pushes each) and 0x225C at 0x0041AC8E (the 0x2244 probe frame plus append 5's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004A54A0 34, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F0FE0 32, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x5, 0x004F1A80 x5, 0x00401400 x3, 0x004F0FA0 x2, 0x004F0FC0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004A54A0, 0x004E7530, 0x004F0FE0, 0x004F1950, 0x004F1A60, 0x004F1A70 - 27 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x2244-byte probed frame and no saved register. Reads the parent type through 0x00424850, registers "TextBox" with object size 0x2244 through 0x00401900 and stores the returned 16-bit id at 0x0053B580. Then makes EIGHT handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041AE20, the event word from 0x004E7530 -> 0x0041AEA0, event 2 -> 0x0041AD20, the word from 0x00424870 -> 0x0041B170 and the word from 0x00424880 -> 0x0041A120; through the context link 0x00401400 - event 0 -> 0x0041ACA0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041B160 and the byte from 0x004F1A70 -> 0x0041AD70. Then allocates a FIVE-entry parameter container through 0x004F1950 into 0x0057BB98 and makes five six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x54, frame+0x28, frame+0x2C, frame+0x21A4 and frame+0x21F4, the first with the argument-less load 0x004F0FE0 as its fourth argument, the next two with 0x004F0FA0 and the last two with 0x004F0FC0, and all five with the word 7 as their fifth argument and the literal 0 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the eight handler addresses 0x0041AE20, 0x0041AEA0, 0x0041AD20, 0x0041B170, 0x0041A120, 0x0041ACA0, 0x0041B160 and 0x0041AD70 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F0FE0 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol; 0x004A54A0 is deliberately NOT declared as a callee, because it is the compiler-emitted MSVC 6 stack probe rather than a retail routine - the C++ frame replaces it, which is what the two precedents in this tree do. THE SIXTH ARGUMENT IS THE LITERAL 0 AT ALL FIVE APPEND SITES: retail pushes `6a 00; 6a 07` (0 at 0x0041ABE1, 0x0041AC03, 0x0041AC25, 0x0041AC47, 0x0041AC6C and 7 at 0x0041ABE3, 0x0041AC05, 0x0041AC27, 0x0041AC49, 0x0041AC6E) - two immediates, not a slot address - so unlike core/native_4199f0.cpp this unit has no load-bearing pointer in arg6, and the order is re-derived from each call's own push sequence rather than carried over from that unit. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004F0FE0, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The five append addresses are retail frame addresses and the callee stores only their differences (0x54, 0x28, 0x2C, 0x21A4 and 0x21F4), so the transcription passes equivalent addresses inside a local 0x2244 scratch range, which is never read; 0x54 is the first and highest of the low group, and the two 0x21xx offsets sit near the top of the same probed frame. 0x0053B580 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041AB1B `mov %ax`, 0x0041AB34 `mov %ax`, 0x0041AB40 `mov %cx`, 0x0041AB61 `mov %dx`, 0x0041AB7D `mov %ax`, 0x0041AB89 `mov %cx`, 0x0041ABAA `mov %dx`, 0x0041ABC6 `mov %ax`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B584 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41aaf0.o libdeps_41aaf0.a <libgcc.a>` exit 0, all 15 symbols this file references defined exactly once in the merged object - 14 of them by tree natives and the fifteenth (`___chkstk_ms`) by the toolchain CRT, which is the probe helper the i686 compiler emits for this file's 0x2244-byte frame (evidence: `nm --defined-only $(i686-w64-mingw32-g++ -print-libgcc-file-name) | grep chkstk` lists `_chkstk_ms.o: T ___chkstk_ms`), so it is not a retail native and not a tree dependency; archive /tmp/q218/link/libdeps_41aaf0.a size 57474 bytes mtime 2026-10-25T15:14:39+05:30; libgcc /opt/homebrew/Cellar/mingw-w64/14.0.0_3/toolchain-i686/lib/gcc/i686-w64-mingw32/16.2.0/libgcc.a; object sha256 4dc2ac04610db43669ee696edad9668362e975bb5a2848b4b797db432f2a7407; source sha256 b68634944e75bede5dd9e01b470a6e6d1ce2a28d78995f21e6f465174518115d, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41aaf0.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 08bc8b42c060daad53cac33bdf1f3dd691d124e232b649f31e9aada96f2cc1ff, size 14474624 bytes, mtime 2026-10-25T15:38:24, which POSTDATES this file and CONTAINS it: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineTextBox_41AAF0$'` = 1, measured, and this file is present in the build target's source list. This sentence previously recorded 0 against binary c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d (size 14453619 bytes, mtime 2026-10-25T15:08:24), which predated this file and did not contain it; a count of 0 from a binary the file was not in measures the build, not the file. Re-measured and corrected 2026-10-25 against the hash named above.
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

// 0x004E7530: the event word (`mov eax,[0x5488C8]; ret`), transcribed in engine/src/retail/core/native_4e7530.cpp.
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);

// 0x004F1950 / 0x004F1A80: the task parameter container, transcribed in engine/src/retail/script/.
extern void* Script_TaskContainer_Alloc(std::int32_t const count);
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

// 0x0053B580: the "TextBox" class id word this registrar writes (0x0041AB1B). Image value 0x0181 (file bytes 81 01 at
// 0x0053B580, immediately before the name literal at 0x0053B584); no other translation unit owns it.
std::uint16_t g_53b580 = 0x0181U;

// 0x0057BB98: the parameter container pointer this body writes (0x0041ABDC). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bb98 = 0U;

void NATIVE_DECL Task_DefineTextBox_41AAF0()
{
    // 0x0041AAF0/0x0041AAF5: `mov eax,0x2244; call 0x004A54A0` - the compiler-emitted MSVC 6 stack probe (CRT, never
    // hand-transcribed). This array is the frame it allocates; the probe's esp drop is what sets the frame base, and every
    // offset below is relative to that base. The 0x2244 bytes are also the registered object size.
    std::uint8_t frame_storage[0x2244];
    const std::int32_t frame_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&frame_storage[0]));

    // 0x0041AAFA/0x0041AAFC: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041AB03-0x0041AB21: `push %eax; push $0x2244; push $0x0053B584; call 0x00401900; mov %ax,0x0053B580`. The name bytes at
    // 0x0053B584 are 54 65 78 74 42 6f 78 00 = "TextBox", and the registered size 0x2244 is this body's probed frame.
    g_53b580 = TaskType_Register("TextBox", 0x2244U, parent, 0U);

    // 0x0041AB13/0x0041AB18/0x0041AB1A: `push $0x0041AE20; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b580, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041AE20U)));

    // 0x0041AB29/0x0041AB2E: `push $0x0041AEA0; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x0041AB33-0x0041AB3B pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b580, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041AEA0U)));

    // 0x0041AB47-0x0041AB4F: `push $0x0041AD20; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b580, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041AD20U)));

    // 0x0041AB57/0x0041AB5C: `push $0x0041B170; call 0x00424870` (argument-less load); 0x0041AB61-0x0041AB6A links it to
    // 0x0041B170 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b580, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B170U)));

    // 0x0041AB72/0x0041AB77: `push $0x0041A120; call 0x00424880`; 0x0041AB7D-0x0041AB84 links it to 0x0041A120 through
    // 0x00401530 (the same handler word the 0x0041B310, 0x0041A1E0 and 0x004199F0 registrars link to this getter's value).
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b580, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041A120U)));

    // 0x0041AB90-0x0041AB98: `push $0x0041ACA0; push $0x0; push %cx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041ACA0.
    QTask_LinkQTaskEventHandler(g_53b580, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041ACA0U)));

    // 0x0041ABA0/0x0041ABA5: `push $0x0041B160; call 0x004F1A60` (argument-less byte getter); 0x0041ABAA-0x0041ABB3 links it to
    // 0x0041B160 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b580, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B160U)));

    // 0x0041ABBB/0x0041ABC0: `push $0x0041AD70; call 0x004F1A70` (the second byte getter); 0x0041ABC5-0x0041ABCD links it to
    // 0x0041AD70 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b580, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041AD70U)));

    // 0x0041ABD2/0x0041ABD4/0x0041ABDC: `push $0x5; call 0x004F1950; mov %eax,0x0057BB98` - a FIVE-entry container, matching the
    // five appends below.
    g_57bb98 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(5)));

    // Append 1: 0x0041ABE1/0x0041ABE3: `push $0x0; push $0x7; call 0x004F0FE0` (the argument-less load ignores both words; they are
    // this append's sixth and fifth arguments, and both are immediates here, not a slot address); 0x0041ABEA: `lea 0x5C(%esp)` =
    // frame+0x54; 0x0041ABF4: `lea 0xC(%esp)` = frame+0x00; 0x0041ABFB: call 0x004F1A80; 0x0041AC00: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb98)),
                                frame_base, frame_base + 0x54,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 2: 0x0041AC03/0x0041AC05: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041AC0C: `lea 0x30(%esp)` = frame+0x28;
    // 0x0041AC17: `lea 0x10(%esp)` = frame+0x00 (the low pointer is computed after both pushes here, hence the 0x10); 0x0041AC1D:
    // call 0x004F1A80; 0x0041AC22: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb98)),
                                frame_base, frame_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 3: 0x0041AC25/0x0041AC27: `push $0x0; push $0x7; call 0x004F0FA0`; 0x0041AC2E: `lea 0x34(%esp)` = frame+0x2C;
    // 0x0041AC38: `lea 0xC(%esp)` = frame+0x00; 0x0041AC3F: call 0x004F1A80; 0x0041AC44: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb98)),
                                frame_base, frame_base + 0x2C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 4: 0x0041AC47/0x0041AC49: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041AC50: `lea 0x21AC(%esp)` = frame+0x21A4;
    // 0x0041AC5D: `lea 0xC(%esp)` = frame+0x00; 0x0041AC64: call 0x004F1A80; 0x0041AC69: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb98)),
                                frame_base, frame_base + 0x21A4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x0041AC6C/0x0041AC6E: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041AC75: `lea 0x21FC(%esp)` = frame+0x21F4;
    // 0x0041AC82: `lea 0xC(%esp)` = frame+0x00; 0x0041AC89: call 0x004F1A80. This append has no cleanup of its own: 0x0041AC8E
    // `add $0x225C,%esp` covers the 0x2244 probed frame plus its six pushes, and 0x0041AC94 returns with EAX still holding the
    // append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb98)),
                                frame_base, frame_base + 0x21F4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
