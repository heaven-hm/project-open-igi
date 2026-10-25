// Native: Task_DefineGenericPickup (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046C0C0-0x0046C2A5.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 486 bytes, 1 block, 134 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 134 instructions / 486 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046C0C0-0x0046C2A5; the ret is at 0x0046C2A5, ten nops follow, and
//   0x0046C2B0 is the next function, which this body uses as a handler); `ps` at 0x0053FDB8 for the name literal and `p8 24 @ 0x0053FDB8` for
//   its bytes (47656e657269635069636b7570 00 = "GenericPickup", byte-identical to the literal below); `pxw` at 0x005BE2D0 and 0x005BE2D4 for the
//   image values; a stack-pointer simulation over all 134 instructions, which returns esp to its entry value at the ret and accounts for every
//   cleanup (0x38 = fourteen pushes: the two inert arguments, the three registration arguments and three three-push links; 0xC nine times; 0x10
//   once = the 0x4CEA10 link's three pushes plus the container count; 0x138 = the 0x120 frame plus the last append's six pushes) and which
//   resolved all three `lea reg,[esp+N]` pairs to frame-relative addresses. Every callee was re-read at its own VA, which is how 0x00416850,
//   0x00416880, 0x004167E0, 0x004F1210, 0x004F1220, 0x004CEA10, 0x004CEA20 and 0x00467D90 were established as argument-less loads despite being
//   called with pushes. Argument numbers were derived by counting the pushes above each access, not by reading an [esp+N] offset as an argument
//   number.
// Behavior/CFG: one straight-line body with a 0x120-byte frame. It registers the "GenericPickup" task type of size 0x120 with parent
//   Sys_GetGlobal16_4E0830() -> id at 0x005BE2D0, then links THIRTEEN handlers: e0->0x0046C2B0 (0x00401400), e1->0x0046C3C0 and e2->0x0046C410
//   (0x00401530), 0x004F1A60->0x0046C480 and 0x004F1A70->0x0046C490 (0x00401400), 0x004F1210->0x0046C570 and 0x004F1220->0x0046C5D0
//   (0x00401530), three more through 0x00401400 whose event ids come from the argument-less byte getters 0x00416850, 0x00416880 and 0x004167E0
//   and whose handlers are 0x0045F920 (twice, the second event of the same handler) and 0x0046C630, one through 0x00401400 whose event id comes
//   from the lazily allocated 0x00467D90 and whose handler is 0x00443DC0 (below 0x00460000), and two more through 0x00401400 whose event ids come
//   from the argument-less byte getters 0x004CEA20 and 0x004CEA10 and whose handlers are 0x0046C8A0 and 0x0046C750. It allocates a THREE-entry
//   parameter container through 0x004F1950 into 0x005BE2D4 and appends three 16-byte records through 0x004F1A80 with recorded deltas 0xF0, 0x70
//   and 0x108. One basic block, no branch, no loop, 32 call sites over 19 distinct callees. There is NO 0x004B8810 script-name tail in this
//   body, and NO QTask_AllocateEvent call: this registrar allocates no event id of its own and has no byte store.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BE2D0 and the container slot 0x005BE2D4 both hold image value 0 and are the
//   low end of the 0x005BE2xx cluster that also holds 0x0046CB70's 0x005BE2E0/0x005BE2F8 and 0x0046D420's 0x005BE300/0x005BE304. The container is
//   allocated with count 3 and exactly three records are appended. This body is the third member of the pickup family in this lane - with the
//   accepted 0x0046D420 ("AmmoPickup", size 0x138, four records) and 0x0046CB70 ("GunPickup", size 0x158, three records) it shares the parent
//   0x004E0830, the event getters 0x004F1A60/0x004F1A70/0x004F1210/0x004F1220, the byte getters 0x00416850/0x00416880/0x004167E0 with the
//   0x0045F920 handler and the first two record deltas 0xF0/0x70 - but its size, its link set (thirteen links here, ten in each sibling) and its
//   third record differ, and unlike both siblings it allocates no event id. That is the same lesson the two siblings demonstrated: no value may be
//   carried across these bodies. The 0x38 first cleanup is the direct confirmation that cleanup constants are derived: it covers FOURTEEN pushes
//   and the two siblings' otherwise identical 0x3C covers fifteen, the extra word being the QTask_AllocateEvent argument they push and this body
//   does not. The third record's type is the plain argument-less load 0x004F0FD0 and its sixth argument is 0. The sixth argument of 0x004F1A80 is
//   transcribed here (0 at all three appends) with the corrected six-argument shape: the callee reads arg1..arg4, arg5 as a byte (record +0x0C)
//   and arg6 as a dword (record +0x08). Documented shape differences, all value-preserving: (1) the getters are argument-less loads in retail that
//   ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value in the same order, and
//   for the three appends they are the values that become the append's sixth and fifth arguments; (2) byte-valued event ids are zero-extended in
//   the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index; (3) retail's 0x120-byte frame is reproduced as a single local slot whose address stands in for the frame base,
//   the same convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420 and 0x0046CB70 files use - only the two addresses passed to
//   0x004F1A80 matter and the callee stores their difference, and nothing in this body reads the intervening bytes. Handlers 0x0046C2B0,
//   0x0046C3C0, 0x0046C410, 0x0046C480, 0x0046C490, 0x0046C570, 0x0046C5D0, 0x0046C630, 0x0046C750 and 0x0046C8A0 are raw code immediates in
//   retail and are not transcribed yet (native gaps in 0x0046xxxx-0x0048xxxx; 0x0046C2B0, 0x0046C3C0, 0x0046C410, 0x0046C480, 0x0046C490 and
//   0x0046C750 were read only far enough to confirm they are separate functions with their own extents); 0x00443DC0 and 0x0045F920 lie below
//   0x00460000 (other lane). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);            // 0x004E0830: `mov ax,[0x548698]; ret` (parent type)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (event id)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (event id)
extern "C" std::uint8_t Sys_GetSettingByte_416850(void);          // 0x00416850: `mov al,[0x539548]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);          // 0x00416880: `mov al,[0x53954D]; ret` (event id; ignores its push)
extern "C" std::uint8_t Sys_GetSettingByte_4167E0(void);          // 0x004167E0: `mov al,[0x539541]; ret` (event id; ignores its push)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90: lazily allocated event id (core/native_467d90.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void);      // 0x004CEA20: argument-less byte getter (script/native_4cea20.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void);      // 0x004CEA10: argument-less byte getter (script/native_4cea10.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret`
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BE2D0: the "GenericPickup" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be2d0 = 0U;

// 0x005BE2D4: the three-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be2d4 = 0U;

void Task_DefineGenericPickup()
{
    // 0x0046C0C0: sub esp,0x120 - retail's 288-byte frame, which is the registered object size. The three 0x004F1A80 calls below pass frame+0x00
    // and frame+0xF0 / +0x70 / +0x108; the callee stores the differences. A single local slot stands in for the frame base, which is the
    // convention the accepted 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420 and 0x0046CB70 files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0046C0C6/0x0046C0C8: push 0; push 0 (both inert arguments of the parent getter); 0x0046C0CA: call 0x4E0830; 0x0046C0CF: push eax;
    // 0x0046C0D0: push 0x120; 0x0046C0D5: push "GenericPickup"; 0x0046C0DA: call 0x401900.
    const std::uint16_t generic_pickup_id = TaskType_Register("GenericPickup", // 0x0046C0D5: push 0x0053FDB8
                                                              0x120U,           // 0x0046C0D0: push 0x120 (object size, equal to the frame)
                                                              Sys_GetGlobal16_4E0830(), // 0x0046C0CA: call 0x4E0830 (parent type)
                                                              0U);              // 0x0046C0C8: push 0 (inert)

    // 0x0046C0DF: push 0x0046C2B0 (pre-pushed handler); 0x0046C0E4: push 0; 0x0046C0E6: push eax; 0x0046C0E7: mov word [0x5BE2D0],ax (the id
    // store lands between the pushes); 0x0046C0ED: call 0x401400.
    ::g_5be2d0 = generic_pickup_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C0E7: mov ax,[0x5BE2D0]
                                0U,                                     // 0x0046C0E4: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C2B0U))); // 0x0046C0DF: push handler

    // 0x0046C0F2: mov ax,[0x5BE2D0]; 0x0046C0F8: push 0x0046C3C0; 0x0046C0FD: push 1; 0x0046C0FF: push eax; 0x0046C100: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C0F2: mov ax,[0x5BE2D0]
                                           1U,                                     // 0x0046C0FD: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046C3C0U))); // 0x0046C0F8: push handler

    // 0x0046C105: mov cx,[0x5BE2D0]; 0x0046C10C: push 0x0046C410; 0x0046C111: push 2; 0x0046C113: push ecx; 0x0046C114: call 0x401530;
    // 0x0046C119: add esp,0x38 - FOURTEEN pushes: the two inert arguments, the three registration arguments and three three-push links. The
    // sibling registrars' otherwise identical 0x3C covers fifteen because they also push the QTask_AllocateEvent argument; this body does not
    // allocate an event id, so its constant is one word smaller.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C105: mov cx,[0x5BE2D0]
                                           2U,                                     // 0x0046C111: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046C410U))); // 0x0046C10C: push handler

    // 0x0046C11C: push 0x0046C480; 0x0046C121: call 0x4F1A60; 0x0046C126: mov dx,[0x5BE2D0]; 0x0046C12D: push eax; 0x0046C12E: push edx;
    // 0x0046C12F: call 0x401400; 0x0046C134: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C126: mov dx,[0x5BE2D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0046C12D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C480U))); // 0x0046C11C: push handler

    // 0x0046C137: push 0x0046C490; 0x0046C13C: call 0x4F1A70; 0x0046C141: push eax; 0x0046C142: mov ax,[0x5BE2D0]; 0x0046C148: push eax;
    // 0x0046C149: call 0x401400; 0x0046C14E: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C142: mov ax,[0x5BE2D0]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046C141: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C490U))); // 0x0046C137: push handler

    // 0x0046C151: push 0x0046C570; 0x0046C156: call 0x4F1210; 0x0046C15B: mov cx,[0x5BE2D0]; 0x0046C162: push eax; 0x0046C163: push ecx;
    // 0x0046C164: call 0x401530; 0x0046C169: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C15B: mov cx,[0x5BE2D0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x0046C162: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046C570U))); // 0x0046C151: push handler

    // 0x0046C16C: push 0x0046C5D0; 0x0046C171: call 0x4F1220; 0x0046C176: mov dx,[0x5BE2D0]; 0x0046C17D: push eax; 0x0046C17E: push edx;
    // 0x0046C17F: call 0x401530; 0x0046C184: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C176: mov dx,[0x5BE2D0]
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x0046C17D: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046C5D0U))); // 0x0046C16C: push handler

    // 0x0046C187: push 0x0045F920 (the handler, and the inert argument of the byte getter); 0x0046C18C: call 0x416850;
    // 0x0046C191: push eax; 0x0046C192: mov ax,[0x5BE2D0]; 0x0046C198: push eax; 0x0046C199: call 0x401400; 0x0046C19E: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C192: mov ax,[0x5BE2D0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416850()), // 0x0046C191: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046C187: push handler (below 0x00460000)

    // 0x0046C1A1: push 0x0045F920 (the same handler again, for its second event); 0x0046C1A6: call 0x416880; 0x0046C1AB: mov cx,[0x5BE2D0];
    // 0x0046C1B2: push eax; 0x0046C1B3: push ecx; 0x0046C1B4: call 0x401400; 0x0046C1B9: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C1AB: mov cx,[0x5BE2D0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416880()), // 0x0046C1B2: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0045F920U))); // 0x0046C1A1: push handler (below 0x00460000)

    // 0x0046C1BC: push 0x0046C630; 0x0046C1C1: call 0x4167E0; 0x0046C1C6: mov dx,[0x5BE2D0]; 0x0046C1CD: push eax; 0x0046C1CE: push edx;
    // 0x0046C1CF: call 0x401400; 0x0046C1D4: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C1C6: mov dx,[0x5BE2D0]
                                static_cast<std::uint32_t>(Sys_GetSettingByte_4167E0()), // 0x0046C1CD: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C630U))); // 0x0046C1BC: push handler

    // 0x0046C1D7: push 0x00443DC0 (the handler, below 0x00460000, and the inert argument of 0x467D90); 0x0046C1DC: call 0x467D90;
    // 0x0046C1E1: push eax; 0x0046C1E2: mov ax,[0x5BE2D0]; 0x0046C1E8: push eax; 0x0046C1E9: call 0x401400; 0x0046C1EE: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C1E2: mov ax,[0x5BE2D0]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x0046C1E1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00443DC0U))); // 0x0046C1D7: push handler (below 0x00460000)

    // 0x0046C1F1: push 0x0046C8A0; 0x0046C1F6: call 0x4CEA20; 0x0046C1FB: mov cx,[0x5BE2D0]; 0x0046C202: push eax; 0x0046C203: push ecx;
    // 0x0046C204: call 0x401400; 0x0046C209: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C1FB: mov cx,[0x5BE2D0]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()), // 0x0046C202: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C8A0U))); // 0x0046C1F1: push handler

    // 0x0046C20C: push 0x0046C750; 0x0046C211: call 0x4CEA10; 0x0046C216: mov dx,[0x5BE2D0]; 0x0046C21D: push eax; 0x0046C21E: push edx;
    // 0x0046C21F: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be2d0), // 0x0046C216: mov dx,[0x5BE2D0]
                                static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()), // 0x0046C21D: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046C750U))); // 0x0046C20C: push handler

    // 0x0046C224: push 3 (Script_TaskContainer_Alloc's argument); 0x0046C226: call 0x4F1950;
    // 0x0046C22B: add esp,0x10 - the last link's three pushes plus this argument; 0x0046C22E: mov [0x5BE2D4],eax. The count 3 matches the
    // three appends below.
    ::g_5be2d4 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(3));

    // Append 1: 0x0046C233/0x0046C235: push 0/7 (the append's sixth and fifth arguments); 0x0046C237: call 0x4F1010;
    // 0x0046C23C: mov edx,[0x5BE2D4] (the container); 0x0046C242: push eax (the record type); 0x0046C243: lea eax,[esp+0xFC] -> frame+0xF0;
    // 0x0046C24A: lea ecx,[esp+0xC] -> frame+0x00; 0x0046C24E/0x0046C24F/0x0046C250: the three argument pushes;
    // 0x0046C251: call 0x4F1A80; 0x0046C256: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2d4)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x0046C259/0x0046C25B: push 0/7; 0x0046C25D: call 0x4F0F70; 0x0046C262: mov edx,[0x5BE2D4];
    // 0x0046C269: lea eax,[esp+0x7C] -> frame+0x70; 0x0046C26D: lea ecx,[esp+0xC] -> frame+0x00; 0x0046C274: call 0x4F1A80;
    // 0x0046C279: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2d4)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // Append 3: 0x0046C27C/0x0046C27E: push 0/7; 0x0046C280: call 0x4F0FD0; 0x0046C285: mov edx,[0x5BE2D4];
    // 0x0046C28C: lea eax,[esp+0x114] -> frame+0x108; 0x0046C293: lea ecx,[esp+0xC] -> frame+0x00; 0x0046C29A: call 0x4F1A80. This append has
    // no cleanup of its own.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be2d4)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x0046C29F: add esp,0x138 - the 0x120 frame plus the last append's six pushes; 0x0046C2A5: ret.
} // end of Task_DefineGenericPickup

} // extern "C"
