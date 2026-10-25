// Native: Task_DefinePictureBox_41B310 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x0041B310-0x0041B48E (383 bytes, 1 block, 0 edges, 111 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x0041B310 (383 bytes, 1 block, 0 edges, 111 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x0041B48E, the byte 0x0041B48F is one 0x90 alignment nop (not part of the routine), and 0x0041B490 is the next function - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C14 `e8 f7 86 01 00` = `call 0x0041B310`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x0053B590 = "PictureBox" (file bytes 50 69 63 74 75 72 65 42 6f 78 00) and `pxw` at 0x0053B58C = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; the container slot 0x0057BBA8 is above the file-backed end of .data and holds image value 0. A stack replay of all 111 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x0041B342 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x0041B384 (three links of three pushes each), 0xC at each of 0x0041B39E, 0x0041B3B9, 0x0041B3D4 and 0x0041B3EE (one link of three pushes each), 0x10 at 0x0041B3F5 (the last link plus the container count), 0x18 at each of 0x0041B41C, 0x0041B43E, 0x0041B463 and 0x0041B488's predecessor (each append's six pushes) and 0xA14 at 0x0041B488 (the 0x9FC frame plus the last append's six pushes). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FA0 155, 0x004F0FC0 106, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x5, 0x004F1A80 x4, 0x00401400 x3, 0x004F0FA0 x2, 0x004F0FC0 x2 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F1950, 0x004F1A60, 0x004F1A70 - 26 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x9FC-byte frame and no saved register. Reads the parent type through 0x00424850, registers "PictureBox" with object size 0x9FC through 0x00401900 and stores the returned 16-bit id at 0x0053B58C. Then makes EIGHT handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x0041B590, the event word from 0x004E7530 -> 0x0041B5F0, event 2 -> 0x0041B500, the word from 0x00424870 -> 0x0041B750 and the word from 0x00424880 -> 0x0041A120; through the context link 0x00401400 - event 0 -> 0x0041B490 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041B830 and the byte from 0x004F1A70 -> 0x0041B530. Then allocates a FOUR-entry parameter container through 0x004F1950 into 0x0057BBA8 and makes four six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x28, frame+0x2C, frame+0x95C and frame+0x9AC, the first two with the argument-less load 0x004F0FA0 as their fourth argument and the last two with 0x004F0FC0, and all four with the words 7 and 0 as their fifth and sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the eight handler addresses 0x0041B590, 0x0041B5F0, 0x0041B500, 0x0041B490, 0x0041B830, 0x0041B530, 0x0041B750 and 0x0041A120 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F0FC0 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80 in script/), so this file adds no undefined symbol. The eight handler addresses 0x0041B590, 0x0041B5F0, 0x0041B500, 0x0041B490, 0x0041B830, 0x0041B530, 0x0041B750 and 0x0041A120 are untranscribed bodies in the 0x0041A1xx/0x0041B4xx-0x0041B8xx band; they are passed as cast integers exactly as core/native_42d2b0.cpp and core/native_40fe30.cpp do, so they create no link dependency and no stub is introduced. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F0FC0, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order, and for the appends the pre-pushed 0 and 7 become the fifth and sixth arguments. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). The append addresses are retail frame addresses and the callee stores only their differences (0x28, 0x2C, 0x95C and 0x9AC), so the transcription passes equivalent addresses inside a local 0x9FC scratch range, which is never read. 0x0053B58C is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x0041B337 `mov %ax`, 0x0041B350 `mov %ax`, 0x0041B35C `mov %cx`, 0x0041B370 `mov %dx`, 0x0041B392 `mov %ax`, 0x0041B3AB `mov %cx`, 0x0041B3C6 `mov %dx`, 0x0041B3E2 `mov %ax`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x0053B590 and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_41b310.o libdeps_41b310.a` exit 0, all 13 symbols this file references defined exactly once in the merged object; archive /tmp/q218/link/libdeps_41b310.a size 58696 bytes mtime 2026-10-25T15:05:29+05:30; object sha256 9ab223325e76b8458d3aa47847b43358bd4952ecabe378342e377f001acafa6e; source sha256 ec0efe80612099f70712198d687f069348b823b8beac3a47bccb76a4995caa49, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_41b310.cpp | shasum -a 256`).
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

// 0x004F0FA0: `mov eax,[0xA70C7C]; ret`, transcribed in engine/src/retail/core/native_4f0fa0.cpp as Core_GetRenderWordA_4F0FA0.
extern std::uint32_t NATIVE_DECL Core_GetRenderWordA_4F0FA0(void);

// 0x004F0FC0: `mov eax,[0xA70C9C]; ret`, transcribed in engine/src/retail/sys/native_4f0fc0.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F0FC0(void);

// 0x004E7530: the event word (`mov eax,[0x5488C8]; ret`), transcribed in engine/src/retail/core/native_4e7530.cpp.
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);

// 0x004F1950 / 0x004F1A80: the task parameter container, transcribed in engine/src/retail/script/.
extern void* Script_TaskContainer_Alloc(std::int32_t const count);
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

// 0x0053B58C: the "PictureBox" class id word this registrar writes (0x0041B337). Image value 0x0181 (file bytes 81 01 at
// 0x0053B58C, immediately before the name literal at 0x0053B590); no other translation unit owns it.
std::uint16_t g_53b58c = 0x0181U;

// 0x0057BBA8: the parameter container pointer this body writes (0x0041B3F8). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bba8 = 0U;

void NATIVE_DECL Task_DefinePictureBox_41B310()
{
    // 0x0041B310: `sub $0x9FC,%esp`. The frame is the range the four appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0x9fc];

    // 0x0041B316/0x0041B318: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth
    // argument and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x0041B31F-0x0041B33D: `push %eax; push $0x9FC; push $0x0053B590; call 0x00401900; mov %ax,0x0053B58C`. The name bytes
    // at 0x0053B590 are 50 69 63 74 75 72 65 42 6f 78 00 = "PictureBox", and the registered size 0x9FC is this body's frame.
    g_53b58c = TaskType_Register("PictureBox", 0x9fcU, parent, 0U);

    // 0x0041B32F/0x0041B334/0x0041B336: `push $0x0041B590; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b58c, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B590U)));

    // 0x0041B345/0x0041B34A: `push $0x0041B5F0; call 0x004E7530` - the pushed handler is inert for the argument-less getter
    // and is consumed as this link's third argument; 0x0041B34F-0x0041B357 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b58c, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B5F0U)));

    // 0x0041B363-0x0041B36B: `push $0x0041B500; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b58c, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B500U)));

    // 0x0041B377-0x0041B37F: `push $0x0041B490; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x0041B490.
    QTask_LinkQTaskEventHandler(g_53b58c, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B490U)));

    // 0x0041B387/0x0041B38C: `push $0x0041B830; call 0x004F1A60` (argument-less byte getter); 0x0041B391-0x0041B399 links it
    // to 0x0041B830 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b58c, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B830U)));

    // 0x0041B3A1/0x0041B3A6: `push $0x0041B530; call 0x004F1A70` (the second byte getter); 0x0041B3AB-0x0041B3B4 links it to
    // 0x0041B530 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_53b58c, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B530U)));

    // 0x0041B3BC/0x0041B3C1: `push $0x0041B750; call 0x00424870` (argument-less load); 0x0041B3C6-0x0041B3CF links it to
    // 0x0041B750 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b58c, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041B750U)));

    // 0x0041B3D7/0x0041B3DC: `push $0x0041A120; call 0x00424880`; 0x0041B3E1-0x0041B3E9 links it to 0x0041A120 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_53b58c, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041A120U)));

    // 0x0041B3EE/0x0041B3F0/0x0041B3F8: `push $0x4; call 0x004F1950; mov %eax,0x0057BBA8` - a FOUR-entry container, matching the
    // four appends below.
    g_57bba8 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(4)));

    // The four appends address this body's frame: retail passes frame+0x00 and frame+0x28/0x2C/0x95C/0x9AC and 0x004F1A80
    // stores only the differences, so the frame base is taken once as the same 32-bit value retail computes.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));

    // Append 1: 0x0041B3FD/0x0041B3FF: `push $0x0; push $0x7; call 0x004F0FA0` (the argument-less load ignores both words and they
    // become this append's sixth and fifth arguments); 0x0041B406: `lea 0x30(%esp)` = frame+0x28; 0x0041B410: `lea 0xC(%esp)` =
    // frame+0x00; 0x0041B417: call 0x004F1A80; 0x0041B41C: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bba8)),
                                parameter_base, parameter_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 2: 0x0041B41F/0x0041B421: `push $0x0; push $0x7; call 0x004F0FA0` (the same getter as append 1); 0x0041B428:
    // `lea 0x34(%esp)` = frame+0x2C; 0x0041B42E: `lea 0x10(%esp)` = frame+0x00; 0x0041B439: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bba8)),
                                parameter_base, parameter_base + 0x2C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 3: 0x0041B441/0x0041B443: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041B44A: `lea 0x964(%esp)` = frame+0x95C;
    // 0x0041B457: `lea 0xC(%esp)` = frame+0x00; 0x0041B45E: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bba8)),
                                parameter_base, parameter_base + 0x95C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 4: 0x0041B466/0x0041B468: `push $0x0; push $0x7; call 0x004F0FC0`; 0x0041B46F: `lea 0x9B4(%esp)` = frame+0x9AC;
    // 0x0041B47C: `lea 0xC(%esp)` = frame+0x00; 0x0041B483: call 0x004F1A80. This append has no cleanup of its own: 0x0041B488
    // `add $0xA14,%esp` covers the 0x9FC frame plus its six pushes, and 0x0041B48E returns with EAX still holding the append's
    // return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bba8)),
                                parameter_base, parameter_base + 0x9AC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
}

} // extern "C"
