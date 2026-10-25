// Native: Task_DefineComputerHilight (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004682F0-0x004684E1.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 498 bytes, 1 block, 147 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 147 instructions / 498 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004682F0-0x004684E1; the ret is at 0x004684E1, fourteen nops follow, and
//   0x004684F0 is the next function, which this body uses as a handler); `ps` at 0x0053F6D4 for the name literal and `px 24 @ 0x0053F6D0` for
//   its bytes and for the two bytes immediately before it (81 01 00 00 then 436f6d707574657248696c6967687400 = "ComputerHilight", so the
//   16-bit id slot at 0x0053F6D0 holds 0x0181 in the image); `pxw` at 0x005BDC90, 0x005BDC94, 0x005BDC84 and 0x005BDC80 for the image values; a
//   stack-pointer simulation over all 147 instructions, which returns esp to its entry value at the ret and accounts for every cleanup (0x2C =
//   eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the first two links; 0xC twice;
//   0x10 once = the third link's three pushes plus the container count; 0x18 seven times; 0x4FC = the 0x4E0 frame plus the eighth append's six
//   pushes and the task-list argument) and which resolved all sixteen `lea reg,[esp+N]` pairs to frame-relative addresses. Every callee was
//   re-read at its own VA, which is how 0x0046B4D0, 0x005034D0, 0x004F1010, 0x004F0FC0, 0x004F0FF0, 0x004F0F00, 0x004F0FE0, 0x004F1A60,
//   0x004F1A70 and 0x00467D90 were established as argument-less loads despite being called with pushes. Argument numbers were derived by counting
//   the pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body with a 0x4E0-byte frame, which is also the registered object size. It first writes 0x13 into the dword at
//   0x005BDC90, then stores (0x0046B4D0() + 0x4B0) into the dword at 0x005BDC94, then registers the "ComputerHilight" task type of size 0x4E0
//   with parent Sys_GetGlobal16_5034D0() -> id at 0x0053F6D0. It links FIVE handlers: e0->0x004684F0 (0x00401400), **event 2**->0x00468570
//   (0x00401530 - event 1 is skipped), 0x004F1A60->0x004685B0 and 0x004F1A70->0x004685C0 (0x00401400), and the lazily allocated event id from
//   0x00467D90->0x004687A0 (0x00401400). It allocates an EIGHT-entry parameter container through 0x004F1950 into 0x005BDC84 and appends eight
//   16-byte records through 0x004F1A80 with recorded deltas 0x20, 0x38, 0x8C, 0x88, 0x294, 0x2B8, 0x2DC and 0x3DC - the first pair of which is
//   out of ascending order, so the retail order is not sorted and is preserved. It finishes by building a task list with QTaskList_New(0x20) into
//   0x005BDC80. One basic block, no branch, no loop, 29 call sites over 16 distinct callees. There is NO 0x004B8810 script-name tail in this
//   body, and NO QTask_AllocateEvent call.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x0053F6D0 is declared std::uint16_t and initialised to 0x0181 because that is its
//   IMAGE VALUE, not a guess: the bytes before the name literal are 81 01 00 00. All six accesses to it in the image are 16-bit (0x00468330
//   `mov %ax,0x53F6D0`, 0x0046833B `mov 0x53F6D0,%ax`, 0x0046835B `%cx`, 0x00468376 `%dx`, 0x00468392 `%ax`, 0x0046887B `%cx`), so the width is
//   established the same way as the width of 0x005BDDA0 in core/native_4696b0.cpp. Because the image value is non-zero, the image-value rule makes
//   this a `D` object even though it sits in the 0x0053F6xx region among the name literals - a useful case: the section is not where the address
//   looks, it is what the image value says. The same value 0x0181 (385) is what the parent getters read: 0x005034D0 loads `[0x54E634]` and
//   0x004E0830 loads `[0x548698]`, and radare2 reports 385 at both. The meaning of 0x0181 is NOT established here; what is established is that
//   this id slot holds it in the image and that the parents of this lane's registrars hold it too. 0x005BDC90 holds image value 0, is written
//   0x13 by this body at 0x004682F6, and its ADDRESS is the fourth append's sixth argument - the sixth case of the adopted sixth-argument shape
//   (the address of a static object) and the only one in which the same body also writes a value into that object. 0x005BDC94 holds image value 0
//   and is written once, with 0x0046B4D0()+0x4B0; 0x0046B4D0 is `mov eax,0x5BDDB0; ret` and is already implemented in this tree as
//   Sys_GetBlock5BDDB0_46B4D0 (resource/native_46b4d0.cpp), so this file declares that symbol rather than duplicating it, and the +0x4B0 lands
//   beyond the 0x3C bytes that file proves, which means 0x005BDDB0 starts a larger region - recorded, not claimed. 0x005BDC84 holds image value 0
//   and is the container; the count 8 matches the eight appends. 0x005BDC80 holds image value 0 and is the task list. This registrar has no
//   QTask_AllocateEvent call, so its first cleanup is 0x2C and not the 0x3C/0x38 of the pickup family - the third independent measurement of the
//   derived-cleanup rule. The append deltas are independently corroborated by the e0 handler 0x004684F0, which addresses the same fields:
//   0x004684F6 `lea 0x38(%esi),%eax`, 0x004684FF `lea 0x294(%esi),%ecx`, 0x00468513 `mov %ebx,0x88(%esi)`, 0x0046850D `mov %bl,0x8c(%esi)`,
//   0x0046852A `lea 0x2b8(%esi),%edx`, 0x0046854F `mov %bl,0x2dc(%esi)` and 0x00468555 `mov %bl,0x3dc(%esi)` - seven of the eight deltas appear
//   as field accesses in the handler of the same type, which is the registrar-delta cross-check. The sixth argument of 0x004F1A80 is transcribed
//   here (0 at seven appends and 0x005BDC90 at the fourth) with the corrected six-argument shape: the callee reads arg1..arg4, arg5 as a byte
//   (record +0x0C) and arg6 as a dword (record +0x08). Documented shape differences, all value-preserving: (1) the getters are argument-less loads
//   in retail that ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not
//   reproduced; each is simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value in
//   the same order, and for the appends they are the values that become the append's sixth and fifth arguments; (2) byte-valued event ids are
//   zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530,
//   which mask the event word for the phase index; (3) retail's 0x4E0-byte frame is reproduced as a single local slot whose address stands in for
//   the frame base, the same convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0 files use -
//   only the two addresses passed to 0x004F1A80 matter and the callee stores their difference, and nothing in this body reads the intervening
//   bytes, not even for the largest delta 0x3DC; (4) the deferred cleanups are reproduced implicitly by the tree's calling convention rather than
//   by explicit `add esp` arithmetic, since the port never pushes the inert arguments; the retail sizes are recorded here so a future reviewer can
//   check the arithmetic. Handlers 0x004684F0, 0x00468570, 0x004685B0 and 0x004685C0 are raw code immediates in retail and are not transcribed
//   yet (native gaps in 0x0046xxxx-0x0048xxxx); 0x004687A0 is also in range. Domain: core/, because it is one of the 0x00402930 startup
//   registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/core/qtask_list.hpp"

#include <cstdint>

extern "C" std::uint8_t* Sys_GetBlock5BDDB0_46B4D0(void);         // 0x0046B4D0: `mov eax,0x5BDDB0; ret` (resource/native_46b4d0.cpp)
extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);            // 0x005034D0: `mov ax,[0x54E634]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FF0(void);            // 0x004F0FF0: `mov eax,[0xA7186C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F00(void);            // 0x004F0F00: `mov eax,[0xA70C70]; ret` (ignores both pushed arguments)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FE0(void);            // 0x004F0FE0: `mov eax,[0xA71888]; ret`
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90: lazily allocated event id (core/native_467d90.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053F6D0: the "ComputerHilight" task type id. Image value 0x0181, so it is initialised data (`D`) despite sitting four bytes before the name
// literal; all six accesses in the image are 16-bit.
std::uint16_t g_53f6d0 = 0x0181U;

// 0x005BDC90: image value 0. This body writes 0x13 into it at 0x004682F6 and passes its address as the fourth append's sixth argument.
std::uint32_t g_5bdc90 = 0U;

// 0x005BDC94: image value 0. This body writes (0x0046B4D0() + 0x4B0) into it at 0x0046830E.
std::uint32_t g_5bdc94 = 0U;

// 0x005BDC84: the eight-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5bdc84 = 0U;

// 0x005BDC80: the task list built by QTaskList_New(0x20). Image value 0, written and read as a dword.
std::uint32_t g_5bdc80 = 0U;

void Task_DefineComputerHilight()
{
    // 0x004682F0: sub esp,0x4E0 - retail's 1248-byte frame, which is the registered object size. The eight 0x004F1A80 calls below pass frame+0x00
    // and frame+0x20 / +0x38 / +0x8C / +0x88 / +0x294 / +0x2B8 / +0x2DC / +0x3DC; the callee stores the differences. A single local slot stands
    // in for the frame base, which is the convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0
    // files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004682F6: mov dword [0x5BDC90],0x13 - a 19 written into the object whose address is the fourth append's sixth argument.
    ::g_5bdc90 = 0x13U;

    // 0x00468300: call 0x46B4D0 (Sys_GetBlock5BDDB0_46B4D0); 0x00468305: add eax,0x4B0; 0x0046830E: mov [0x5BDC94],eax. The +0x4B0 lands beyond
    // the 0x3C bytes that resource/native_46b4d0.cpp proves for 0x005BDDB0, so that address starts a larger region.
    ::g_5bdc94 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Sys_GetBlock5BDDB0_46B4D0()) + 0x4B0U);

    // 0x0046830A/0x0046830C: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00468313: call 0x5034D0; 0x00468318: push eax; 0x00468319: push 0x4E0; 0x0046831E: push "ComputerHilight"; 0x00468323: call 0x401900.
    const std::uint16_t computer_hilight_id = TaskType_Register("ComputerHilight", // 0x0046831E: push 0x0053F6D4
                                                                0x4E0U,             // 0x00468319: push 0x4E0 (object size, equal to the frame)
                                                                Sys_GetGlobal16_5034D0(), // 0x00468313: call 0x5034D0 (parent type)
                                                                0U);               // 0x0046830C: push 0 (fourth argument)

    // 0x00468328: push 0x004684F0 (pre-pushed handler); 0x0046832D: push 0; 0x0046832F: push eax; 0x00468330: mov word [0x53F6D0],ax (the id
    // store lands between the pushes); 0x00468336: call 0x401400.
    ::g_53f6d0 = computer_hilight_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f6d0), // 0x00468330: mov ax,[0x53F6D0]
                                0U,                                     // 0x0046832D: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004684F0U))); // 0x00468328: push handler

    // 0x0046833B: mov ax,[0x53F6D0]; 0x00468341: push 0x00468570; 0x00468346: push 2; 0x00468348: push eax; 0x00468349: call 0x401530;
    // 0x0046834E: add esp,0x2C - eleven pushes: the two zeros, the parent-getter result, the size, the name, and the three pushes of each of the
    // first two links. NOTE the argument is 2, not 1: this registrar has NO event-1 handler and its first 0x00401530 link is event 2.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53f6d0), // 0x0046833B: mov ax,[0x53F6D0]
                                           2U,                                     // 0x00468346: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00468570U))); // 0x00468341: push handler

    // 0x00468351: push 0x004685B0; 0x00468356: call 0x4F1A60; 0x0046835B: mov cx,[0x53F6D0]; 0x00468362: push eax; 0x00468363: push ecx;
    // 0x00468364: call 0x401400; 0x00468369: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f6d0), // 0x0046835B: mov cx,[0x53F6D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00468362: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004685B0U))); // 0x00468351: push handler

    // 0x0046836C: push 0x004685C0; 0x00468371: call 0x4F1A70; 0x00468376: mov dx,[0x53F6D0]; 0x0046837D: push eax; 0x0046837E: push edx;
    // 0x0046837F: call 0x401400; 0x00468384: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f6d0), // 0x00468376: mov dx,[0x53F6D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046837D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004685C0U))); // 0x0046836C: push handler

    // 0x00468387: push 0x004687A0; 0x0046838C: call 0x467D90; 0x00468391: push eax; 0x00468392: mov ax,[0x53F6D0]; 0x00468398: push eax;
    // 0x00468399: call 0x401400. This link has no cleanup of its own: the next add esp,0x10 covers it together with the container argument.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53f6d0), // 0x00468392: mov ax,[0x53F6D0]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x00468391: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004687A0U))); // 0x00468387: push handler

    // 0x0046839E: push 8 (Script_TaskContainer_Alloc's argument); 0x004683A0: call 0x4F1950; 0x004683A5: add esp,0x10 - the link's three
    // pushes plus this argument; 0x004683A8: mov [0x5BDC84],eax. The count 8 matches the eight appends below.
    ::g_5bdc84 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(8));

    // Append 1: 0x004683AD/0x004683AF: push 0/7 (the append's sixth and fifth arguments); 0x004683B1: call 0x4F1010;
    // 0x004683B6: lea ecx,[esp+0x28] -> frame+0x20; 0x004683BA: push eax (the record type); 0x004683BB: mov eax,[0x5BDC84] (the container);
    // 0x004683C0: lea edx,[esp+0xC] -> frame+0x00; 0x004683C7: call 0x4F1A80; 0x004683CC: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x004683CF/0x004683D1: push 0/7; 0x004683D3: call 0x4F0FC0; 0x004683D8: lea ecx,[esp+0x40] -> frame+0x38;
    // 0x004683E9: call 0x4F1A80; 0x004683EE: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x38,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 3: 0x004683F1/0x004683F3: push 0/7; 0x004683F5: call 0x4F0FF0; 0x004683FA: lea ecx,[esp+0x94] -> frame+0x8C;
    // 0x0046840E: call 0x4F1A80; 0x00468413: add esp,0x18. The deltas are NOT in ascending order: 0x8C here, 0x88 next.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x8C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);

    // Append 4: 0x00468416: push 0x005BDC90 (the append's SIXTH argument - the address of the object written at 0x004682F6);
    // 0x0046841B: push 7; 0x0046841D: call 0x4F0F00; 0x00468422: lea ecx,[esp+0x90] -> frame+0x88; 0x00468429: push eax (the record type);
    // 0x00468436: call 0x4F1A80; 0x0046843B: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x88,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F00()), 7,
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_5bdc90)));

    // Append 5: 0x0046843E/0x00468440: push 0/7; 0x00468442: call 0x4F0FE0; 0x00468447: lea ecx,[esp+0x29C] -> frame+0x294;
    // 0x0046845B: call 0x4F1A80; 0x00468460: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x294,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 6: 0x00468463/0x00468465: push 0/7; 0x00468467: call 0x4F0FE0 (the same getter as append 5);
    // 0x0046846C: lea ecx,[esp+0x2C0] -> frame+0x2B8; 0x00468480: call 0x4F1A80; 0x00468485: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x2B8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 7: 0x00468488/0x0046848A: push 0/7; 0x0046848C: call 0x4F0FF0 (the same getter as append 3);
    // 0x00468491: lea ecx,[esp+0x2E4] -> frame+0x2DC; 0x004684A5: call 0x4F1A80; 0x004684AA: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x2DC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);

    // Append 8: 0x004684AD/0x004684AF: push 0/7; 0x004684B1: call 0x4F0FF0 (the same getter as appends 3 and 7);
    // 0x004684B6: lea ecx,[esp+0x3E4] -> frame+0x3DC; 0x004684CA: call 0x4F1A80. This append has no cleanup of its own.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc84)),
                                append_frame, append_frame + 0x3DC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FF0()), 7, 0);

    // 0x004684CF: push 0x20 (QTaskList_New's capacity); 0x004684D1: call 0x4C1800; 0x004684D6: mov [0x5BDC80],eax;
    // 0x004684DB: add esp,0x4FC - the 0x4E0 frame plus the last append's six pushes and this argument; 0x004684E1: ret.
    ::g_5bdc80 = reinterpret_cast<std::uint32_t>(QTaskList_New(0x20));
} // end of Task_DefineComputerHilight

} // extern "C"
