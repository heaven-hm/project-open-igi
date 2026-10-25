// Native: Task_DefineToggleBox_41DC50 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041DC50-0x0041DDC2 (371 bytes, 1 block, 0 edges, 101 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041DC50 (371 bytes, 1 block, 0 edges, 101 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041DDC2, the bytes 0x0041DDC3-0x0041DDCF are thirteen 0x90 alignment nops (not part of the routine), and 0x0041DDD0 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C00 `e8 4b b0 01 00` = `call 0x0041DC50`, a direct call inside the startup guard 0x00402930 (found by decoding every E8 opcode in .text and taking v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B678 = "ToggleBox" (file bytes 54 6f 67 67 6c 65 42 6f 78 00) and `pxw` at 0x0053B674 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBD0 is above the file-backed end of .data and holds image value 0. A stack replay of all 101 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041DC82 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041DCC4 (three links of three pushes each), 0xC at each of 0x0041DCDE, 0x0041DCF9, 0x0041DD14, 0x0041DD2E and 0x0041DD49 (one link of three pushes each), 0x10 at 0x0041DD6B (the last link plus the container count), 0x18 at 0x0041DD96 (the first append's six pushes) and 0x164 at 0x0041DDBC (the 0x14C frame plus the second append's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FC0 106, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x7, 0x00401400 x3, 0x004F1A80 x2, 0x004F0FC0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 24 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x14C-byte frame and no saved register. Reads the parent type through 0x00424850, registers "ToggleBox" with object size 0x14C through 0x00401900 and stores the returned 16-bit id at 0x0053B674. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041E020, the event word from 0x004E7530 -> 0x0041E130, event 2 -> 0x0041DEB0, the word from 0x00424870 -> 0x0041E420, the word from 0x00424880 -> 0x0041E500, the word from 0x004F1210 -> 0x0041E5A0 and the word from 0x004F1220 -> 0x0041E600; through the context link 0x00401400 - event 0 -> 0x0041DDD0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041E410 and the byte from 0x004F1A70 -> 0x0041DF30. Then allocates a TWO-entry parameter container through 0x004F1950 into 0x0057BBD0 and makes two six-argument appends through 0x004F1A80, both from frame+0x00 to frame+0xAC and frame+0xFC, both with the argument-less load 0x004F0FC0 as their fourth argument and the words 7 and 0 as their fifth and sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the ten handler addresses 0x0041E020, 0x0041E130, 0x0041DEB0, 0x0041DDD0, 0x0041E410, 0x0041DF30, 0x0041E420, 0x0041E500, 0x0041E5A0 and 0x0041E600 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0/0x004F1210/0x004F1220 in sys/, 0x004E7530 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. The ten handler addresses 0x0041E020, 0x0041E130, 0x0041DEB0, 0x0041DDD0, 0x0041E410, 0x0041DF30, 0x0041E420, 0x0041E500, 0x0041E5A0 and 0x0041E600 are untranscribed bodies in the 0x0041DExx/0x0041E4xx/0x0041E5xx/0x0041E6xx band; they are passed as cast integers exactly as core/native_42d2b0.cpp and core/native_40fe30.cpp do, so they create no link dependency and no stub is introduced. The getters 0x00424870, 0x00424880, 0x004F0FC0, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order, and for the appends the pre-pushed 0 and 7 become the fifth and sixth arguments. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The append addresses are retail frame addresses and the callee stores only their differences (0xAC and 0xFC), so the transcription passes equivalent addresses inside a local 0x14C scratch range, which is never read. 0x0053B674 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041DC77 `mov %ax`, 0x0041DC90 `mov %ax`, 0x0041DC9C `mov %cx`, 0x0041DCB0 `mov %dx`, 0x0041DCD2 `mov %ax`, 0x0041DCEB `mov %cx`, 0x0041DD06 `mov %dx`, 0x0041DD22 `mov %ax`, 0x0041DD3B `mov %cx`, 0x0041DD56 `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B678 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41dc50.o libdeps_41dc50.a` exit 0, all 14 symbols this file references defined exactly once in the merged object; archive /tmp/q218/link/libdeps_41dc50.a size 58696 bytes mtime 2026-10-25T15:05:29+05:30; object sha256 f95a08f8d6aaa3241d390a13b1023d7ede1f9e99eebe5a0761d71b46abe4d223; source sha256 0e9b569f58a491cc29b8717c4457ff856dad88ac1fb9775967e5fe9f078eef78, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41dc50.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d, size 14453619 bytes, mtime 2026-10-25T15:08:24; `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineToggleBox_41DC50$'` = 1, so this native is in the linked binary exactly once (measured, not inferred from a green build).
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

// 0x00424870 / 0x00424880: argument-less 32-bit loads (`mov eax,[0x53B914]; ret` and `mov eax,[0x53B918]; ret`), transcribed
// in engine/src/retail/sys/native_424870.cpp and native_424880.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424870(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_424880(void);

// 0x004F0FC0 / 0x004F1210 / 0x004F1220: argument-less 32-bit loads (`[0xA70C9C]`, `[0x54DDA8]`, `[0x54DDAC]`), transcribed in
// engine/src/retail/sys/native_4f0fc0.cpp, native_4f1210.cpp and native_4f1220.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void);

// 0x004E7530: the event word (`mov eax,[0x5488C8]; ret`), transcribed in engine/src/retail/core/native_4e7530.cpp.
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);

// 0x004F1950 / 0x004F1A80: the task parameter container, transcribed in engine/src/retail/script/.
extern void* Script_TaskContainer_Alloc(std::int32_t const count);
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

// 0x0053B674: the "ToggleBox" class id word this registrar writes (0x0041DC77). Image value 0x0181 (file bytes 81 01 at
// 0x0053B674, immediately before the name literal at 0x0053B678); no other translation unit owns it.
std::uint16_t g_53b674 = 0x0181U;

// 0x0057BBD0: the parameter container pointer this body writes (0x0041DD6E). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bbd0 = 0U;

void NATIVE_DECL Task_DefineToggleBox_41DC50()
{
    // 0x0041DC50: `sub $0x14C,%esp`. The frame is the range the two appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0x14c];

    // 0x0041DC56/0x0041DC58: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041DC5F-0x0041DC7D: `push %eax; push $0x14C; push $0x0053B678; call 0x00401900; mov %ax,0x0053B674`. The name bytes
    // at 0x0053B678 are 54 6f 67 67 6c 65 42 6f 78 00 = "ToggleBox", and the registered size 0x14C is this body's frame.
    g_53b674 = TaskType_Register("ToggleBox", 0x14cU, parent, 0U);

    // 0x0041DC6F/0x0041DC74/0x0041DC76: `push $0x0041E020; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E020U)));

    // 0x0041DC85/0x0041DC8A: `push $0x0041E130; call 0x004E7530` - the pushed handler is inert for the argument-less getter
    // and is consumed as this link's third argument; 0x0041DC8F-0x0041DC97 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E130U)));

    // 0x0041DCA3-0x0041DCAB: `push $0x0041DEB0; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DEB0U)));

    // 0x0041DCB7-0x0041DCBF: `push $0x0041DDD0; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041DDD0.
    QTask_LinkQTaskEventHandler(g_53b674, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DDD0U)));

    // 0x0041DCC7/0x0041DCCC: `push $0x0041E410; call 0x004F1A60` (argument-less byte getter); 0x0041DCD1-0x0041DCD9 links it
    // to 0x0041E410 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b674, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E410U)));

    // 0x0041DCE1/0x0041DCE6: `push $0x0041DF30; call 0x004F1A70` (the second byte getter); 0x0041DCEB-0x0041DCF4 links it to
    // 0x0041DF30 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b674, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041DF30U)));

    // 0x0041DCFC/0x0041DD01: `push $0x0041E420; call 0x00424870` (argument-less load); 0x0041DD06-0x0041DD0F links it to
    // 0x0041E420 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E420U)));

    // 0x0041DD17/0x0041DD1C: `push $0x0041E500; call 0x00424880`; 0x0041DD21-0x0041DD29 links it to 0x0041E500 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E500U)));

    // 0x0041DD31/0x0041DD36: `push $0x0041E5A0; call 0x004F1210`; 0x0041DD3B-0x0041DD44 links it to 0x0041E5A0 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E5A0U)));

    // 0x0041DD4C/0x0041DD51: `push $0x0041E600; call 0x004F1220`; 0x0041DD56-0x0041DD5F links it to 0x0041E600 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b674, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041E600U)));

    // 0x0041DD64/0x0041DD66/0x0041DD6E: `push $0x2; call 0x004F1950; mov %eax,0x0057BBD0` - a TWO-entry container, matching the
    // two appends below.
    g_57bbd0 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(2)));

    // The two appends address this body's frame: retail passes frame+0x00 and frame+0xAC/0xFC and 0x004F1A80 stores only the
    // differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041DD73/0x0041DD75: `push $0x0; push $0x7; call 0x004F0FC0` (the argument-less load ignores both words and they
    // become this append's sixth and fifth arguments); 0x0041DD7D: `lea 0xB8(%esp)` = frame+0xAC; 0x0041DD8A: `lea 0xC(%esp)` =
    // frame+0x00; 0x0041DD91: call 0x004F1A80; 0x0041DD96: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbd0)),
                                parameter_base, parameter_base + 0xAC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 2: 0x0041DD99/0x0041DD9B: `push $0x0; push $0x7; call 0x004F0FC0` (the same getter as append 1); 0x0041DDA9:
    // `lea 0x108(%esp)` = frame+0xFC; 0x0041DDB0: `lea 0xC(%esp)` = frame+0x00; 0x0041DDB7: call 0x004F1A80. This append has no
    // cleanup of its own: 0x0041DDBC `add $0x164,%esp` covers the 0x14C frame plus its six pushes, and 0x0041DDC2 returns with EAX
    // still holding the append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bbd0)),
                                parameter_base, parameter_base + 0xFC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
