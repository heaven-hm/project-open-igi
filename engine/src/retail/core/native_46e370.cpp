// Native: Task_DefineAIStationaryGunHolder (behavior-derived placeholder name; retail symbol unknown); retail VA 0x0046E370-0x0046E5AF.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 576 bytes, 1 block, 165 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 165 instructions / 576 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x0046E370-0x0046E5AF; the ret is at 0x0046E5AF, nine nops follow, and
//   0x0046E5B0 is the next function, which this body uses as a handler); `ps` at 0x0053FEA8 for the name literal (byte-identical to
//   "AIStationaryGunHolder"); `pxw` at 0x005BE318 for the image value; a stack-pointer simulation over all 165 instructions in which every
//   `push`, `pop`, `add esp` and the `mov eax,0x26A0; call 0x004A54A0` allocation were applied in order, which returns esp to its entry value at
//   the ret (the last cleanup, `add esp,0x26BC` at 0x0046E5A9, is the 0x26A0 frame plus the final append's six pushes and the QTaskList_New
//   argument) and which resolved every `lea reg,[esp+N]` to its frame-relative address. Every callee was re-read at its own VA, including the
//   eight record appends' getters (all are `mov eax,[global]; ret` and read no argument). Argument numbers were derived by counting the pushes
//   above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body. It allocates a 0x26A0-byte frame with the MSVC 6 stack-probe helper (`mov eax,0x26A0; call 0x004A54A0`
//   at 0x0046E370), registers the "AIStationaryGunHolder" task type of size 0x26A0 with parent Sys_GetGlobal16_4C48C0() -> id at 0x005BE318,
//   links e0->0x0046E210 (0x00401400), e1->0x0046E5B0 and e2->0x0046E7A0 (0x00401530), then links five more handlers through 0x00401400
//   (0x004F1A70->0x0046E7F0, 0x004F1A60->0x0046E9D0, 0x00489630->0x0046E9E0, 0x00489610->0x0042FC50, 0x00467D90->0x0046E9F0). It then zeroes
//   0x005BE320, writes 0x42C80000 to 0x005BE324, allocates an EIGHT-entry parameter container through 0x004F1950 into 0x005BE328, appends eight
//   16-byte records through 0x004F1A80 with recorded deltas 0x2598, 0x25B0, 0x25D8, 0x25E8, 0x25EC, 0x25F0, 0x25F4 and 0x2644, and finishes by
//   allocating a 0x10-entry task list through QTaskList_New (0x004C1800) into 0x005BE330. One basic block, no branch, no loop, 34 call sites
//   over 19 distinct callees. There is NO 0x004B8810 script-name tail in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BE318 holds image value 0 and is one of the image-zero slots in the
//   0x005BE3xx cluster. The eight appends' container holds exactly eight records and is allocated with count 8, which agrees with the eight
//   append sites. The sixth argument of 0x004F1A80 is transcribed here: retail's callee reads SIX stack arguments, and its record stores put
//   arg6 at +0x08 and byte(arg5) at +0x0C; the tree's current implementation of 0x004F1A80 still declares five parameters and writes arg5 into
//   both fields, so this file passes the correct sixth argument but the field will stay wrong until that implementation is corrected (0x004F1A80
//   is above 0x0048FFFF and is not in this lane's scope). Because the port uses C linkage for this symbol, passing the extra argument links
//   against the five-parameter definition and the slot is ignored, so nothing here breaks the link; this is recorded rather than hidden. Seven
//   of the eight appends pass 0 as that sixth argument and one (0x0046E52E, the 0x004F0F50 append) passes 0x005BE320. The frame-relative
//   deltas were resolved rather than read: at the first append's `lea ecx,[esp+0x25A0]` only the two inert pushes are on the stack (8 bytes)
//   while at every other append's equivalent lea the getter result is already pushed (12 bytes), so the raw offsets 0x25A0 and 0x25BC are not
//   comparable as written and only the resolved values are. The 0x004A54A0 call is the MSVC 6 stack-probe helper (it walks one 0x1000 page per
//   iteration and lowers esp itself, and the matching `add esp,0x26BC` accounts for exactly 0x26A0 of frame), so it is a compiler-runtime
//   allocation rather than a game native; the tree's established convention for it, followed here, is that the C++ frame replaces it (see
//   terrain/terrain_draw.cpp at 0x00512C30), which is why this file's call table has 33 sites where retail has 34 - the one documented
//   difference. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the
//   arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next 0x00401400/0x00401530 link call's pre-pushed handler, which the port keeps as the handler value in the same order,
//   and for the eight appends they are the 0 and the 7 that become the append's sixth and fifth arguments; (2) byte-valued event ids are
//   zero-extended in the port, which cannot change behaviour because every link fed by a byte getter here goes through 0x00401400 or
//   0x00401530, which mask the event word for the phase index; (3) retail's 0x26A0-byte frame is reproduced as a single local slot whose address
//   stands in for the frame base, which is the same convention the accepted 0x004813E0 file uses - only the two addresses passed to 0x004F1A80
//   matter and the callee stores their difference, and nothing in this body reads the intervening bytes; (4) the one append whose sixth argument
//   is retail's address 0x005BE320 passes the port address of the global that mirrors it (`&::g_5be320`) instead of the retail constant, because
//   0x004F1A80 stores that argument as a value in the parameter record and a retail address there would leave a pointer into unmapped retail
//   .data inside the record. Handlers 0x0046E210, 0x0046E5B0,
//   0x0046E7A0, 0x0046E7F0, 0x0046E9D0, 0x0046E9E0 and 0x0046E9F0 are raw code immediates in retail and are not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx); handler 0x0042FC50 is BELOW 0x00460000 and belongs to the 0x00400000-0x0045FFFF owner. Domain: core/, because it
//   is one of the 0x00402930 startup registrars.
#include "igi/retail/core/qtask_list.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: `mov ax,[0x547F42]; ret` (parent type)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Sys_GetGlobal8_489630(void);              // 0x00489630: argument-less byte getter (core/native_489630.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_489610(void);              // 0x00489610: argument-less byte getter (core/native_489610.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90: lazily allocated event id (core/native_467d90.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F30(void);            // 0x004F0F30: `mov eax,[0xA70CA4]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret` (ignores the pushed 0x5BE320)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). Six arguments: the callee reads arg1..arg4, arg5 as a byte (stored at record +0x0C) and arg6 as a
// dword (stored at record +0x08). The tree's implementation still declares five and writes arg5 into both fields; see the status block above.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BE318: the "AIStationaryGunHolder" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5be318 = 0U;

// 0x005BE320: image value 0, zeroed by a 32-bit store at 0x0046E454 and passed as the sixth argument of the 0x004F0F50 append.
std::uint32_t g_5be320 = 0U;

// 0x005BE324: image value 0x42C80000, written at 0x0046E45E. The raw bit pattern is transcribed as retail stores it (0x42C80000 is the
// single-precision bit pattern for 100.0); its meaning is not established here.
std::uint32_t g_5be324 = 0x42C80000U;

// 0x005BE328: the eight-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5be328 = 0U;

// 0x005BE330: the 0x10-entry task list. Image value 0, written and read as a dword.
std::uint32_t g_5be330 = 0U;

void Task_DefineAIStationaryGunHolder()
{
    // 0x0046E370: `mov eax,0x26A0`; 0x0046E375: `call 0x4A54A0` - the MSVC 6 stack-probe helper allocates the 0x26A0-byte frame, which equals
    // the registered object size below. The C++ frame replaces it (tree convention, see terrain/terrain_draw.cpp at 0x00512C30). The single
    // local slot stands in for the frame base; the eight 0x004F1A80 calls pass frame+0 and frame+<delta>, and only their difference is stored.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0046E37A/0x0046E37C: push 0; push 0 (both inert arguments of the parent getter); 0x0046E37E: call 0x4C48C0; 0x0046E383: push eax;
    // 0x0046E384: push 0x26A0; 0x0046E389: push "AIStationaryGunHolder"; 0x0046E38E: call 0x401900.
    const std::uint16_t holder_id = TaskType_Register("AIStationaryGunHolder", // 0x0046E389: push 0x0053FEA8
                                                      0x26A0U, // 0x0046E384: push 0x26A0 (object size, equal to the frame)
                                                      Sys_GetGlobal16_4C48C0(), // 0x0046E37E: call 0x4C48C0 (parent type)
                                                      0U);    // 0x0046E37C: push 0 (inert)

    // 0x0046E393: push 0x0046E210 (pre-pushed handler); 0x0046E398: push 0; 0x0046E39A: push eax;
    // 0x0046E39B: mov word [0x5BE318],ax (the store lands between the pushes and the call); 0x0046E3A1: call 0x401400.
    ::g_5be318 = holder_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E3A6: mov ax,[0x5BE318]
                                0U,                                     // 0x0046E398: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046E210U))); // 0x0046E393: push handler

    // 0x0046E3AC: push 0x0046E5B0; 0x0046E3B1: push 1; 0x0046E3B3: push eax; 0x0046E3B4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be318), // 0x0046E3A6: mov ax,[0x5BE318]
                                           1U,                                     // 0x0046E3B1: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046E5B0U))); // 0x0046E3AC: push handler

    // 0x0046E3B9: mov cx,[0x5BE318]; 0x0046E3C0: push 0x0046E7A0; 0x0046E3C5: push 2; 0x0046E3C7: push ecx; 0x0046E3C8: call 0x401530;
    // 0x0046E3CD: add esp,0x38 - the fourteen pushes above (two inert, three registration, three per link).
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5be318), // 0x0046E3B9: mov cx,[0x5BE318]
                                           2U,                                     // 0x0046E3C5: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046E7A0U))); // 0x0046E3C0: push handler

    // 0x0046E3D0: push 0x0046E7F0; 0x0046E3D5: call 0x4F1A70; 0x0046E3DA: mov dx,[0x5BE318]; 0x0046E3E1: push eax; 0x0046E3E2: push edx;
    // 0x0046E3E3: call 0x401400; 0x0046E3E8: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E3DA: mov dx,[0x5BE318]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x0046E3E1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046E7F0U))); // 0x0046E3D0: push handler

    // 0x0046E3EB: push 0x0046E9D0; 0x0046E3F0: call 0x4F1A60; 0x0046E3F5: push eax; 0x0046E3F6: mov ax,[0x5BE318]; 0x0046E3FC: push eax;
    // 0x0046E3FD: call 0x401400; 0x0046E402: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E3F6: mov ax,[0x5BE318]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0046E3F5: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046E9D0U))); // 0x0046E3EB: push handler

    // 0x0046E405: push 0x0046E9E0; 0x0046E40A: call 0x489630; 0x0046E40F: mov cx,[0x5BE318]; 0x0046E416: push eax; 0x0046E417: push ecx;
    // 0x0046E418: call 0x401400; 0x0046E41D: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E40F: mov cx,[0x5BE318]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489630()), // 0x0046E416: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046E9E0U))); // 0x0046E405: push handler

    // 0x0046E420: push 0x0042FC50; 0x0046E425: call 0x489610; 0x0046E42A: mov dx,[0x5BE318]; 0x0046E431: push eax; 0x0046E432: push edx;
    // 0x0046E433: call 0x401400; 0x0046E438: add esp,0xc.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E42A: mov dx,[0x5BE318]
                                static_cast<std::uint32_t>(Sys_GetGlobal8_489610()), // 0x0046E431: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0042FC50U))); // 0x0046E420: push handler (below 0x00460000)

    // 0x0046E43B: push 0x0046E9F0; 0x0046E440: call 0x467D90; 0x0046E445: push eax; 0x0046E446: mov ax,[0x5BE318]; 0x0046E44C: push eax;
    // 0x0046E44D: call 0x401400. This link has no cleanup of its own.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5be318), // 0x0046E446: mov ax,[0x5BE318]
                                static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()), // 0x0046E445: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x0046E9F0U))); // 0x0046E43B: push handler

    // 0x0046E452: push 8 (Script_TaskContainer_Alloc's argument); 0x0046E454: mov dword [0x5BE320],0; 0x0046E45E: mov dword
    // [0x5BE324],0x42C80000; 0x0046E468: call 0x4F1950; 0x0046E46D: add esp,0x10 - the last link's three pushes plus this argument;
    // 0x0046E470: mov [0x5BE328],eax. The count 8 matches the eight appends below.
    ::g_5be320 = 0U;
    ::g_5be324 = 0x42C80000U;
    ::g_5be328 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(8));

    // The eight record appends. Each one pushes 0 and 7 (the append's sixth and fifth arguments) before calling an argument-less getter,
    // pushes the getter's result as the record type, then pushes frame+delta, frame+0 and the container. The `lea` offsets below are the
    // RESOLVED frame-relative values, not the raw [esp+N] immediates: at the first append the lea runs with 8 bytes pushed and at the others
    // with 12, so raw 0x25A0 and raw 0x25BC both resolve into the same progression.

    // 0x0046E475/0x0046E477: push 0/7; 0x0046E479: call 0x4F1010; 0x0046E47E: lea ecx,[esp+0x25A0] -> frame+0x2598; 0x0046E485: push eax;
    // 0x0046E48B: lea edx,[esp+0xC] -> frame+0x00; 0x0046E492: call 0x4F1A80; 0x0046E497: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x2598,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x0046E49A/0x0046E49C: push 0/7; 0x0046E49E: call 0x4F0F70; 0x0046E4A3: push eax; 0x0046E4A9: lea ecx,[esp+0x25BC] -> frame+0x25B0;
    // 0x0046E4B0: lea edx,[esp+0xC] -> frame+0x00; 0x0046E4B7: call 0x4F1A80; 0x0046E4BC: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25B0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x0046E4BF/0x0046E4C1: push 0/7; 0x0046E4C3: call 0x4F0FD0; 0x0046E4C8: lea ecx,[esp+0x25E0] -> frame+0x25D8;
    // 0x0046E4D5: lea edx,[esp+0xC] -> frame+0x00; 0x0046E4DC: call 0x4F1A80; 0x0046E4E1: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25D8,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x0046E4E4/0x0046E4E6: push 0/7; 0x0046E4E8: call 0x4F0F30; 0x0046E4ED: lea ecx,[esp+0x25F0] -> frame+0x25E8;
    // 0x0046E4FA: lea edx,[esp+0xC] -> frame+0x00; 0x0046E501: call 0x4F1A80; 0x0046E506: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25E8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x0046E509/0x0046E50B: push 0/7; 0x0046E50D: call 0x4F0F30 (the same getter as the previous append); 0x0046E512: lea ecx,[esp+0x25F4]
    // -> frame+0x25EC; 0x0046E51F: lea edx,[esp+0xC] -> frame+0x00; 0x0046E526: call 0x4F1A80; 0x0046E52B: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25EC,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F30()), 7, 0);

    // 0x0046E52E: push 0x005BE320 (this is the ONE append whose sixth argument is not 0); 0x0046E533: push 7; 0x0046E535: call 0x4F0F50;
    // 0x0046E53A: lea ecx,[esp+0x25F8] -> frame+0x25F0; 0x0046E547: lea edx,[esp+0xC] -> frame+0x00; 0x0046E54E: call 0x4F1A80;
    // 0x0046E553: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25F0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7,
                                static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_5be320))); // port address of retail 0x005BE320

    // 0x0046E556/0x0046E558: push 0/7; 0x0046E55A: call 0x4F0FC0; 0x0046E55F: lea ecx,[esp+0x25FC] -> frame+0x25F4;
    // 0x0046E56C: lea edx,[esp+0xC] -> frame+0x00; 0x0046E573: call 0x4F1A80; 0x0046E578: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x25F4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x0046E57B/0x0046E57D: push 0/7; 0x0046E57F: call 0x4F0FC0 (the same getter as the previous append); 0x0046E584: lea ecx,[esp+0x264C]
    // -> frame+0x2644; 0x0046E591: lea edx,[esp+0xC] -> frame+0x00; 0x0046E598: call 0x4F1A80. This append has no cleanup of its own.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5be328)),
                                append_frame, append_frame + 0x2644,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x0046E59D: push 0x10 (QTaskList_New's capacity); 0x0046E59F: call 0x4C1800; 0x0046E5A4: mov [0x5BE330],eax;
    // 0x0046E5A9: add esp,0x26BC - the 0x26A0 frame plus the last append's six pushes and this argument; 0x0046E5AF: ret.
    ::g_5be330 = reinterpret_cast<std::uint32_t>(QTaskList_New(0x10));
} // end of Task_DefineAIStationaryGunHolder

} // extern "C"
