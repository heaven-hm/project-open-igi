// Native: Task_DefineDialogWindow_4199F0 (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x004199F0-0x00419BDF (496 bytes, 1 block, 0 edges, 139 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 af/afij at 0x004199F0 (496 bytes, 1 block, 0 edges, 139 instructions) plus NASM ndisasm over the exact extent; both listings agree instruction-for-instruction. The ret is at 0x00419BDF and 0x00419BE0 is the next function with no alignment byte between them - this body hands that address to 0x00401400 as its event-0 handler. Caller: 0x00402C23 `e8 c8 6d 01 00` = `call 0x004199F0`, a direct call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; the same scan finds the known-real guard calls 0x00402BEC -> 0x00420CC0 and 0x00402BF1 -> 0x00420300, which is the instrument control). `ps` at 0x00539C2C = "DialogWindow" (file bytes 44 69 61 6c 6f 67 57 69 6e 64 6f 77 00) and `pxw` at 0x00539C28 = 81 01; the parent getter 0x00424850 loads 0x0053B90C, image value 81 01; 0x0057BB80/0x0057BB84/0x0057BB88 are above the file-backed end of .data and hold image value 0. A stack replay of all 139 instructions returns esp to its entry value at the ret with zero slack: 0x20 at 0x00419A30 (the two zero words, the parent-getter result, the size, the name and the three pushes of the first link), 0x24 at 0x00419A72 (three links of three pushes each), 0xC at each of 0x00419A8C, 0x00419AA7, 0x00419AC2, 0x00419ADC and 0x00419AF7 (one link of three pushes each), 0x10 at 0x00419B19 (the last link plus the container count), 0x18 at each of 0x00419B44, 0x00419B6A, 0x00419B90 and 0x00419BB6 (appends 1-4, six pushes each) and 0x74 at 0x00419BDC (the 0x5C frame plus append 5's six pushes, which has no cleanup of its own). Call census over the linear sweep of .text (0x00401000-0x00532510): 0x00401400 1312 sites, 0x00401530 623, 0x00401900 279, 0x00424850 88, 0x00424870 26, 0x00424880 21, 0x004E7530 88, 0x004F0FA0 155, 0x004F1000 78, 0x004F1210 56, 0x004F1220 100, 0x004F1950 118, 0x004F1A60 140, 0x004F1A70 223, 0x004F1A80 870; inside this body the call counts are 0x00401530 x7, 0x004F1A80 x5, 0x004F0FA0 x4, 0x00401400 x3 and one each of 0x00401900, 0x00424850, 0x00424870, 0x00424880, 0x004E7530, 0x004F1000, 0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70 - 30 call instructions in all.
// Behavior/CFG: one straight-line block, a 0x5C-byte frame and no saved register. First initialises the two-word block at 0x0057BB80 (0x004199F3 `mov dword [0x57BB80],0x0`, 0x004199FD `mov dword [0x57BB84],0x280`). Then reads the parent type through 0x00424850, registers "DialogWindow" with object size 0x5C through 0x00401900 and stores the returned 16-bit id at 0x00539C28. Then makes TEN handler links on that id: through the no-context link 0x00401530 - event 1 -> 0x00419C90, the event word from 0x004E7530 -> 0x00419CB0, event 2 -> 0x00419C20, the word from 0x00424870 -> 0x00424300, the word from 0x00424880 -> 0x0041A120 and the words from 0x004F1210 and 0x004F1220 -> 0x004D3710 (the no-op handler, transcribed); through the context link 0x00401400 - event 0 -> 0x00419BE0 (the function immediately after this one), the byte from 0x004F1A60 -> 0x0041A110 and the byte from 0x004F1A70 -> 0x00419C50. Then allocates a FIVE-entry parameter container through 0x004F1950 into 0x0057BB88 and makes five six-argument appends through 0x004F1A80, all from frame+0x00 to frame+0x20, frame+0x24, frame+0x28, frame+0x2C and frame+0x59, the first four with the argument-less load 0x004F0FA0 as their fourth argument and the fifth with 0x004F1000, all five with the word 7 as their fifth argument and the address 0x0057BB80 as their sixth. It returns with EAX holding 0x004F1A80's return (the container pointer) because the body never writes EAX after that call.
// Status/limits: TRANSCRIBED; LINKABLE (the eight handler addresses 0x00419C90, 0x00419CB0, 0x00419C20, 0x00419BE0, 0x0041A110, 0x00419C50, 0x00424300 and 0x0041A120 are raw code immediates in retail and are transcribed by nobody, so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs. 0x004D3710 IS transcribed (engine/src/retail/script/native_4d3710.cpp) and is referenced as a symbol, so it is a real link edge); STRUCTURALLY_VERIFIED for the CFG only (one block, no edges, so there is no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra single-file compile clean, host clang++ -std=c++20 single-file compile clean); runtime UNOBSERVED. Every callee is already transcribed in this tree (0x00401900/0x00401400/0x00401530 in core/task_types.cpp and core/native_401900.cpp, 0x00424850/0x00424870/0x00424880/0x004F1210/0x004F1220 in sys/, 0x004E7530/0x004F0FA0 in core/, 0x004F1000 in render/native_4f1000.cpp, 0x004F1950/0x004F1A60/0x004F1A70/0x004F1A80/0x004D3710 in script/), so this file adds no undefined symbol. THE SIXTH ARGUMENT IS AN ADDRESS, NOT AN INERT PUSH: at every one of the five append sites retail pushes the immediate 0x0057BB80 before the argument-less getter, exactly as core/native_508fa0.cpp does with 0x00A7B2B8/0x00A7B2A8; the push is inert for the getter and load-bearing for the record, which receives it as arg6, so the port passes `&::g_57bb80` and not 0. 0x0057BB80 and 0x0057BB84 are one 8-byte block in retail (the second is the first plus 4) and the block's address is what every append records, so the port declares them as one two-element array under the first slot's name rather than two scalars; a later native that writes 0x0057BB84 must use `::g_57bb80[1]`, not a second global, or the adjacency the append records would be lost. The five frame addresses are retail frame addresses and the callee stores only their differences (0x20, 0x24, 0x28, 0x2C and 0x59), so the transcription passes equivalent addresses inside a local 0x5C scratch range, which is never read; 0x59 is byte-unaligned and is preserved as measured, not rounded. The getters 0x00424870, 0x00424880, 0x004F0FA0, 0x004F1000, 0x004F1210, 0x004F1220, 0x004E7530, 0x004F1A60 and 0x004F1A70 are argument-less loads in retail (each is `mov eax,[imm32]; ret`, verified at its own VA) and the words pushed in front of them are the handler words the following link consumes; the port keeps them as that link's handler in retail order. The byte-valued event ids from 0x004F1A60/0x004F1A70 widen to std::uint32_t, which cannot change behaviour because both link natives mask the event word for the phase index (0x00401400 to its low byte). The two zero words before the parent getter are the registration's fourth argument and the surplus fifth word every retail registration site pushes (task_types.hpp records the four-parameter callee). 0x00539C28 is declared std::uint16_t and initialised to 0x0181 because that is its image value and every access to it in the image is 16-bit (0x00419A25 `mov %ax`, 0x00419A3E `mov %ax`, 0x00419A4A `mov %cx`, 0x00419A5E `mov %dx`, 0x00419A80 `mov %ax`, 0x00419A99 `mov %cx`, 0x00419AB4 `mov %dx`, 0x00419AD0 `mov %ax`, 0x00419AE9 `mov %cx`, 0x00419B04 `mov %dx`); the meaning of 0x0181 is not established here. The class name is the retail bytes at 0x00539C2C and the string literal carries those bytes exactly. Domain: core/, because it is a task-type registrar of the guard 0x00402930 family.
// Build evidence: partial link `i686-w64-mingw32-ld -r native_4199f0.o libdeps_4199f0.a` exit 0, all 16 symbols this file references defined exactly once in the merged object; archive /tmp/q218/link/libdeps_4199f0.a size 58622 bytes mtime 2026-10-25T15:11:56+05:30; object sha256 c04f27360420924a7a76f7d1b121495933252988c7053c76a30845764e422902; source sha256 dfe01043181d302334f21e13484e171f693b7907bc243e1502e354bfe0bc98f2, which is this file with this line removed (`sed '/^\/\/ Build evidence:/d' native_4199f0.cpp | shasum -a 256`). Binary: build-win32-no-verify-gate/igi1.exe sha256 08bc8b42c060daad53cac33bdf1f3dd691d124e232b649f31e9aada96f2cc1ff, size 14474624 bytes, mtime 2026-10-25T15:38:24, which POSTDATES this file and CONTAINS it: `i686-w64-mingw32-nm build-win32-no-verify-gate/igi1.exe | grep -c ' [Tt] _Task_DefineDialogWindow_4199F0$'` = 1, measured, and this file is present in the build target's source list. This sentence previously recorded 0 against binary c04855796d289d270d6d3ed2cd484efd84bb11d2249fb27ec778c1b0d991215d (size 14453619 bytes, mtime 2026-10-25T15:08:24), which predated this file and did not contain it; a count of 0 from a binary the file was not in measures the build, not the file. Re-measured and corrected 2026-10-25 against the hash named above.
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

// 0x004F1000: `mov eax,[0xA70C90]; ret`, transcribed in engine/src/retail/render/native_4f1000.cpp as Camera_Get_4F1000.
extern std::uint32_t Camera_Get_4F1000(void);

// 0x004F1210 / 0x004F1220: argument-less 32-bit loads, transcribed in engine/src/retail/sys/native_4f1210.cpp and native_4f1220.cpp.
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1210(void);
extern std::uint32_t NATIVE_DECL Sys_GetGlobal32_4F1220(void);

// 0x004E7530: the event word (`mov eax,[0x5488C8]; ret`), transcribed in engine/src/retail/core/native_4e7530.cpp.
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530(void);

// 0x004D3710: the one-byte QTask_NoOpHandler, transcribed in engine/src/retail/script/native_4d3710.cpp. This is a real link
// edge: retail pushes the address 0x004D3710 twice here, as the handler word of the 0x004F1210 and 0x004F1220 links.
void NATIVE_DECL QTask_NoOpHandler();

// 0x004F1950 / 0x004F1A80: the task parameter container, transcribed in engine/src/retail/script/.
extern void* Script_TaskContainer_Alloc(std::int32_t const count);
extern void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                         std::int32_t const arg3, std::int32_t const arg4,
                                         std::int32_t const arg5, std::int32_t const arg6);

// 0x00539C28: the "DialogWindow" class id word this registrar writes (0x00419A25). Image value 0x0181 (file bytes 81 01 at
// 0x00539C28); no other translation unit owns it.
std::uint16_t g_539c28 = 0x0181U;

// 0x0057BB80-0x0057BB87: the two adjacent dwords this body initialises (0x004199F3 zeroes the word at 0x0057BB80, 0x004199FD
// stores 0x280 at 0x0057BB84, which retail addresses as the first word plus 4). Both words are above the file-backed end of .data
// and hold image value 0. Every append below records the address of the first word as its sixth argument, so the two words must
// stay adjacent and are declared as one object; slot [0] is 0x0057BB80 and slot [1] is 0x0057BB84.
std::uint32_t g_57bb80[2] = { 0U, 0U };

// 0x0057BB88: the parameter container pointer this body writes (0x00419B1C). Above the file-backed end of .data, image value 0.
std::uint32_t g_57bb88 = 0U;

void NATIVE_DECL Task_DefineDialogWindow_4199F0()
{
    NATIVE_LOG_ENTER("Task_DefineDialogWindow_4199F0", "0x004199F0");
    // 0x004199F0: `sub $0x5C,%esp`. The frame is the range the five appends' addresses point into; retail never writes it.
    std::uint8_t parameter_range[0x5c];

    // 0x004199F3/0x004199FD: `mov dword [0x57BB80],0x0; mov dword [0x57BB84],0x280`. These are the only two writes to that block
    // anywhere in the image before the appends record its address.
    ::g_57bb80[0] = 0U;       // 0x0057BB80
    ::g_57bb80[1] = 0x280U;   // 0x0057BB84

    // 0x00419A07/0x00419A09: `push $0x0; push $0x0; call 0x00424850` - the two zero words are the registration's fourth argument
    // and the surplus fifth word, and the getter itself takes no argument.
    const std::uint16_t parent = Sys_GetGlobal16_424850();

    // 0x00419A10-0x00419A2B: `push %eax; push $0x5C; push $0x00539C2C; call 0x00401900; mov %ax,0x00539C28`. The name bytes at
    // 0x00539C2C are 44 69 61 6c 6f 67 57 69 6e 64 6f 77 00 = "DialogWindow", and the registered size 0x5C is this body's frame.
    g_539c28 = TaskType_Register("DialogWindow", 0x5cU, parent, 0U);

    // 0x00419A1D/0x00419A22/0x00419A24: `push $0x00419C90; push $0x1; push %eax; call 0x00401530` - event 1.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00419C90U)));

    // 0x00419A33/0x00419A38: `push $0x00419CB0; call 0x004E7530` - the pushed handler is inert for the argument-less getter and
    // is consumed as this link's third argument; 0x00419A3D-0x00419A45 pushes the event and the id and calls 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00419CB0U)));

    // 0x00419A51-0x00419A59: `push $0x00419C20; push $0x2; push %cx; call 0x00401530` - event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00419C20U)));

    // 0x00419A65-0x00419A6D: `push $0x00419BE0; push $0x0; push %dx; call 0x00401400` - event 0 through the context link; the
    // handler is the function immediately after this one at 0x00419BE0.
    QTask_LinkQTaskEventHandler(g_539c28, 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00419BE0U)));

    // 0x00419A75/0x00419A7A: `push $0x0041A110; call 0x004F1A60` (argument-less byte getter); 0x00419A7F-0x00419A87 links it to
    // 0x0041A110 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_539c28, Script_PhaseIdGetE4_4F1A60(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041A110U)));

    // 0x00419A8F/0x00419A94: `push $0x00419C50; call 0x004F1A70` (the second byte getter); 0x00419A99-0x00419AA2 links it to
    // 0x00419C50 through 0x00401400.
    QTask_LinkQTaskEventHandler(g_539c28, Script_PhaseIdGetE5_4F1A70(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00419C50U)));

    // 0x00419AAA/0x00419AAF: `push $0x00424300; call 0x00424870` (argument-less load); 0x00419AB4-0x00419ABD links it to
    // 0x00424300 through 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, Sys_GetGlobal32_424870(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00424300U)));

    // 0x00419AC5/0x00419ACA: `push $0x0041A120; call 0x00424880`; 0x00419ACF-0x00419AD7 links it to 0x0041A120 through 0x00401530
    // (the same handler word the 0x0041B310 and 0x0041A1E0 registrars link to this getter's value).
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, Sys_GetGlobal32_424880(),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0041A120U)));

    // 0x00419ADF/0x00419AE4: `push $0x004D3710; call 0x004F1210`; 0x00419AE9-0x00419AF2 links it to the no-op handler through
    // 0x00401530. 0x004D3710 is transcribed, so this is a symbol reference and a real link edge, not a cast integer.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, Sys_GetGlobal32_4F1210(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x00419AFA/0x00419AFF: `push $0x004D3710; call 0x004F1220`; 0x00419B04-0x00419B0D links it to the no-op handler through
    // 0x00401530.
    QTask_LinkQTaskEventHandlerNoContextFn(g_539c28, Sys_GetGlobal32_4F1220(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler));

    // 0x00419B12/0x00419B14/0x00419B1C: `push $0x5; call 0x004F1950; mov %eax,0x0057BB88` - a FIVE-entry container, matching the
    // five appends below.
    g_57bb88 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(5)));

    // The five appends address this body's frame: retail passes frame+0x00 and frame+0x20/0x24/0x28/0x2C/0x59 and 0x004F1A80
    // stores only the differences, so the frame base is taken once as the same 32-bit value retail computes. The sixth argument is
    // the address of the 0x0057BB80 block, which retail pushes as an immediate before each argument-less getter.
    const std::int32_t parameter_base =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&parameter_range[0]));
    const std::int32_t block_57bb80 =
        static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bb80[0]));

    // Append 1: 0x00419B21/0x00419B26: `push $0x0057BB80; push $0x7; call 0x004F0FA0` (the argument-less load ignores both words;
    // they are this append's sixth and fifth arguments); 0x00419B34: `lea 0x2C(%esp)` = frame+0x20; 0x00419B38: `lea 0xC(%esp)` =
    // frame+0x00; 0x00419B3F: call 0x004F1A80; 0x00419B44: `add $0x18,%esp`.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb88)),
                                parameter_base, parameter_base + 0x20,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, block_57bb80);

    // Append 2: 0x00419B47/0x00419B4C: `push $0x0057BB80; push $0x7; call 0x004F0FA0`; 0x00419B5A: `lea 0x30(%esp)` = frame+0x24;
    // 0x00419B5E: `lea 0xC(%esp)` = frame+0x00; 0x00419B65: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb88)),
                                parameter_base, parameter_base + 0x24,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, block_57bb80);

    // Append 3: 0x00419B6D/0x00419B72: `push $0x0057BB80; push $0x7; call 0x004F0FA0`; 0x00419B80: `lea 0x34(%esp)` = frame+0x28;
    // 0x00419B84: `lea 0xC(%esp)` = frame+0x00; 0x00419B8B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb88)),
                                parameter_base, parameter_base + 0x28,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, block_57bb80);

    // Append 4: 0x00419B93/0x00419B98: `push $0x0057BB80; push $0x7; call 0x004F0FA0`; 0x00419BA6: `lea 0x38(%esp)` = frame+0x2C;
    // 0x00419BAA: `lea 0xC(%esp)` = frame+0x00; 0x00419BB1: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb88)),
                                parameter_base, parameter_base + 0x2C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, block_57bb80);

    // Append 5: 0x00419BB9/0x00419BBE: `push $0x0057BB80; push $0x7; call 0x004F1000` (the camera dword, not 0x004F0FA0);
    // 0x00419BCC: `lea 0x65(%esp)` = frame+0x59 (byte-unaligned, as measured); 0x00419BD0: `lea 0xC(%esp)` = frame+0x00;
    // 0x00419BD7: call 0x004F1A80. This append has no cleanup of its own: 0x00419BDC `add $0x74,%esp` covers the 0x5C frame plus
    // its six pushes, and 0x00419BDF returns with EAX still holding the append's return.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(g_57bb88)),
                                parameter_base, parameter_base + 0x59,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, block_57bb80);
}

} // extern "C"
