// Native: Task_DefineSwitch (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x00444BF0-0x00444F14.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 805 bytes, 1 block, 232 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 232 instructions, 805 bytes, 0 edges (the body is
//   straight-line with no branch of any kind; established before the body was written). The objdump listing build/retail-dis.txt was read for the same
//   extent. `ps` at 0x0053C6F4 (byte-identical to "Switch"). `px 4` at 0x0057C1B8 and 0x0057C1B4 gives zero, so both initialisers are 0 and both
//   symbol classes are `B`. The two non-obvious callees were read from their own bytes before being trusted as getters: 0x0047E6F0 is
//   `mov al,[0x540904]; ret` and 0x004C5800 is `mov al,[0x547F40]; ret` - both take NO argument, which matters for the fifteenth link (below). The
//   prologue was read before any `lea` was resolved: `sub esp,0x228` with NO saved-register pushes and no stack probe, so the frame-relative shift is
//   ZERO. A push-by-push simulation closes exactly: 111 pushes (444 bytes) against `add esp,0x38` + `add esp,0xC` x11 + `add esp,0x10` +
//   `add esp,0x18` x10 = 444 bytes, then the final `add esp,0x240` removes the 0x228 frame plus the last append's six; `esp` is 0 at the ret. All 23
//   distinct callees were swept against the tree's `// Native:` index and every one resolves to an existing file with the return type used here.
// Behavior/CFG: one straight-line body with a 0x228-byte frame. Registers the "Switch" task type of size 0x228 (equal to its own frame) with parent
//   Sys_GetGlobal16_4E0830() into the word at 0x0057C1B8, then links FIFTEEN handlers (eleven 0x00401400 and four 0x00401530): 0x00444F20/e0,
//   0x004450F0/e1 and 0x00445270/e2 through the literal event ids 0, 1 and 2; then 0x004F1A60->0x004452E0, 0x004F1A70->0x004452F0,
//   0x004167F0->0x00445730, 0x004F01B0<-0x00416810, 0x004D3710 (the shared no-op handler)<-0x00416800, 0x00445820<-0x00416820,
//   0x00445840<-0x00416840, 0x004F1210->0x00445860, 0x004F1220->0x00445980, 0x004C5800->0x00445A80, 0x00467D90->0x00443DC0 and finally a NULL
//   HANDLER fed by 0x0047E6F0 - then Script_TaskContainer_Alloc(0xA) -> 0x0057C1B4 and TEN appends through 0x004F1A80 with recorded frame deltas
//   0xF0, 0x108, 0x1D0, 0x1C0, 0x170, 0x180, 0x190, 0x1A0, 0x1B0 and 0x220, argument 4 taken in order from 0x004F1010, 0x004F0F70, 0x004F0FC0,
//   0x004F1000, 0x004F0FD0 (x5), 0x004F1000 again and the last from 0x004F1000. One block, no branch, 50 call sites over 23 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x11, 0x00401530 x4 (15 links), 0x00401900 x1, 0x004167F0 x1, 0x00416800 x1,
//   0x00416810 x1, 0x00416820 x1, 0x00416840 x1, 0x00467D90 x1, 0x0047E6F0 x1, 0x004C5800 x1, 0x004E0830 x1, 0x004F0F70 x1, 0x004F0FC0 x1,
//   0x004F0FD0 x5, 0x004F1000 x2, 0x004F1010 x1, 0x004F1210 x1, 0x004F1220 x1, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x10 =
//   50 sites / 23 distinct. Cleanup reconciliation: 0x00444C49 `add esp,0x38` = the five registration pushes plus links 1-3; 0x00444C64,
//   0x00444C7E, 0x00444C99, 0x00444CB4, 0x00444CCE, 0x00444CE9, 0x00444D04, 0x00444D1E, 0x00444D39, 0x00444D54 and 0x00444D6E `add esp,0xC`
//   ELEVEN times = links 4-14, one each; 0x00444D8D `add esp,0x10` = link 15's three pushes plus the alloc's capacity push;
//   0x00444DB8/0x00444DDE/0x00444E04/0x00444E2A/0x00444E50/0x00444E76/0x00444E9C/0x00444EC2/0x00444EE8 `add esp,0x18` NINE times plus the final
//   `add esp,0x240` = the ten appends (the last one folded into the tail); 0x00444F14 `ret`.
//   **Three numbers for the one container: TEN static append sites, TEN dynamic records, capacity 0xA requested by `push $0xA` at 0x00444D86.**
//   records == capacity, corroborated because the ten sites are distinct static sites rather than a loop body and the capacity is a literal in a
//   different instruction.
//   **The fifteenth link is the deferred-push pattern in its sharpest form: `push $0x0` at 0x00444D71 is ONE push used twice** - it is the argument
//   the getter at 0x00444D73 receives (and the getter's body proves it is ignored) and it simultaneously stays on the stack as the link's third
//   argument, making the handler slot a NULL HANDLER. So a reader who treats `push imm` as "an argument to the next call" and nothing else will
//   either lose the null handler or duplicate the push.
//   **The ten recorded deltas are NOT monotonic**: 0xF0, 0x108, 0x1D0, **0x1C0, 0x170** (descending), then 0x180, 0x190, 0x1A0, 0x1B0 (ascending)
//   and 0x220. This is the third body in this family whose delta order contradicts "deltas increase", and the second one this run.
//   Cross-references to already-landed files: handler 0x00443DC0 is the same handler native_443680.cpp and native_443ed0.cpp link, and 0x004D3710 is
//   the shared no-op handler (163 link sites image-wide); 0x004F01B0 is used here as a handler immediate and is NOT transcribed in the tree yet.
//   Documented shape differences, all value-preserving: (1) argument-less getters ignore the arguments retail pushes at their call sites and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced except where the push also carries a different role (the fifteenth
//   link, reproduced by passing 0 directly to the link as the null handler); (2) the `add esp` cleanups are reproduced implicitly by the tree's calling
//   convention rather than by explicit arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the 32-bit link
//   arguments in the port while retail leaves the upper bits of eax undefined - no effect on the links, both of which mask the event word.
//   Unestablished: the meaning of the ten recorded deltas (retail's own frame-relative values, transcribed and not decoded); what 0x004F01B0 does (it
//   is a handler immediate with no transcription); the handler immediates 0x00444F20, 0x004450F0, 0x00445270, 0x004452E0, 0x004452F0, 0x00445730,
//   0x004F01B0, 0x00445820, 0x00445840, 0x00445860, 0x00445980, 0x00445A80 and 0x00443DC0 are raw code immediates and this file deliberately does
//   not reference them as symbols.

#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_4E0830(void);       // 0x004E0830: parent type getter (sys/native_4e0830.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);         // 0x0047E6F0: `mov al,[0x540904]; ret` (sys/native_47e6f0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);         // 0x004C5800: `mov al,[0x547F40]; ret` (dynobj/native_4c5800.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);       // 0x004F0F70 (sys/native_4f0f70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);       // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);            // 0x004F1000 (render/native_4f1000.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);   // 0x004F0FD0 (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);       // 0x004F1010 (sys/native_4f1010.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);       // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);       // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);    // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);    // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);     // 0x004167F0 (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);     // 0x00416800 (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);     // 0x00416810 (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);     // 0x00416820 (config/native_416820.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);     // 0x00416840 (config/native_416840.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);  // 0x00467D90 (core/native_00467d90.cpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t const count); // 0x004F1950 (script/native_4f1950.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp): arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

extern "C" {


// 0x0057C1B8: the "Switch" task type id (word). Image value 0 -> initialiser 0, symbol class `B`, writable `.data`: `.rdata` ends at 0x005358F6 and
// `.data` starts at 0x00536000, and this image has no `.bss`, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol
// classification rather than the section. No other file in the tree owns it.
std::uint16_t g_57c1b8 = 0U;

// 0x0057C1B4: the "Switch" parameter container, ten records (capacity 0xA, exactly the number of appends in this body). Image value 0 -> `B`.
std::uint32_t g_57c1b4 = 0U;

void Task_DefineSwitch()
{
    // 0x00444BF0: sub esp,0x228 - a plain frame: no saved-register pushes and no stack probe, so no frame-relative shift applies. The frame is
    // annotated with the "Switch" object size (0x228) and holds the ten append templates.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of every append
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00444BF6: push 0; 0x00444BF8: push 0; 0x00444BFA: call 0x4E0830; 0x00444BFF: push eax; 0x00444C00: push 0x228;
    // 0x00444C05: push 0x0053C6F4; 0x00444C0A: call 0x401900.
    const std::uint16_t switch_id = TaskType_Register("Switch",                  // 0x00444C05: push 0x0053C6F4
                                                      0x228U,                   // 0x00444C00: push 0x228 (object size, equal to the frame)
                                                      Sys_GetGlobal16_4E0830(), // 0x00444BFA: call 0x4E0830 (parent type)
                                                      0U);                      // 0x00444BF8: push 0
    ::g_57c1b8 = switch_id; // 0x00444C17: mov word [0x57C1B8],ax - the only store to this slot

    // 0x444C0F: push 0x444F20 (handler, deferred argument 3);
    // 0x444C14: push $0x0 (argument 2, literal event id);
    // 0x444C16: push %eax (argument 1, still the id TaskType_Register returned); 0x444C1D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(switch_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x444F20U)));

    // 0x444C28: push 0x4450F0 (handler, deferred argument 3);
    // 0x444C2D: push $0x1 (argument 2, literal event id);
    // 0x444C22: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444C30: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1b8), 0x1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4450F0U)));

    // 0x444C3C: push 0x445270 (handler, deferred argument 3);
    // 0x444C41: push $0x2 (argument 2, literal event id);
    // 0x444C35: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444C44: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1b8), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445270U)));

    // 0x444C4C: push 0x4452E0 (handler, deferred argument 3);
    // 0x444C51: call Script_PhaseIdGetE4_4F1A60 (supplies argument 2, the event id);
    // 0x444C56: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444C5F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4452E0U)));

    // 0x444C67: push 0x4452F0 (handler, deferred argument 3);
    // 0x444C6C: call Script_PhaseIdGetE5_4F1A70 (supplies argument 2, the event id);
    // 0x444C72: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444C79: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4452F0U)));

    // 0x444C81: push 0x445730 (handler, deferred argument 3);
    // 0x444C86: call Sys_GetSettingByte_4167F0 (supplies argument 2, the event id);
    // 0x444C8B: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444C94: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445730U)));

    // 0x444C9C: push 0x4F01B0 (handler, deferred argument 3);
    // 0x444CA1: call Sys_GetSettingByte_416810 (supplies argument 2, the event id);
    // 0x444CA6: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444CAF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetSettingByte_416810()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4F01B0U)));

    // 0x444CB7: push 0x4D3710 (handler, deferred argument 3);
    // 0x444CBC: call Sys_GetSettingByte_416800 (supplies argument 2, the event id);
    // 0x444CC2: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444CC9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetSettingByte_416800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x444CD1: push 0x445820 (handler, deferred argument 3);
    // 0x444CD6: call Sys_GetSettingByte_416820 (supplies argument 2, the event id);
    // 0x444CDB: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444CE4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetSettingByte_416820()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445820U)));

    // 0x444CEC: push 0x445840 (handler, deferred argument 3);
    // 0x444CF1: call Sys_GetSettingByte_416840 (supplies argument 2, the event id);
    // 0x444CF6: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444CFF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445840U)));

    // 0x444D07: push 0x445860 (handler, deferred argument 3);
    // 0x444D0C: call Sys_GetGlobal32_4F1210 (supplies argument 2, the event id);
    // 0x444D12: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444D19: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445860U)));

    // 0x444D21: push 0x445980 (handler, deferred argument 3);
    // 0x444D26: call Sys_GetGlobal32_4F1220 (supplies argument 2, the event id);
    // 0x444D2B: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444D34: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445980U)));

    // 0x444D3C: push 0x445A80 (handler, deferred argument 3);
    // 0x444D41: call DynObj_GetByte_4C5800 (supplies argument 2, the event id);
    // 0x444D46: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444D4F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(DynObj_GetByte_4C5800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x445A80U)));

    // 0x444D57: push 0x443DC0 (handler, deferred argument 3);
    // 0x444D5C: call Task_GetCachedEventId_467D90 (supplies argument 2, the event id);
    // 0x444D62: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444D69: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x443DC0U)));

    // 0x444D71: push $0x0 - **ONE push serving two purposes**: it is the argument the getter at 0x444D73 is given (which the getter ignores - its
    // body is `mov al,[0x540904]; ret`) AND it stays on the stack as this link's THIRD argument, so the handler slot is a NULL HANDLER;
    // 0x444D73: call Sys_GetGlobal8_47E6F0 (argument 2, the event id);
    // 0x444D78: mov [0x57c1b8] into a register (argument 1, the id RELOADED from the global); 0x444D81: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c1b8), static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U)));

    // 0x00444D86: push $0xA (container CAPACITY, ten records - the same literal as the number of appends); 0x00444D88: call 0x4F1950;
    // 0x00444D8D: add esp,0x10 (this push plus the fifteenth link's three); 0x00444D90: mov [0x57C1B4],eax.
    ::g_57c1b4 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(10)); // 0x00444D86: push 0xA

    // 0x444D96: push $0x0 (arg6); 0x444D98: push $0x7 (arg5); 0x444D99: call Sys_GetGlobal32_4F1010 (arg4);
    // 0x444DA5: lea 0xfc(%esp) -> frame+0xF0 (arg3); 0x444DAC: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444DB3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x444DBC: push $0x0 (arg6); 0x444DBE: push $0x7 (arg5); 0x444DBF: call Sys_GetGlobal32_4F0F70 (arg4);
    // 0x444DCB: lea 0x114(%esp) -> frame+0x108 (arg3); 0x444DD2: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444DD9: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x444DE2: push $0x0 (arg6); 0x444DE4: push $0x7 (arg5); 0x444DE5: call Sys_GetGlobal32_4F0FC0 (arg4);
    // 0x444DF1: lea 0x1dc(%esp) -> frame+0x1D0 (arg3); 0x444DF8: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444DFF: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x1D0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x444E08: push $0x0 (arg6); 0x444E0A: push $0x7 (arg5); 0x444E0B: call Camera_Get_4F1000 (arg4);
    // 0x444E17: lea 0x1cc(%esp) -> frame+0x1C0 (arg3); 0x444E1E: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444E25: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x1C0,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x444E2E: push $0x0 (arg6); 0x444E30: push $0x7 (arg5); 0x444E31: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x444E3D: lea 0x17c(%esp) -> frame+0x170 (arg3); 0x444E44: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444E4B: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x170,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x444E54: push $0x0 (arg6); 0x444E56: push $0x7 (arg5); 0x444E57: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x444E63: lea 0x18c(%esp) -> frame+0x180 (arg3); 0x444E6A: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444E71: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x444E7A: push $0x0 (arg6); 0x444E7C: push $0x7 (arg5); 0x444E7D: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x444E89: lea 0x19c(%esp) -> frame+0x190 (arg3); 0x444E90: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444E97: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x190,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x444EA0: push $0x0 (arg6); 0x444EA2: push $0x7 (arg5); 0x444EA3: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x444EAF: lea 0x1ac(%esp) -> frame+0x1A0 (arg3); 0x444EB6: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444EBD: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x1A0,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x444EC6: push $0x0 (arg6); 0x444EC8: push $0x7 (arg5); 0x444EC9: call Core_GetRenderWordB_4F0FD0 (arg4);
    // 0x444ED5: lea 0x1bc(%esp) -> frame+0x1B0 (arg3); 0x444EDC: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444EE3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x1B0,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x444EEC: push $0x0 (arg6); 0x444EEE: push $0x7 (arg5); 0x444EEF: call Camera_Get_4F1000 (arg4);
    // 0x444EFB: lea 0x22c(%esp) -> frame+0x220 (arg3); 0x444F02: lea 0xc(%esp) -> frame+0x0 (arg2); 0x444F09: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1b4)),
                                append_frame, append_frame + 0x220,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x00444F0E: add esp,0x240 - the 0x228 frame plus the tenth append's six outstanding pushes; 0x00444F14: ret.
} // end of Task_DefineSwitch

} // extern "C"
