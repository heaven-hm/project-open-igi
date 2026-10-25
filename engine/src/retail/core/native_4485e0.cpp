// Native: Task_DefineBuilding (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x004485E0-0x0044875D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 382 bytes (0x17E), 1 block,
//   106 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 106 instructions, 0 edges (straight-line; the
//   `add esp,0x210` at 0x00448757 followed by the ret is the only exit). The objdump listing build/retail-dis.txt was read for the same extent.
//   `ps` at 0x0053C828 (byte-identical to "Building") and at 0x0053C814 (byte-identical to "TASKTYPE_BUILDING" - a DIFFERENT string from the type
//   name). `px 4` at 0x0057C200 and 0x0057C204 gives zero for both. The prologue is a bare `sub esp,0x1F0` with NO saved-register pushes, so the
//   frame-relative shift is ZERO. A push-by-push simulation over all 106 instructions closes exactly: 5 + 8*3 + 1 + 3*6 + 2 = 50 pushes = 200 bytes
//   against `add esp,0x2C` + five `add esp,0xC` + `add esp,0x10` + two `add esp,0x18` + the tail's (0x210 - 0x1F0) = 200 bytes; `esp` is 0 at the ret.
// Behavior/CFG: a 0x1F0-byte frame. Registers the "Building" task type of size 0x1F0 (equal to its own frame) with parent Sys_GetGlobal16_4DEE80()
//   into the word at 0x0057C200, links EIGHT handlers (seven 0x00401400 and one 0x00401530), allocates a THREE-record container into 0x0057C204,
//   appends three templates, and finally hash-registers the type under "TASKTYPE_BUILDING".
//   Links: 0x00448760 through the literal event id 0 and 0x00448850 through the literal event id **2** - **there is NO link through the literal
//   event id 1 in this body**, so the two literal links are 0 and 2 rather than the 0/1/2 run every earlier body in this family has; then
//   0x004F1A60->0x00448880, 0x004F1A70->0x00448890, 0x00416880->0x00416AB0, 0x00467D90->0x00443DC0, 0x004CEA20->0x00448A90 and
//   0x004CEA10->0x00448950. Note 0x00416AB0 (a handler immediate) and 0x00416880 (its event getter) are BOTH in the config range, which is the only
//   body in this lane where a handler immediate and its event getter sit in the same address band.
//   Appends: THREE static sites through 0x004F1A80 with recorded frame deltas 0x1D8, 0x1A4 and 0x1C8 (argument 2 is frame+0x00 at every one),
//   argument 4 from 0x004F1010, 0x004F0F70 and 0x004F0FD0, argument 5 always the literal 7 and argument 6 always zero.
//   **The three sites do NOT share one instruction order, and the deltas prove it.** At appends 1 and 3 the argument-3 `lea` executes BEFORE the
//   arg-4 `push %eax` (two pushes outstanding, so `lea 0x1E0(%esp)` = frame+0x1D8 and `lea 0x1D0(%esp)` = frame+0x1C8); at append 2 the `push %eax`
//   executes FIRST, so its `lea 0x1AC(%esp)` resolves with THREE pushes outstanding and is frame+0x1A0 - the same-shaped displacement measured at the
//   other two positions would have given 0x1A4. Every delta here is measured at the exact instruction where its own `lea` executes, never inferred
//   from a sibling site.
//   The hash registration explicitly truncates the id word: 0x00448740 `mov ecx,[0x57C200]`, 0x00448746 `and $0xFFFF,%ecx`, 0x0044874C `push ecx`,
//   0x0044874D `push 0x0053C814`, 0x00448752 `call 0x004B8810`.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x7, 0x00401530 x1 (8 links), 0x00401900 x1, 0x00416880 x1, 0x00467D90 x1,
//   0x004B8810 x1, 0x004CEA10 x1, 0x004CEA20 x1, 0x004DEE80 x1, 0x004F0F70 x1, 0x004F0FD0 x1, 0x004F1010 x1, 0x004F1950 x1, 0x004F1A60 x1,
//   0x004F1A70 x1, 0x004F1A80 x3 = 24 sites / 16 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: 0x00448625 `add esp,0x2C` = the five registration pushes plus links 1-2;
//   FIVE `add esp,0xC` at 0x00448640, 0x0044865B, 0x00448675, 0x00448690 and 0x004486AB = links 3-7, one each; 0x004486CC `add esp,0x10` = link 8's
//   three pushes (which have no cleanup of their own) plus the alloc's capacity push; two `add esp,0x18` at 0x004486F6 and 0x0044871B = appends 1-2;
//   the tail `add esp,0x210` at 0x00448757 = the 0x1F0 frame plus EIGHT outstanding pushes, namely the THIRD append's six and the hash
//   registration's two. Totals as above; the arithmetic closes.
//   **Three numbers: THREE static append sites, THREE records expected, capacity 3 requested by `push $0x3` at 0x004486C5.** The container is the one
//   this body allocates itself, so unlike 0x00447DB0 the capacity IS established here and `records == capacity` is corroborated by a literal in a
//   different instruction from the three sites.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced; (2) the `add esp` cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the
//   32-bit link arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word;
//   (4) the `and $0xFFFF` before the hash registration is reproduced explicitly as a mask on the id word, because retail executes it as a real
//   instruction rather than relying on the load's width.
//   Unestablished: the meaning of the literal event id 2 being linked while 1 is not (no evidence in this body explains the gap); the meaning of every
//   recorded delta; and the EIGHT handler immediates 0x00448760, 0x00448850, 0x00448880, 0x00448890, 0x00416AB0, 0x00443DC0, 0x00448A90 and
//   0x00448950, none of which has a transcription anywhere in the tree.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4DEE80(void);    // 0x004DEE80: parent type getter (sys/native_4dee80.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);    // 0x004F0F70 (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void); // 0x004F0FD0 (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);    // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void); // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void); // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416880(void);  // 0x00416880 (config/native_416880.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void); // 0x00467D90 (core/native_00467d90.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548176_4CEA10(void); // 0x004CEA10 (script/native_4cea10.cpp)
extern "C" std::uint8_t QTask_PhaseIdGet548177_4CEA20(void); // 0x004CEA20 (script/native_4cea20.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057C200: the "Building" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c200 = 0U;

// 0x0057C204: the "Building" parameter container, three records (capacity 3, exactly the number of appends in this body). Image value 0 -> `B`.
std::uint32_t g_57c204 = 0U;

void Task_DefineBuilding()
{
    // 0x004485E0: sub esp,0x1F0 - a plain frame: no saved-register pushes and no stack probe, so the frame-relative shift is ZERO.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004485E6: push 0; 0x004485E8: push 0; 0x004485EA: call 0x4DEE80 (parent type); 0x004485EF: push eax; 0x004485F0: push 0x1F0;
    // 0x004485F5: push 0x0053C828; 0x004485FA: call 0x401900.
    const std::uint16_t building_id = TaskType_Register("Building",                // 0x004485F5: push 0x0053C828
                                                        0x1F0U,                    // 0x004485F0: push 0x1F0 (object size, equal to the frame)
                                                        Sys_GetGlobal16_4DEE80(),  // 0x004485EA: call 0x4DEE80 (parent type)
                                                        0U);                       // 0x004485E8: push 0
    ::g_57c200 = building_id; // 0x00448607: mov word [0x57C200],ax

    // 0x4485FF: push 0x448760 (handler, deferred argument 3);
    // 0x448604: push $0x0 (argument 2, literal event id);
    // 0x448606: push %eax (argument 1, still the id TaskType_Register returned); 0x44860D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(building_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448760U)));

    // 0x448618: push 0x448850 (handler, deferred argument 3);
    // 0x44861D: push $0x2 (argument 2, literal event id);
    // 0x448612: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x448620: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c200), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448850U)));

    // 0x448628: push 0x448880 (handler, deferred argument 3);
    // 0x44862D: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x448632: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x44863B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448880U)));

    // 0x448643: push 0x448890 (handler, deferred argument 3);
    // 0x448648: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x44864D: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x448656: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448890U)));

    // 0x44865E: push 0x416AB0 (handler, deferred argument 3);
    // 0x448663: call Sys_GetSettingByte_416880 (supplies argument 2, the event id);
    // 0x448669: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x448670: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(Sys_GetSettingByte_416880()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x416AB0U)));

    // 0x448678: push 0x443DC0 (handler, deferred argument 3);
    // 0x44867D: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x448682: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x44868B: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443DC0U)));

    // 0x448693: push 0x448A90 (handler, deferred argument 3);
    // 0x448698: call QTask_PhaseIdGet548177_4CEA20 (supplies argument 2, the event id);
    // 0x44869D: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x4486A6: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(QTask_PhaseIdGet548177_4CEA20()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448A90U)));

    // 0x4486AE: push 0x448950 (handler, deferred argument 3);
    // 0x4486B3: call QTask_PhaseIdGet548176_4CEA10 (supplies argument 2, the event id);
    // 0x4486B9: mov [0x57c200] into a register (argument 1, the id RELOADED from the global); 0x4486C0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c200), static_cast<std::uint32_t>(QTask_PhaseIdGet548176_4CEA10()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x448950U)));

    // 0x004486C5: push $0x3 (container CAPACITY, three records - the same literal as the number of appends); 0x004486C7: call 0x4F1950;
    // 0x004486CC: add esp,0x10 (this push plus the eighth link's three); 0x004486CF: mov [0x57C204],eax.
    ::g_57c204 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(3)); // 0x004486C5: push 0x3

    // 0x4486D4: push $0x0 (arg6); 0x4486D6: push $0x7 (arg5); 0x4486D8: call Sys_GetGlobal32_4F1010 (arg4);
    // 0x4486DD: lea 0x1e0(%esp) -> frame+0x1D8 (arg3); 0x4486EA: lea 0xc(%esp) -> frame+0x0 (arg2); 0x4486F1: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c204)),
                                append_frame, append_frame + 0x1D8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x4486F9: push $0x0 (arg6); 0x4486FB: push $0x7 (arg5); 0x4486FD: call Sys_GetGlobal32_4F0F70 (arg4);
    // 0x448703: lea 0x1ac(%esp) -> frame+0x1A0 (arg3); 0x44870F: lea 0xc(%esp) -> frame+0x0 (arg2); 0x448716: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c204)),
                                append_frame, append_frame + 0x1A0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x44871E: push $0x0 (arg6); 0x448720: push $0x7 (arg5); 0x448722: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x448727: lea 0x1d0(%esp) -> frame+0x1C8 (arg3); 0x448734: lea 0xc(%esp) -> frame+0x0 (arg2); 0x44873B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c204)),
                                append_frame, append_frame + 0x1C8,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x00448740: mov ecx,[0x57C200]; 0x00448746: and $0xFFFF,%ecx (an explicit 16-bit truncation of the id word, reproduced as a real mask);
    // 0x0044874C: push ecx; 0x0044874D: push 0x0053C814; 0x00448752: call 0x004B8810. The pushed order makes the NAME the first argument and the
    // masked id the second. The "TASKTYPE_BUILDING" literal at 0x0053C814 is a DIFFERENT string from the type name at 0x0053C828.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c200) & 0xFFFFU; // 0x00448746: and $0xffff,%ecx
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_BUILDING"),                       // 0x0044874D: push 0x0053C814
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x0044874C: push ecx

    // 0x00448757: add esp,0x210 - the 0x1F0 frame plus EIGHT outstanding pushes: the third append's six and this hash registration's two;
    // 0x0044875D: ret.
} // end of Task_DefineBuilding

} // extern "C"
