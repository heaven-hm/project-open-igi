// Native: Task_DefineAlarmLightAndReflector (behavior-derived placeholder name: this ONE body registers BOTH the "AlarmLight" and the "AlarmLightReflector" task types, and the retail symbol is unknown); retail VA 0x004397C0-0x00439A4C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 653 bytes, 5 basic blocks, 185 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 5 blocks: 0x004397C0-0x0043995B (114 instrs, falls through
//   to the first loop), 0x0043995C-0x00439980 (14 instrs, jumps back to itself, exits to 0x00439981), 0x00439981-0x0043998C (2 instrs), then
//   0x0043998D-0x004399B0 (14 instrs, jumps back to itself, exits to 0x004399B1) and 0x004399B1-0x00439A4C (41 instrs, ends in ret) - plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction. `ps` at 0x0053C2B4 ("AlarmLight"), 0x0053C2A0
//   ("TASKTYPE_ALARMLIGHT"), 0x0053C28C ("AlarmLightReflector"), 0x0053C26C ("TASKTYPE_ALARMLIGHTREFLECTOR") and 0x0053C25C ("alarmlight.spr");
//   `px 4` at 0x0057BE9C, 0x0057BEA0, 0x0057BEA2 and 0x0057BEA4 (all image value 0). 0x004E0830 (`mov ax,[0x548698]; ret`, image 385) and
//   0x00416860 (`mov al,[0x53954B]; ret`, image 255) were re-read at their own VAs and swept against the tree before being named; both are missing
//   from the tree and are named here by the tree's existing conventions for their shape (a 16-bit parent getter and a byte setting getter).
//   A full push-by-push simulation of all five blocks, including both loops, closing exactly: the first registration's `add esp,0x38` is fourteen
//   pushes, the container's `add esp,0x18` is the last link's three plus the registration's two plus the count push, the second registration's
//   `add esp,0x44` is seventeen pushes, the hash+lookup `add esp,0xC` is three, and the epilogue `add esp,0x1F8` matches the prologue's
//   `sub esp,0x1F8` with `pop edi`/`pop esi` restoring the two callee-saved registers the loops use.
// Behavior/CFG: this single retail function registers TWO task types and contains TWO loops; it is not a straight-line registrar.
//   Prologue: `sub esp,0x1F8` (a plain frame, NO stack probe) then `push esi; push edi`, so the two loop registers are saved below the frame and
//   every frame-relative `lea` in this body is offset by those eight bytes - the second argument of each append resolves to frame+0x00 only when
//   that offset is taken into account.
//   FIRST TYPE ("AlarmLight", size 0x1F8, parent Sys_GetGlobal16_4E0830(), id at 0x0057BEA2): nine links - e0->0x0043A0E0, e1->0x0043A270 and
//   e2->0x0043A3C0 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043A440, 0x004F1A70->0x0043A450, 0x00416840->0x0043A5E0,
//   0x00416860->0x0043A660, 0x004F1210->0x0043A710 and 0x004F1220->0x0043A790. It registers "TASKTYPE_ALARMLIGHT" against the masked id, allocates
//   an EIGHT-entry parameter container through Script_TaskContainer_Alloc(8) into 0x0057BE9C, appends three records directly (deltas 0xF0, 0x108
//   and 0x1A4) and then appends FIVE MORE from two loops: the first loops three times from frame+0x130 stepping 0x10 (0x130, 0x140, 0x150) and the
//   second loops twice from frame+0x160 stepping 0x20 (0x160, 0x180). 3 + 3 + 2 = 8 records, which is exactly the container's count - the count is
//   what confirms the two loop trip counts independently of the loop registers.
//   SECOND TYPE ("AlarmLightReflector", size 0x140, parent Sys_GetGlobal16_4D9FA0() = image 385, id at 0x0057BEA0): four links - e0->0x00439A50,
//   e1->0x00439B70 and e2->0x00426860 with the literal event ids 0, 1 and 2, then event id 4 -> 0x00439EC0 through 0x00401400. It registers
//   "TASKTYPE_ALARMLIGHTREFLECTOR" against the masked id and finishes with one Sys_LookupOrAppendRecord_4169D0("alarmlight.spr") whose returned
//   pointer lands in 0x0057BEA4.
//   Call census: 37 call sites over 19 distinct callees - 0x00401400 x7, 0x00401530 x6, 0x00401900 x2, 0x004B8810 x2, 0x004F1A80 x5,
//   0x004F0FD0 x2, and one each of 0x00416840, 0x00416860, 0x004169D0, 0x004D9FA0, 0x004E0830, 0x004F0F70, 0x004F0FC0, 0x004F1010,
//   0x004F1210, 0x004F1220, 0x004F1950, 0x004F1A60, 0x004F1A70. Five static append sites execute as eight records because two of them are loop
//   bodies.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. All four globals hold image value 0, so they are zero-initialised `.data` (symbol class `B`); the two id slots
//   are 16-bit and the container and lookup slots are dwords. **The loop trip counts are read from the `mov edi` immediates, not assumed**:
//   0x00439957 is `mov edi,3` for the 0x10-stride loop and 0x00439988 is `mov edi,2` for the 0x20-stride loop, and the eight-entry container
//   allocation is consistent with 3 + 3 + 2. The loop base addresses are `lea esi,[esp+0x138]` (0x00439950, resolving to frame+0x130 once the
//   eight bytes of `push esi`/`push edi` are accounted for) and `lea esi,[esp+0x168]` (0x00439981, resolving to frame+0x160). Both loops are
//   do-while: the append runs before the `dec`/`jne`, so the body executes at least once. The two loops use DIFFERENT registers for the same two
//   roles (`esi` as the running pointer and `edi` as the counter in both), which is why the port keeps one running delta and one count per loop
//   rather than reusing a single induction variable. Deferred pushes: at every getter-fed link site the handler immediate is pushed before the
//   event-id getter call and consumed as the link's third argument. Handler immediates 0x0043A0E0, 0x0043A270, 0x0043A3C0, 0x0043A440,
//   0x0043A450, 0x0043A5E0, 0x0043A660, 0x0043A710, 0x0043A790, 0x00439A50, 0x00439B70, 0x00426860 and 0x00439EC0 are raw code immediates with no
//   implementation in the tree yet. The two hash registrations are NOT the same call: the first masks the id from 0x0057BEA2 through `xor edx,edx;
//   mov dx,[0x57BEA2]` (a zero-extending 16-bit load) and the second uses `mov ecx,[0x57BEA0]; and ecx,0xFFFF` (a dword load then a mask), so the
//   two are transcribed with the same masking result but their retail instruction shapes differ. Documented shape differences, all value-preserving:
//   (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, so those inert pushes are not
//   reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the
//   retail sizes 0x38, 0x18, 0x10, 0xC, 0x44 and 0x1F8 are recorded so a future reviewer can check the arithmetic; (3) the port's loop locals take
//   the place of `esi`/`edi`, whose save/restore in the prologue and epilogue is a calling-convention detail rather than behaviour.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);            // 0x004E0830: `mov ax,[0x548698]; ret` (parent of the first type; image 385)
extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: `mov ax,[0x548648]; ret` (parent of the second type; image 385)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: `mov al,[0x539549]; ret` (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);          // 0x00416860: `mov al,[0x53954B]; ret` (image 255)
extern "C" void* Sys_LookupOrAppendRecord_4169D0(char const* name);   // 0x004169D0 (config/native_4169d0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
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

// 0x0057BEA2: the "AlarmLight" task type id. Image value 0, so it is zero-initialised `.data` (symbol class `B`); all accesses are 16-bit.
std::uint16_t g_57bea2 = 0U;

// 0x0057BEA0: the "AlarmLightReflector" task type id. Image value 0, so it is zero-initialised `.data` (symbol class `B`); all accesses are 16-bit.
std::uint16_t g_57bea0 = 0U;

// 0x0057BE9C: the eight-entry parameter container. Image value 0, so it is zero-initialised `.data` (symbol class `B`).
std::uint32_t g_57be9c = 0U;

// 0x0057BEA4: the pointer Sys_LookupOrAppendRecord_4169D0("alarmlight.spr") returns. Image value 0, so it is zero-initialised `.data` (symbol class `B`).
std::uint32_t g_57bea4 = 0U;

void Task_DefineAlarmLightAndReflector()
{
    // 0x004397C0: sub esp,0x1F8 - a plain frame; this body has NO stack probe. 0x004397C6/0x004397C7: push esi; push edi - the two callee-saved
    // registers the loops below use, saved BELOW the frame, which is why every frame-relative address in this body is offset by eight bytes.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 (the address of the `sub esp,0x1F8` result)
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // --- FIRST TYPE: "AlarmLight" ---

    // 0x004397C8: push 0 (TaskType_Register's fourth argument, pushed first); 0x004397CA: push 0 (the parent getter's inert argument);
    // 0x004397CC: call 0x4E0830; 0x004397D1: push eax; 0x004397D2: push 0x1F8; 0x004397D7: push "AlarmLight"; 0x004397DC: call 0x401900.
    const std::uint16_t alarm_light_id = TaskType_Register("AlarmLight",             // 0x004397D7: push 0x0053C2B4
                                                           0x1F8U,                    // 0x004397D2: push 0x1F8 (object size, equal to the frame)
                                                           Sys_GetGlobal16_4E0830(),  // 0x004397CC: call 0x4E0830 (parent type)
                                                           0U);                      // 0x004397CA: push 0 (fourth argument)
    ::g_57bea2 = alarm_light_id; // 0x004397E9: mov word [0x57BEA2],ax - between the pushes of the first link

    // 0x004397E1: push 0x0043A0E0 (handler, deferred argument 3); 0x004397E6: push 0x0 (argument 2, literal event id);
    // 0x004397E8: push eax (argument 1); 0x004397EF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea2), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A0E0U)));

    // 0x004397F4: mov ax,[0x57BEA2]; 0x004397FA: push 0x0043A270 (handler); 0x004397FF: push 0x1 (event id); 0x00439801: push eax (argument 1);
    // 0x00439802: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea2), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A270U)));

    // 0x00439807: mov cx,[0x57BEA2]; 0x0043980E: push 0x0043A3C0 (handler); 0x00439813: push 0x2 (event id); 0x00439815: push ecx (argument 1);
    // 0x00439816: call 0x401530; 0x0043981B: add esp,0x38 - fourteen pushes.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea2), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A3C0U)));

    // 0x0043981E: push 0x0043A440 (handler, deferred argument 3); 0x00439823: call 0x4F1A60 (event id; the pushed handler is not read);
    // 0x00439828: mov dx,[0x57BEA2]; 0x0043982F: push eax (argument 2); 0x00439830: push edx (argument 1); 0x00439831: call 0x401400;
    // 0x00439836: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea2),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x0043982F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A440U)));

    // 0x00439839: push 0x0043A450 (handler, deferred argument 3); 0x0043983E: call 0x4F1A70; 0x00439843: push eax (argument 2);
    // 0x00439844: mov ax,[0x57BEA2]; 0x0043984A: push eax (argument 1); 0x0043984B: call 0x401400; 0x00439850: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea2),
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00439843: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A450U)));

    // 0x00439853: push 0x0043A5E0 (handler, deferred argument 3); 0x00439858: call 0x416840; 0x0043985D: mov cx,[0x57BEA2];
    // 0x00439864: push eax (argument 2); 0x00439865: push ecx (argument 1); 0x00439866: call 0x401400; 0x0043986B: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea2),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // 0x00439864: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A5E0U)));

    // 0x0043986E: push 0x0043A660 (handler, deferred argument 3); 0x00439873: call 0x416860; 0x00439878: mov dx,[0x57BEA2];
    // 0x0043987F: push eax (argument 2); 0x00439880: push edx (argument 1); 0x00439881: call 0x401400; 0x00439886: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea2),
                                static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // 0x0043987F: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A660U)));

    // 0x00439889: push 0x0043A710 (handler, deferred argument 3); 0x0043988E: call 0x4F1210; 0x00439893: push eax (argument 2);
    // 0x00439894: mov ax,[0x57BEA2]; 0x0043989A: push eax (argument 1); 0x0043989B: call 0x401530; 0x004398A0: add esp,0xC.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea2),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()), // 0x00439893: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A710U)));

    // 0x004398A3: push 0x0043A790 (handler, deferred argument 3); 0x004398A8: call 0x4F1220; 0x004398AD: mov cx,[0x57BEA2];
    // 0x004398B4: push eax (argument 2); 0x004398B5: push ecx (argument 1); 0x004398B6: call 0x401530. This link has no cleanup of its own:
    // the container allocation's add esp,0x18 covers it.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea2),
                                           static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()), // 0x004398B4: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043A790U)));

    // 0x004398BB: xor edx,edx; 0x004398BD: mov dx,[0x57BEA2] (zero-extending 16-bit load); 0x004398C4: push edx (argument 2);
    // 0x004398C5: push 0x0053C2A0 (argument 1); 0x004398CA: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_ALARMLIGHT"), // 0x004398C5: push 0x0053C2A0
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bea2) & 0xFFFFU))); // 0x004398BD: mov dx,[0x57BEA2]

    // 0x004398CF: push 0x8 (the container entry count); 0x004398D1: call 0x4F1950; 0x004398D6: add esp,0x18 - the last link's three pushes
    // (0xC) plus the registration's two plus this count push; 0x004398D9: mov [0x57BE9C],eax.
    ::g_57be9c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(8)); // 0x004398CF: push 0x8

    // 0x004398DE-0x0043994D: three direct appends. Each pushes arg6 = 0 and arg5 = 7, calls its getter, then resolves arg2 to frame+0x00 and arg3
    // to the recorded delta. At these sites esp is eight bytes lower than the frame base because of the saved esi/edi, which is why the retail
    // displacements (0x104, 0x11C, 0x1B8) are twenty larger than the deltas.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be9c)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0); // 0x004398E8: lea eax,[esp+0x104]

    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be9c)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0); // 0x00439914: lea eax,[esp+0x11C]

    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be9c)),
                                append_frame, append_frame + 0x1A4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0); // 0x0043993A: lea eax,[esp+0x1B8]

    // 0x00439950: lea esi,[esp+0x138] -> frame+0x130 (the running third argument); 0x00439957: mov edi,3 (the trip count).
    // 0x0043995C-0x0043997F: do-while - the append runs first, then `add esi,0x10`, `dec edi`, `jne 0x0043995C`.
    std::int32_t loop_delta = 0x130; // retail esi
    std::int32_t loop_count = 3;     // retail edi (0x00439957: mov edi,3)
    do
    {
        // 0x0043995C: push 0 (arg6); 0x0043995E: push 0x7 (arg5); 0x00439960: call 0x4F0FD0 (arg4); 0x00439965: mov ecx,[0x57BE9C] (arg1);
        // 0x0043996B: push eax (arg4); 0x0043996C: lea eax,[esp+0x14] -> frame+0x00 (arg2); 0x00439970: push esi (arg3);
        // 0x00439973: call 0x4F1A80; 0x00439978: add esp,0x18.
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be9c)),
                                    append_frame, append_frame + loop_delta,
                                    static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
        loop_delta += 0x10; // 0x0043997B: add esi,0x10
    } while (--loop_count != 0); // 0x0043997E: dec edi; 0x0043997F: jne 0x0043995C

    // 0x00439981: lea esi,[esp+0x168] -> frame+0x160; 0x00439988: mov edi,2.
    // 0x0043998D-0x004399AF: do-while with a 0x20 stride - `add esi,0x20`, `dec edi`, `jne 0x0043998D`.
    std::int32_t loop2_delta = 0x160; // retail esi
    std::int32_t loop2_count = 2;     // retail edi (0x00439988: mov edi,2)
    do
    {
        // 0x0043998D: push 0 (arg6); 0x0043998F: push 0x7 (arg5); 0x00439991: call 0x4F0FD0 (arg4); 0x00439996: push eax (arg4);
        // 0x00439997: mov eax,[0x57BE9C] (arg1); 0x0043999C: lea edx,[esp+0x14] -> frame+0x00 (arg2); 0x004399A0: push esi (arg3);
        // 0x004399A3: call 0x4F1A80; 0x004399A8: add esp,0x18.
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57be9c)),
                                    append_frame, append_frame + loop2_delta,
                                    static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
        loop2_delta += 0x20; // 0x004399AB: add esi,0x20
    } while (--loop2_count != 0); // 0x004399AE: dec edi; 0x004399AF: jne 0x0043998D

    // --- SECOND TYPE: "AlarmLightReflector" ---

    // 0x004399B1: push 0 (fourth argument); 0x004399B3: push 0 (the parent getter's inert argument); 0x004399B5: call 0x4D9FA0;
    // 0x004399BA: push eax; 0x004399BB: push 0x140; 0x004399C0: push "AlarmLightReflector"; 0x004399C5: call 0x401900.
    const std::uint16_t alarm_light_reflector_id = TaskType_Register("AlarmLightReflector", // 0x004399C0: push 0x0053C28C
                                                                     0x140U,                // 0x004399BB: push 0x140 (object size)
                                                                     Sys_GetGlobal16_4D9FA0(), // 0x004399B5: call 0x4D9FA0 (parent type)
                                                                     0U);                   // 0x004399B3: push 0 (fourth argument)
    ::g_57bea0 = alarm_light_reflector_id; // 0x004399D2: mov word [0x57BEA0],ax - between the pushes of the first link

    // 0x004399CA: push 0x00439A50 (handler); 0x004399CF: push 0x0 (literal event id); 0x004399D1: push eax (argument 1); 0x004399D8: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea0), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439A50U)));

    // 0x004399DD: mov cx,[0x57BEA0]; 0x004399E4: push 0x00439B70 (handler); 0x004399E9: push 0x1 (event id); 0x004399EB: push ecx (argument 1);
    // 0x004399EC: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea0), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439B70U)));

    // 0x004399F1: mov dx,[0x57BEA0]; 0x004399F8: push 0x00426860 (handler); 0x004399FD: push 0x2 (event id); 0x004399FF: push edx (argument 1);
    // 0x00439A00: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bea0), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00426860U)));

    // 0x00439A05: mov ax,[0x57BEA0]; 0x00439A0B: push 0x00439EC0 (handler); 0x00439A10: push 0x4 (event id 4, not 3); 0x00439A12: push eax
    // (argument 1); 0x00439A13: call 0x401400. This link has no cleanup of its own: the add esp,0x44 below covers it.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bea0), 4U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00439EC0U)));

    // 0x00439A18: mov ecx,[0x57BEA0] (a dword load, unlike the first type's zero-extending 16-bit load); 0x00439A1E: add esp,0x44 - the
    // outstanding pushes of this second type; 0x00439A21: and ecx,0xFFFF; 0x00439A27: push ecx (argument 2); 0x00439A28: push 0x0053C26C
    // (argument 1); 0x00439A2D: call 0x4B8810.
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_ALARMLIGHTREFLECTOR"), // 0x00439A28: push 0x0053C26C
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(
                                 static_cast<std::uint32_t>(::g_57bea0) & 0xFFFFU))); // 0x00439A21: and ecx,0xFFFF

    // 0x00439A32: push 0x0053C25C ("alarmlight.spr", argument 1); 0x00439A37: call 0x4169D0; 0x00439A3C: add esp,0xC - this registration's two
    // pushes plus the lookup's one; 0x00439A3F: mov [0x57BEA4],eax.
    ::g_57bea4 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Sys_LookupOrAppendRecord_4169D0("alarmlight.spr"))); // 0x00439A32: push 0x0053C25C

    // 0x00439A44: pop edi; 0x00439A45: pop esi; 0x00439A46: add esp,0x1F8; 0x00439A4C: ret - the frame and the two saved registers are
    // restored here; the port's locals take their place.
} // end of Task_DefineAlarmLightAndReflector

} // extern "C"
