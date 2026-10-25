// Native: Task_DefineDoor (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x004464C0-0x004468EF.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1072 bytes (0x430), 3 blocks,
//   297 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - THREE blocks, and unlike every other target in this lane the
//   body is NOT straight-line: block 1 (0x004464C0-0x004468C2, 279 instructions) falls through into block 2 (0x004468C3-0x004468E6, 14
//   instructions), which ends `jne 0x004468C3` - a back-edge to its own head - and block 3 (0x004468E7-0x004468EF, 4 instructions) is the exit. The
//   objdump listing build/retail-dis.txt was read for the same extent. `ps` at 0x0053C770 (byte-identical to "Door"). `px 4` at all ten globals this
//   body touches (0x0057C1C0, 0x0057C1C8, 0x0057C1CC, 0x0057C1D0, 0x0057C1D4, 0x0057C1D8, 0x0057C1DC, 0x0057C1E0, 0x0057C1E4, 0x0057C1E8) gives
//   ZERO for every one, so every initialiser is 0 and every symbol class is `B`. The prologue was read before any `lea`: `sub esp,0x328` followed by
//   `push %esi` and `push %edi` - there ARE saved-register pushes in this body, but they are pushed AFTER the frame is established, so they live below
//   the frame base and the frame-relative shift for every `lea` stays ZERO; they are popped at 0x004468E7/0x004468E8 before the frame is released,
//   which is exactly why the tail is a bare `add esp,0x328`.
// Behavior/CFG: a 0x328-byte frame that registers the "Door" task type of size 0x328 (equal to its own frame) with parent Sys_GetGlobal16_4C48C0()
//   into the word at 0x0057C1C0, links FIFTEEN handlers (eleven 0x00401400 and four 0x00401530), allocates a SIXTEEN-record container into
//   0x0057C1E8, initialises an eight-dword parameter block, appends THIRTEEN templates directly, and then appends three more in a counted loop.
//   Links: 0x004468F0/e0, 0x00446D10/e1, 0x00446AC0/e2 through the literal event ids 0, 1 and 2; then 0x004F1A60->0x00447A80,
//   0x004F1A70->0x00447830, 0x004167F0->0x00446B50, 0x00416810->0x004EFB00, 0x00416800->0x004D3710 (the shared no-op handler),
//   0x00416820->0x00446CE0, 0x004F1210->0x00447A90, 0x004F1220->0x00447BE0, 0x004C5800->0x00445DE0, 0x00450CA0->0x00446470,
//   0x005096A0->0x004464B0 and 0x00467D90->0x00443DC0.
//   Parameter block (eight 32-bit integer stores, retail order): 0x0057C1C8=0xC1200000 (-10.0f), 0x0057C1CC=0x41200000 (+10.0f),
//   0x0057C1E0=0x00000000 (0.0f), 0x0057C1E4=0x3F800000 (1.0f), 0x0057C1D8=0xC3B40000 (-360.0f), 0x0057C1DC=0x43B40000 (+360.0f),
//   0x0057C1D0=0x3D4CCCCD (0.05f), 0x0057C1D4=0x42C80000 (100.0f). The addresses are NOT traversed in order (the third and fourth stores jump
//   ahead to 0x0057C1E0/0x0057C1E4 before returning to 0x0057C1D8), so the statement order is retail's, not address order. They form a contiguous
//   32-byte block ending exactly where the container pointer at 0x0057C1E8 begins.
//   Appends: THIRTEEN static sites with recorded frame deltas 0xF0, 0x108, 0x10C, 0x138, 0x110, 0x148, 0x140, 0x144, 0x13C, 0x294, 0x240, 0x1A0
//   and 0x1F0 (argument 2 is frame+0x00 at every one), then 0x004468B7 `lea 0x160(%esp),%esi` = frame+0x158, 0x004468BE `mov $0x3,%edi`, and a
//   do-while loop that appends one record per iteration with argument 3 = frame+0x158, +0x168, +0x178 (`add $0x10,%esi` after each append) and
//   argument 4 from 0x004F0FD0. `dec %edi`/`jne` means the advance and the decrement both happen AFTER the append, and the loop body's `add esp,0x18`
//   keeps the stack balanced on every iteration.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The pre-write census and the cleanup reconciliation both close, and the slot assignment was then
//   re-checked per site rather than trusted from the arithmetic** - the census closing only proves every push was accounted for, not that each push
//   went to the right slot, so every link was verified against its own window (push handler / getter result / id reload) and every append against its
//   own two `lea` displacements. Census: 0x00401400 x11, 0x00401530 x4 (15 links), 0x00401900 x1, 0x004167F0 x1, 0x00416800 x1, 0x00416810 x1,
//   0x00416820 x1, 0x00450CA0 x1, 0x00467D90 x1, 0x004C48C0 x1, 0x004C5800 x1, 0x004F0F50 x6, 0x004F0F70 x1, 0x004F0FC0 x3, 0x004F0FD0 x2,
//   0x004F1000 x1, 0x004F1010 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x14, 0x005096A0 x1 =
//   58 sites / 24 distinct. Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x0044651B `add esp,0x38` = the five
//   registration pushes plus links 1-3; ELEVEN `add esp,0xC` at 0x00446536, 0x00446550, 0x0044656B, 0x00446586, 0x004465A0, 0x004465BB,
//   0x004465D6, 0x004465F0, 0x0044660B, 0x00446626 and 0x00446640 = links 4-14, one each; 0x00446662 `add esp,0x10` = link 15's three pushes
//   (which have NO cleanup of their own) plus the alloc's capacity push; FOURTEEN `add esp,0x18` at 0x004466DD, 0x00446706, 0x0044672F,
//   0x00446758, 0x0044677E, 0x004467A4, 0x004467CD, 0x004467F6, 0x0044681C, 0x00446842, 0x00446868, 0x0044688E, 0x004468B4 and 0x004468DE =
//   the thirteen static appends plus the loop's single append site; 0x004468E9 `add esp,0x328` = the frame alone. Totals: 5 + 15*3 + 1 + 14*6 =
//   135 pushes = 540 bytes, removed by 0x38 + 11*0xC + 0x10 + 14*0x18 = 540 bytes; `esp` is 0 at the ret once the two saved-register pops are
//   counted. The simulation was run over all 297 instructions before the body was written.
//   **Three numbers, and this is the first body where the first two DIFFER: FOURTEEN static append sites, SIXTEEN dynamic records, capacity 16
//   requested by `push $0x10` at 0x0044665B.** The thirteen static sites plus THREE loop iterations = sixteen records, and the trip count is
//   evidence (`mov $0x3,%edi`), not assumption, so `records == capacity` is corroborated rather than asserted. A transcription that stopped at the
//   static sites would under-count by three and a transcription that counted the loop site as one record would under-count by two.
//   **New dependencies, both transcribed separately in this same change because neither existed in the tree:** 0x00450CA0 and 0x005096A0 are
//   instruction-for-instruction the same 25-byte lazy cached event-id routine (image-value 0xFF sentinel in a byte slot, `QTask_AllocateEvent(0)`
//   on first call, then the cached byte), and this body uses them as the event getters of links 13 and 14. 0x005096A0 is one of my own second-list
//   targets; **0x00450CA0 is NOT in my 47-target lane and is written only because this body calls it** - it should be dropped or re-owned if another
//   lane has it. Both pass the literal ZERO to QTask_AllocateEvent, unlike 0x00467D90 which passes 0x0181, so the argument's meaning remains
//   unestablished and is transcribed as the literal retail pushes.
//   Cross-references: 0x0044EFB00 and 0x004D3710 are transcribed handlers (script/native_4efb00.cpp, script/native_4d3710.cpp); the remaining THIRTEEN
//   handler immediates (0x004468F0, 0x00446D10, 0x00446AC0, 0x00447A80, 0x00447830, 0x00446B50, 0x00446CE0, 0x00447A90, 0x00447BE0, 0x00445DE0,
//   0x00446470, 0x004464B0, 0x00443DC0) have no transcription in the tree yet, which is the deferred handler backlog rather than a defect in this file -
//   the handler slot is a function pointer and this file passes the retail immediate as a raw value.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore whatever retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the
//   32-bit link arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word;
//   (4) the saved `esi`/`edi` have no port counterpart: retail uses them as the loop template pointer and the loop counter, and the port expresses the
//   same two quantities as locals, so the save/restore pair correctly disappears; (5) the parameter-block stores are `std::uint32_t` holding retail's
//   raw 32-bit patterns, not `float` assignments, because the retail instruction is an integer move.
//   Unestablished: the meaning of every recorded delta; the meaning of the eight parameter-block dwords beyond their IEEE-754 values (the -10/+10 and
//   -360/+360 pairs and the 0/1/0.05/100 singles are named but not interpreted); whether the loop's three templates at frame+0x158/0x168/0x178 are read
//   by any handler; the 0x005096A0/0x00450CA0 allocation argument; and the thirteen untranscribed handler bodies.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530, 0x00401900 and 0x004017C0 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);       // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);       // 0x004F0F50 (sys/native_4f0f50.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);       // 0x004F0F70 (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);       // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);   // 0x004F0FD0 (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);            // 0x004F1000 (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);       // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);       // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);       // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);    // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);    // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);     // 0x004167F0 (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);     // 0x00416800 (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);     // 0x00416810 (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);     // 0x00416820 (config/native_416820.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);  // 0x00467D90 (core/native_00467d90.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);         // 0x004C5800 (dynobj/native_4c5800.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_450CA0(void);  // 0x00450CA0 (core/native_450ca0.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_5096A0(void);  // 0x005096A0 (core/native_5096a0.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C1C0: the "Door" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c1c0 = 0U;

// 0x0057C1C8-0x0057C1E4: the eight-dword "Door" parameter block, all image-zero and therefore all `B`. They are written in retail order by this body
// with the raw 32-bit patterns named in the comments at the stores.
std::uint32_t g_57c1c8 = 0U; // 0x0057C1C8: 0xC1200000 (-10.0f)
std::uint32_t g_57c1cc = 0U; // 0x0057C1CC: 0x41200000 (+10.0f)
std::uint32_t g_57c1d0 = 0U; // 0x0057C1D0: 0x3D4CCCCD (0.05f)
std::uint32_t g_57c1d4 = 0U; // 0x0057C1D4: 0x42C80000 (100.0f)
std::uint32_t g_57c1d8 = 0U; // 0x0057C1D8: 0xC3B40000 (-360.0f)
std::uint32_t g_57c1dc = 0U; // 0x0057C1DC: 0x43B40000 (+360.0f)
std::uint32_t g_57c1e0 = 0U; // 0x0057C1E0: 0x00000000 (0.0f)
std::uint32_t g_57c1e4 = 0U; // 0x0057C1E4: 0x3F800000 (1.0f)

// 0x0057C1E8: the "Door" parameter container, sixteen records (capacity 16, exactly thirteen static appends plus three loop iterations). Image
// value 0 -> `B`.
std::uint32_t g_57c1e8 = 0U;

void Task_DefineDoor()
{
    // 0x004464C0: sub esp,0x328; 0x004464C6: push %esi; 0x004464C7: push %edi. The frame base is set by the plain `sub`, and the two saved-register
    // pushes sit BELOW it, so the frame-relative shift stays ZERO and every `lea` below is measured from the post-`sub` esp. The saved registers are
    // popped again at 0x004468E7/0x004468E8, immediately before the frame is released, which is why the tail is a bare `add esp,0x328`.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004464C8: push 0; 0x004464CA: push 0; 0x004464CC: call 0x4C48C0 (parent type); 0x004464D1: push eax; 0x004464D2: push 0x328;
    // 0x004464D7: push 0x0053C770; 0x004464DC: call 0x401900.
    const std::uint16_t door_id = TaskType_Register("Door",                     // 0x004464D7: push 0x0053C770
                                                    0x328U,                     // 0x004464D2: push 0x328 (object size, equal to the frame)
                                                    Sys_GetGlobal16_4C48C0(),   // 0x004464CC: call 0x4C48C0 (parent type)
                                                    0U);                        // 0x004464CA: push 0
    ::g_57c1c0 = door_id; // 0x004464E9: mov word [0x57C1C0],ax

    // 0x4464E1: push 0x4468F0 (handler, deferred argument 3);
    // 0x4464E6: push $0x0 (argument 2, literal event id);
    // 0x4464E8: push %eax (argument 1, still the id TaskType_Register returned); 0x4464EF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(door_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4468F0U)));

    // 0x4464FA: push 0x446D10 (handler, deferred argument 3);
    // 0x4464FF: push $0x1 (argument 2, literal event id);
    // 0x4464F4: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446502: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1c0), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x446D10U)));

    // 0x44650E: push 0x446AC0 (handler, deferred argument 3);
    // 0x446513: push $0x2 (argument 2, literal event id);
    // 0x446507: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446516: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1c0), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x446AC0U)));

    // 0x44651E: push 0x447A80 (handler, deferred argument 3);
    // 0x446523: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x446528: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446531: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x447A80U)));

    // 0x446539: push 0x447830 (handler, deferred argument 3);
    // 0x44653E: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x446544: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x44654B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x447830U)));

    // 0x446553: push 0x446B50 (handler, deferred argument 3);
    // 0x446558: call Sys_GetSettingByte_4167F0 (supplies argument 2, the event id);
    // 0x44655D: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446566: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x446B50U)));

    // 0x44656E: push 0x4EFB00 (handler, deferred argument 3);
    // 0x446573: call Sys_GetSettingByte_416810 (supplies argument 2, the event id);
    // 0x446578: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446581: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetSettingByte_416810()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4EFB00U)));

    // 0x446589: push 0x4D3710 (handler, deferred argument 3);
    // 0x44658E: call Sys_GetSettingByte_416800 (supplies argument 2, the event id);
    // 0x446594: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x44659B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetSettingByte_416800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x4465A3: push 0x446CE0 (handler, deferred argument 3);
    // 0x4465A8: call Sys_GetSettingByte_416820 (supplies argument 2, the event id);
    // 0x4465AD: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x4465B6: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetSettingByte_416820()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x446CE0U)));

    // 0x4465BE: push 0x447A90 (handler, deferred argument 3);
    // 0x4465C3: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x4465C8: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x4465D1: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x447A90U)));

    // 0x4465D9: push 0x447BE0 (handler, deferred argument 3);
    // 0x4465DE: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x4465E4: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x4465EB: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x447BE0U)));

    // 0x4465F3: push 0x445DE0 (handler, deferred argument 3);
    // 0x4465F8: call DynObj_GetByte_4C5800 (supplies argument 2, the event id);
    // 0x4465FD: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446606: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(DynObj_GetByte_4C5800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445DE0U)));

    // 0x44660E: push 0x446470 (handler, deferred argument 3);
    // 0x446613: call Task_GetCachedEventId_450CA0 (supplies argument 2, the event id);
    // 0x446618: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446621: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Task_GetCachedEventId_450CA0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x446470U)));

    // 0x446629: push 0x4464B0 (handler, deferred argument 3);
    // 0x44662E: call Task_GetCachedEventId_5096A0 (supplies argument 2, the event id);
    // 0x446634: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x44663B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Task_GetCachedEventId_5096A0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4464B0U)));

    // 0x446643: push 0x443DC0 (handler, deferred argument 3);
    // 0x446648: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x44664D: mov [0x57c1c0] into a register (argument 1, the id RELOADED from the global); 0x446656: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1c0), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443DC0U)));

    // 0x0044665B: push $0x10 (container CAPACITY, sixteen records); 0x0044665D: call 0x4F1950; 0x00446662: add esp,0x10 (this push plus the
    // fifteenth link's three, which have no cleanup of their own); 0x00446665: mov [0x57C1E8],eax.
    ::g_57c1e8 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(16)); // 0x0044665B: push 0x10

    // The eight stores below initialise a CONTIGUOUS 32-byte block of dwords at 0x0057C1C8-0x0057C1E4, immediately before the container pointer at
    // 0x0057C1E8. Each is a 32-bit integer move (`movl $imm32,m`), so the port keeps the raw bit pattern and names the IEEE-754 value in the comment
    // rather than writing a float literal. Retail order is preserved exactly.
    ::g_57c1c8 = 0xC1200000U; // 0x0044666A: movl $0xC1200000,0x57C1C8 (-10.0f)
    ::g_57c1cc = 0x41200000U; // 0x00446674: movl $0x41200000,0x57C1CC (+10.0f)
    ::g_57c1e0 = 0x00000000U; // 0x00446682: movl $0x0,0x57C1E0 (0.0f) - note the address order jumps ahead here
    ::g_57c1e4 = 0x3F800000U; // 0x0044668C: movl $0x3F800000,0x57C1E4 (1.0f)
    ::g_57c1d8 = 0xC3B40000U; // 0x00446696: movl $0xC3B40000,0x57C1D8 (-360.0f)
    ::g_57c1dc = 0x43B40000U; // 0x004466A0: movl $0x43B40000,0x57C1DC (+360.0f)
    ::g_57c1d0 = 0x3D4CCCCDU; // 0x004466AA: movl $0x3D4CCCCD,0x57C1D0 (0.05f)
    ::g_57c1d4 = 0x42C80000U; // 0x004466B4: movl $0x42C80000,0x57C1D4 (100.0f)

    // 0x4466BB: push $0x0 (arg6); 0x4466BD: push $0x7 (arg5); 0x4466BE: call Sys_GetGlobal32_4F1010 (arg4);
    // This FIRST append is the widest deferred-push span in the lane: its arg6 (`push $0x0` at 0x0044667E) and arg5 (`push $0x7` at
    // 0x00446680) are pushed BEFORE the whole eight-store parameter block above and stay outstanding across all eight stores and the arg4
    // getter call, until the append finally consumes them at 0x004466D8. The port keeps retail's order — parameter block first, then the call —
    // because the stores must be visible to the container record and the retail pushes carry no side effect of their own.
    // 0x4466CA: lea 0x104(%esp) -> frame+0xF0 (arg3); 0x4466D1: lea 0x14(%esp) -> frame+0x0 (arg2); 0x4466D8: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x4466E4: push $0x0 (arg6); 0x4466E6: push $0x7 (arg5); 0x4466E7: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x4466F3: lea 0x11c(%esp) -> frame+0x108 (arg3); 0x4466FA: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446701: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x44670D: push $0x0 (arg6); 0x44670F: push $0x7 (arg5); 0x446710: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x44671C: lea 0x120(%esp) -> frame+0x10C (arg3); 0x446723: lea 0x14(%esp) -> frame+0x0 (arg2); 0x44672A: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x10C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x446736: push $0x0 (arg6); 0x446738: push $0x7 (arg5); 0x446739: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x44673F: lea 0x14c(%esp) -> frame+0x138 (arg3); 0x44674C: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446753: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x138,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x44675C: push $0x0 (arg6); 0x44675E: push $0x7 (arg5); 0x44675F: call Sys_GetGlobal32_4F0F70 (arg4);
    // 0x44676B: lea 0x124(%esp) -> frame+0x110 (arg3); 0x446772: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446779: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x110,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x446782: push $0x0 (arg6); 0x446784: push $0x7 (arg5); 0x446785: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x446791: lea 0x15c(%esp) -> frame+0x148 (arg3); 0x446798: lea 0x14(%esp) -> frame+0x0 (arg2); 0x44679F: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x148,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x4467AB: push $0x0 (arg6); 0x4467AD: push $0x7 (arg5); 0x4467AE: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x4467BA: lea 0x154(%esp) -> frame+0x140 (arg3); 0x4467C1: lea 0x14(%esp) -> frame+0x0 (arg2); 0x4467C8: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x140,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x4467D4: push $0x0 (arg6); 0x4467D6: push $0x7 (arg5); 0x4467D7: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x4467E3: lea 0x158(%esp) -> frame+0x144 (arg3); 0x4467EA: lea 0x14(%esp) -> frame+0x0 (arg2); 0x4467F1: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x144,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x4467FA: push $0x0 (arg6); 0x4467FC: push $0x7 (arg5); 0x4467FD: call Camera_Get_4F1000 (arg4);
    // 0x446809: lea 0x150(%esp) -> frame+0x13C (arg3); 0x446810: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446817: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x13C,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x446820: push $0x0 (arg6); 0x446822: push $0x7 (arg5); 0x446823: call Sys_GetGlobal32_4F0F50 (arg4);
    // 0x44682F: lea 0x2a8(%esp) -> frame+0x294 (arg3); 0x446836: lea 0x14(%esp) -> frame+0x0 (arg2); 0x44683D: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x294,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);

    // 0x446846: push $0x0 (arg6); 0x446848: push $0x7 (arg5); 0x446849: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x446855: lea 0x254(%esp) -> frame+0x240 (arg3); 0x44685C: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446863: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x240,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x44686C: push $0x0 (arg6); 0x44686E: push $0x7 (arg5); 0x44686F: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x44687B: lea 0x1b4(%esp) -> frame+0x1A0 (arg3); 0x446882: lea 0x14(%esp) -> frame+0x0 (arg2); 0x446889: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x1A0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x446892: push $0x0 (arg6); 0x446894: push $0x7 (arg5); 0x446895: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x4468A1: lea 0x204(%esp) -> frame+0x1F0 (arg3); 0x4468A8: lea 0x14(%esp) -> frame+0x0 (arg2); 0x4468AF: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                append_frame, append_frame + 0x1F0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x004468B7: lea 0x160(%esp),%esi -> frame+0x158 (the esi slot is popped at 0x004468E8, so its own save/restore is the port's local);
    // 0x004468BE: mov $0x3,%edi - THREE iterations, which is the only source of the trip count below.
    std::int32_t* loop_template = reinterpret_cast<std::int32_t*>(
        reinterpret_cast<std::uintptr_t>(&append_frame_base) + 0x158); // 0x004468B7: lea 0x160(%esp),%esi
    std::int32_t loop_counter = 3;                                     // 0x004468BE: mov $0x3,%edi

    // 0x004468C3: push $0x0 (arg6); 0x004468C5: push $0x7 (arg5); 0x004468C7: call 0x4F0FD0 (arg4); 0x004468CC: push eax;
    // 0x004468CD: mov eax,[0x57C1E8] (the container is re-read EVERY iteration); 0x004468D2: lea 0x14(%esp),%edx -> frame+0x00 (arg2);
    // 0x004468D6: push esi (arg3); 0x004468D7: push edx; 0x004468D8: push eax; 0x004468D9: call 0x004F1A80; 0x004468DE: add esp,0x18.
    // 0x004468E1: add $0x10,%esi (advance the template); 0x004468E4: dec %edi; 0x004468E5: jne 0x004468C3 (a do-while, so the advance and the
    // decrement both happen AFTER the append).
    do
    {
        Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1e8)),
                                    append_frame, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(loop_template)),
                                    static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);
        loop_template = reinterpret_cast<std::int32_t*>(reinterpret_cast<std::uintptr_t>(loop_template) + 0x10); // 0x004468E1
        --loop_counter;                                                                                          // 0x004468E4
    } while (loop_counter != 0);                                                                                  // 0x004468E5

    // 0x004468E7: pop %edi; 0x004468E8: pop %esi; 0x004468E9: add esp,0x328 (the frame alone - the two saved pushes are already popped);
    // 0x004468EF: ret.
} // end of Task_DefineDoor

} // extern "C"
