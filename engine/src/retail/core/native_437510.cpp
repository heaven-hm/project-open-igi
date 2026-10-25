// Native: Task_DefineSmoke (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x00437510-0x00437ACE.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1471 bytes, 1 block, 370 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 370 instructions / 0 edges) plus the objdump
//   listing build/retail-dis.txt for the same extent, 0x00437510-0x00437ACE, instruction by instruction (the ret is at 0x00437ACE). `ps` at
//   0x0053C128 for the type name ("Smoke"), 0x0053C164 for the script name ("TASKTYPE_SMOKE"), 0x0053C174 for the model name ("1729_01_1"),
//   and 0x0053C154/0x0053C148/0x0053C13C/0x0053C130 for "fireball.spr"/"blood1.spr"/"smoke1.spr"/"smoke2.spr". `px 4` at all 35 globals this body
//   writes (0x0057BDC8 through 0x0057BE74): every image value is 0, so all of them are zero-initialised data (`B`). 0x005034D0 re-read at its own
//   VA (`mov ax,[0x54E634]; ret`, image 385 = 0x0181) for the parent type; 0x004CEBA0, 0x004169D0, 0x004C5800 and the 0x004F0xxx getters were
//   resolved by sweeping the VA against the tree's existing transcriptions before naming them, and each was re-read at its own VA. A push-by-push
//   simulation of the whole body: `sub esp,0x178`, all 370 instructions, every push and every `add esp` in order, closing exactly with the final
//   `add esp,0x1A8` = 0x178 frame + the last append's six pushes (0x18) + the 0x004B8810 registration's two pushes (0x8) + four 0x004169D0
//   lookups of one push each (0x10). The 21 append deltas and the 9 link sites were generated mechanically with assertions (esp exactly 0x18 below
//   the frame base at every 0x004F1A80 site; exactly one frame operand resolving to 0x00) and then re-checked against retail. One link in this body
//   has a NULL handler and could not be found by a handler-immediate scan; it was located by simulating esp and closing the arithmetic, and the
//   `add esp,0x1C` after the container allocation is what proves it: 0x1C = that link's three pushes plus the container count push, so the two
//   final links share one cleanup.
// Behavior/CFG: one straight-line body, 370 instructions, 65 call sites over 22 distinct callees, no branch and no loop. It allocates a 0x178-byte
//   frame with `sub esp,0x178` (no stack probe). Its first 29 stores initialise 28 dwords of the 0x0057BDD0-0x0057BE74 block - among them the
//   float bit patterns 0x40A00000 (5.0f) at 0x57BDFC's neighbour 0x57BE1C and 0x57BDD4, 0x43160000 (150.0f) at 0x57BE3C and its negation
//   0xC3160000 at 0x57BE38, 0x40490FDB (pi) at 0x57BE0C and 0x57BDDC with -pi at 0x57BDD8, 0x3F800000 (1.0f) at 0x57BE34, 0x40800000 (4.0f) at
//   0x57BE74 and 0x4062C000 at 0x57BE2C - and the value 3 into both 0x57BDE0 and 0x57BE54 from the same `mov eax,3`. It then calls
//   MTP_AddLODVirModel_4CEBA0("1729_01_1") and stores the returned pointer at 0x57BE40, registers the "Smoke" task type of size 0x178 with parent
//   Sys_GetGlobal16_5034D0() -> id at 0x0057BE00, links NINE handlers, allocates a 21-entry parameter container through
//   Script_TaskContainer_Alloc(0x15) into 0x0057BE10, appends 21 records through 0x004F1A80 with recorded deltas 0x28, 0x7C, 0x80, 0xF8, 0x94,
//   0x90, 0x88, 0x8C, 0x104, 0xFC, 0x100, 0x110, 0x118, 0x84, 0x114, 0x120, 0x11C, 0x124, 0x98, 0xA4 and 0x170, registers the script name
//   "TASKTYPE_SMOKE" against the masked id through 0x004B8810, and finishes with four Sys_LookupOrAppendRecord_4169D0 calls whose returned
//   pointers land in 0x57BDC8 ("fireball.spr"), 0x57BE44 ("blood1.spr"), 0x57BE4C ("smoke1.spr") and 0x57BE48 ("smoke2.spr").
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All 35 globals hold image value 0, so they are zero-initialised data (`B`); 0x0057BE00 is 16-bit
//   and the rest are dwords. The container is allocated with count 0x15 = 21 and there are exactly 21 appends. **One of the nine links registers a
//   NULL handler** (0x0043771F: `push 0; push 4; push cx; call 0x00401400`), so event id 4 is linked with no handler at all; that site is
//   transcribed with an explicit zero, and it is the reason the body's link count had to be established from the call census (0x00401400 x5,
//   0x00401530 x4) rather than from scanning for handler immediates. **Arg6 of 0x004F1A80 is per-site**: 0 at nine sites and the address of a
//   global at the other twelve (0x57BE08, 0x57BDD8, 0x57BE58, 0x57BE18, 0x57BDF8 twice, 0x57BE70 twice, 0x57BE68, 0x57BE50, 0x57BDD0,
//   0x57BE38, 0x57BE30, 0x57BE20) - so this body pushes initialised globals as the sixth argument, exactly like 0x00436780 and unlike
//   0x004330F0. The four lookups are pipelined in retail: each result is stored only after the next name has been pushed, so the store order is
//   0x57BDC8, 0x57BE44, 0x57BE4C, 0x57BE48 while the call order is fireball, blood1, smoke1, smoke2; the transcription keeps both orders
//   observable by writing them as four sequential store-from-call statements, and the push of the next name is not observable behaviour. The
//   0x0053C114 value stored at 0x57BDE4 is NOT a string pointer: the dword at 0x0053C114 is 0x0053C128, which is the address of "Smoke", so
//   0x57BDE4 receives the address of a data word rather than a text pointer, and it is transcribed as the literal 0x0053C114 with that
//   observation recorded rather than as a string. Deferred pushes: at every link site the handler immediate is pushed before the event-id getter
//   call and consumed as the link's third argument. Handler immediates 0x00437B90, 0x00437DB0, 0x00437D50, 0x00437DA0, 0x00438000, 0x00437CA0,
//   0x00437D00 and 0x00437AD0 are raw code immediates with no implementation in the tree yet. Documented shape differences, all value-preserving:
//   (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no
//   arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by
//   explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (four times), 0x18, 0x1C and 0x1A8 are recorded so a future reviewer can check the
//   arithmetic; (3) the 28 startup stores are transcribed as exact 32-bit bit patterns in `std::uint32_t` slots, because retail stores immediates
//   into dwords and no floating-point operation on those slots occurs in this body.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);            // 0x005034D0: `mov ax,[0x54E634]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t* MTP_AddLODVirModel_4CEBA0(const char* name); // 0x004CEBA0 (model/native_4ceba0.cpp)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name);   // 0x004169D0 (config/native_4169d0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);              // 0x004C5800: argument-less byte accessor (dynobj/native_4c5800.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F20(void);            // 0x004F0F20: `mov eax,[0xA71730]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F40(void);            // 0x004F0F40: `mov eax,[0xA71820]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Camera_Get_4F0F80(void);                 // 0x004F0F80: `mov eax,[0xA71904]; ret` (render/native_4f0f80.cpp)
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA71944]; ret` (core/native_4f0fa0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70D10]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// The 0x0057BDC8-0x0057BE74 block. Every image value is 0, so every slot below is zero-initialised data (`B`); this body is the only routine in
// the tree that references any of them, and it overwrites them at startup in the order recorded at each site.
std::uint32_t g_57bdc8 = 0U;
std::uint32_t g_57bdd0 = 0U;
std::uint32_t g_57bdd4 = 0U;
std::uint32_t g_57bdd8 = 0U;
std::uint32_t g_57bddc = 0U;
std::uint32_t g_57bde0 = 0U;
std::uint32_t g_57bde4 = 0U;
std::uint32_t g_57bdf8 = 0U;
std::uint32_t g_57bdfc = 0U;
std::uint32_t g_57be08 = 0U;
std::uint32_t g_57be0c = 0U;
std::uint32_t g_57be10 = 0U;
std::uint32_t g_57be18 = 0U;
std::uint32_t g_57be1c = 0U;
std::uint32_t g_57be20 = 0U;
std::uint32_t g_57be24 = 0U;
std::uint32_t g_57be28 = 0U;
std::uint32_t g_57be2c = 0U;
std::uint32_t g_57be30 = 0U;
std::uint32_t g_57be34 = 0U;
std::uint32_t g_57be38 = 0U;
std::uint32_t g_57be3c = 0U;
std::uint32_t g_57be40 = 0U;
std::uint32_t g_57be44 = 0U;
std::uint32_t g_57be48 = 0U;
std::uint32_t g_57be4c = 0U;
std::uint32_t g_57be50 = 0U;
std::uint32_t g_57be54 = 0U;
std::uint32_t g_57be58 = 0U;
std::uint32_t g_57be5c = 0U;
std::uint32_t g_57be68 = 0U;
std::uint32_t g_57be6c = 0U;
std::uint32_t g_57be70 = 0U;
std::uint32_t g_57be74 = 0U;
std::uint16_t g_57be00 = 0U; // the "Smoke" task type id; 16-bit

void Task_DefineSmoke()
{
    // 0x00437510: sub esp,0x178 - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00437516-0x0043762C: 29 stores into the 0x0057BDxx/0x0057BExx block, in retail's instruction order. 0x0043752A pushes the model name
    // that the 0x004CEBA0 call at 0x0043762E consumes; the push is argument materialisation only, so it is shown at the call site below and the
    // stores keep their retail order.
    ::g_57bde0 = 0x3U; // 0x437516: mov dword [0x57BDE0],0x3  // mov eax,3; 0x43751B: mov [0x57BDE0],eax
    ::g_57bde4 = 0x53C114U; // 0x437520: mov dword [0x57BDE4],0x53C114
    ::g_57bdf8 = 0x0U; // 0x43752F: mov dword [0x57BDF8],0x0
    ::g_57bdfc = 0x416A6000U; // 0x437539: mov dword [0x57BDFC],0x416A6000
    ::g_57be20 = 0x0U; // 0x437543: mov dword [0x57BE20],0x0
    ::g_57be24 = 0x0U; // 0x43754D: mov dword [0x57BE24],0x0
    ::g_57be28 = 0x0U; // 0x437557: mov dword [0x57BE28],0x0
    ::g_57be2c = 0x4062C000U; // 0x437561: mov dword [0x57BE2C],0x4062C000
    ::g_57be58 = 0x1U; // 0x43756B: mov dword [0x57BE58],0x1
    ::g_57be5c = 0xFFU; // 0x437575: mov dword [0x57BE5C],0xFF
    ::g_57be70 = 0x0U; // 0x43757F: mov dword [0x57BE70],0x0
    ::g_57be74 = 0x40800000U; // 0x437589: mov dword [0x57BE74],0x40800000
    ::g_57be18 = 0x0U; // 0x437593: mov dword [0x57BE18],0x0
    ::g_57be1c = 0x40A00000U; // 0x43759D: mov dword [0x57BE1C],0x40A00000
    ::g_57bdd0 = 0x0U; // 0x4375A7: mov dword [0x57BDD0],0x0
    ::g_57bdd4 = 0x40A00000U; // 0x4375B1: mov dword [0x57BDD4],0x40A00000
    ::g_57be38 = 0xC3160000U; // 0x4375BB: mov dword [0x57BE38],0xC3160000
    ::g_57be3c = 0x43160000U; // 0x4375C5: mov dword [0x57BE3C],0x43160000
    ::g_57be50 = 0x0U; // 0x4375CF: mov dword [0x57BE50],0x0
    ::g_57be54 = 0x3U; // 0x4375D9: mov dword [0x57BE54],0x3  // mov [0x57BE54],eax - eax still holds the 3 stored at 0x437516
    ::g_57be68 = 0x0U; // 0x4375DE: mov dword [0x57BE68],0x0
    ::g_57be6c = 0x2U; // 0x4375E8: mov dword [0x57BE6C],0x2
    ::g_57be08 = 0x0U; // 0x4375F2: mov dword [0x57BE08],0x0
    ::g_57be0c = 0x40490FDBU; // 0x4375FC: mov dword [0x57BE0C],0x40490FDB
    ::g_57bdd8 = 0xC0490FDBU; // 0x437606: mov dword [0x57BDD8],0xC0490FDB
    ::g_57bddc = 0x40490FDBU; // 0x437610: mov dword [0x57BDDC],0x40490FDB
    ::g_57be30 = 0x0U; // 0x43761A: mov dword [0x57BE30],0x0
    ::g_57be34 = 0x3F800000U; // 0x437624: mov dword [0x57BE34],0x3F800000

    // 0x0043752A: push 0x0053C174 (the model name, argument 1); 0x0043762E: call 0x4CEBA0; 0x00437633: add esp,0x4;
    // 0x00437636: mov [0x57BE40],eax - the returned pointer.
    ::g_57be40 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        MTP_AddLODVirModel_4CEBA0("1729_01_1"))); // 0x0043752A: push 0x0053C174

    // 0x0043763B: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043763D: push 0 (the parent getter's inert argument);
    // 0x0043763F: call 0x5034D0; 0x00437644: push eax; 0x00437645: push 0x178; 0x0043764A: push "Smoke"; 0x0043764F: call 0x401900.
    const std::uint16_t smoke_id = TaskType_Register("Smoke",                    // 0x0043764A: push 0x0053C128
                                                     0x178U,                     // 0x00437645: push 0x178 (object size, equal to the frame)
                                                     Sys_GetGlobal16_5034D0(),   // 0x0043763F: call 0x5034D0 (parent type)
                                                     0U);                        // 0x0043763B: push 0 (fourth argument)
    ::g_57be00 = smoke_id; // 0x0043765C: mov word [0x57BE00],ax - between the pushes of the first link

    // 0x437654: push 0x00437B90 (handler, deferred argument 3); 0x437659: push 0x0 (argument 2); 0x437662: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be00), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437B90U)));

    // 0x43766D: push 0x00437DB0 (handler, deferred argument 3); 0x437672: push 0x1 (argument 2); 0x437675: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be00), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437DB0U)));

    // 0x437681: push 0x00437D50 (handler, deferred argument 3); 0x437686: push 0x2 (argument 2); 0x437689: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be00), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437D50U)));

    // 0x437691: push 0x00437DA0 (handler, deferred argument 3); 0x437696: call 0x4F1A60 -> argument 2; 0x4376A4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be00), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437DA0U)));

    // 0x4376AC: push 0x00438000 (handler, deferred argument 3); 0x4376B1: call 0x4F1A70 -> argument 2; 0x4376BE: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be00), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00438000U)));

    // 0x4376C6: push 0x00437CA0 (handler, deferred argument 3); 0x4376CB: call 0x4F1210 -> argument 2; 0x4376D9: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be00), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437CA0U)));

    // 0x4376E1: push 0x00437D00 (handler, deferred argument 3); 0x4376E6: call 0x4F1220 -> argument 2; 0x4376F4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57be00), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437D00U)));

    // 0x4376FC: push 0x00437AD0 (handler, deferred argument 3); 0x437701: call 0x4C5800 -> argument 2; 0x43770E: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be00), static_cast<std::uint32_t>(DynObj_GetByte_4C5800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00437AD0U)));

    // 0x00437713: mov cx,[0x57BE00]; 0x0043771A: push 0x0 (argument 3 = a NULL handler, the only link in this body with no handler at all);
    // 0x0043771C: push 0x4 (argument 2 = the event id); 0x0043771E: push ecx (argument 1); 0x0043771F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57be00), 4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U)));

    // 0x00437724: push 0x15 (the container entry count, exactly the number of appends below); 0x00437726: call 0x4F1950;
    // 0x0043772B: add esp,0x1C - this link's three pushes plus the previous link's three (0x18) plus this count push; 0x0043772E: mov [0x57BE10],eax.
    ::g_57be10 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x15)); // 0x00437724: push 0x15

    // 0x437733: push $0x0 (arg6); 0x437737: call Sys_GetGlobal32_4F1010 (arg4); 0x43774E: call 0x004F1A80 (frame+0x00, frame+0x28).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x28,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x437756: push $0x57be08 (arg6); 0x43775D: call Sys_GetGlobal32_4F0F20 (arg4); 0x437777: call 0x004F1A80 (frame+0x00, frame+0x7C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x7C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be08)));

    // 0x43777F: push $0x57bdd8 (arg6); 0x437786: call Sys_GetGlobal32_4F0F20 (arg4); 0x4377A0: call 0x004F1A80 (frame+0x00, frame+0x80).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x80,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bdd8)));

    // 0x4377A8: push $0x57be58 (arg6); 0x4377AF: call Core_GetRenderWordA_4F0FA0 (arg4); 0x4377C9: call 0x004F1A80 (frame+0x00, frame+0xF8).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0xF8,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be58)));

    // 0x4377D1: push $0x57be18 (arg6); 0x4377D8: call Sys_GetGlobal32_4F0F50 (arg4); 0x4377F2: call 0x004F1A80 (frame+0x00, frame+0x94).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be18)));

    // 0x4377FA: push $0x0 (arg6); 0x4377FE: call Sys_GetGlobal32_4F0F20 (arg4); 0x437818: call 0x004F1A80 (frame+0x00, frame+0x90).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x90,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x437820: push $0x57bdf8 (arg6); 0x437827: call Sys_GetGlobal32_4F0F50 (arg4); 0x437841: call 0x004F1A80 (frame+0x00, frame+0x88).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x88,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bdf8)));

    // 0x437849: push $0x57bdf8 (arg6); 0x437850: call Sys_GetGlobal32_4F0F50 (arg4); 0x43786A: call 0x004F1A80 (frame+0x00, frame+0x8C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x8C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bdf8)));

    // 0x437872: push $0x0 (arg6); 0x437876: call Sys_GetGlobal32_4F0F40 (arg4); 0x437890: call 0x004F1A80 (frame+0x00, frame+0x104).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x104,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F40()), 7, 0);

    // 0x437898: push $0x57be70 (arg6); 0x43789F: call Sys_GetGlobal32_4F0F50 (arg4); 0x4378B9: call 0x004F1A80 (frame+0x00, frame+0xFC).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0xFC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be70)));

    // 0x4378C1: push $0x57be70 (arg6); 0x4378C8: call Sys_GetGlobal32_4F0F50 (arg4); 0x4378E2: call 0x004F1A80 (frame+0x00, frame+0x100).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x100,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be70)));

    // 0x4378EA: push $0x57be68 (arg6); 0x4378F1: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43790B: call 0x004F1A80 (frame+0x00, frame+0x110).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x110,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be68)));

    // 0x437913: push $0x57be50 (arg6); 0x43791A: call Core_GetRenderWordA_4F0FA0 (arg4); 0x437934: call 0x004F1A80 (frame+0x00, frame+0x118).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x118,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be50)));

    // 0x43793C: push $0x57bdd0 (arg6); 0x437943: call Sys_GetGlobal32_4F0F50 (arg4); 0x43795D: call 0x004F1A80 (frame+0x00, frame+0x84).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x84,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bdd0)));

    // 0x437965: push $0x57be38 (arg6); 0x43796C: call Sys_GetGlobal32_4F0F50 (arg4); 0x437986: call 0x004F1A80 (frame+0x00, frame+0x114).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x114,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be38)));

    // 0x43798E: push $0x0 (arg6); 0x437992: call Sys_GetGlobal32_4F0F20 (arg4); 0x4379AC: call 0x004F1A80 (frame+0x00, frame+0x120).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x120,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x4379B4: push $0x0 (arg6); 0x4379B8: call Sys_GetGlobal32_4F0F20 (arg4); 0x4379D2: call 0x004F1A80 (frame+0x00, frame+0x11C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x11C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x4379DA: push $0x57be30 (arg6); 0x4379E1: call Sys_GetGlobal32_4F0F50 (arg4); 0x4379FB: call 0x004F1A80 (frame+0x00, frame+0x124).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x124,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be30)));

    // 0x437A03: push $0x57be20 (arg6); 0x437A0A: call Camera_Get_4F0F80 (arg4); 0x437A24: call 0x004F1A80 (frame+0x00, frame+0x98).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x98,
                                static_cast<std::int32_t>(Camera_Get_4F0F80()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57be20)));

    // 0x437A2C: push $0x0 (arg6); 0x437A30: call Sys_GetGlobal32_4F0FC0 (arg4); 0x437A4A: call 0x004F1A80 (frame+0x00, frame+0xA4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0xA4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x437A52: push $0x0 (arg6); 0x437A56: call Camera_Get_4F1000 (arg4); 0x437A70: call 0x004F1A80 (frame+0x00, frame+0x170).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be10)),
                                append_frame, append_frame + 0x170,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);
    // 0x00437A75: mov edx,[0x57BE00]; 0x00437A7B: and edx,0xFFFF; 0x00437A81: push edx (argument 2); 0x00437A82: push 0x0053C164 (argument 1);
    // 0x00437A87: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_SMOKE"), // 0x00437A82: push 0x0053C164
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57be00) & 0xFFFFU))); // 0x00437A81: push edx

    // 0x00437A8C-0x00437AC3: four record lookups. Retail pushes the next name before storing the previous result, so the stores are
    // 0x57BDC8, 0x57BE44, 0x57BE4C and 0x57BE48 while the lookups are fireball.spr, blood1.spr, smoke1.spr, smoke2.spr.
    ::g_57bdc8 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("fireball.spr"))); // 0x00437A8C: push 0x0053C154; 0x00437A91: call 0x4169D0
    ::g_57be44 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("blood1.spr")));   // 0x00437A96: push 0x0053C148; 0x00437AA0: call 0x4169D0
    ::g_57be4c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("smoke1.spr")));   // 0x00437AA5: push 0x0053C13C; 0x00437AAF: call 0x4169D0
    ::g_57be48 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("smoke2.spr")));   // 0x00437AB4: push 0x0053C130; 0x00437ABE: call 0x4169D0

    // 0x00437AC8: add esp,0x1A8 - the 0x178 frame plus the last append's six pushes (0x18) plus this registration's two pushes (0x8) plus the
    // four lookups' one push each (0x10).
} // end of Task_DefineSmoke

} // extern "C"
