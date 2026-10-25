// Native: Task_DefineTerminal (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown);
//   retail VA 0x00443680-0x00443830.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 433 bytes, 1 block, 118 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image - 1 block, 118 instructions, 433 bytes, 0 edges, i.e. the body
//   is straight-line with no branch of any kind (established before the body was written). The objdump listing build/retail-dis.txt was read for the
//   same extent. `ps` at 0x0053C684 (byte-identical to "Terminal"). `px 4` at 0x0053C680 gives 0x00000181 - this body's id slot carries the
//   unregistered sentinel as its IMAGE value and is overwritten by `mov word [0x53C680],ax` at 0x004436A7 - and at 0x0057C1A0 gives 0, the
//   container slot this body writes. The two getters this body calls were read from their own bytes: 0x00481340 is `mov ax,[0x54097C]; ret` (image
//   value at 0x0054097C is also 0x0181) and 0x00481350 is `mov eax,[0x5C1184]; ret` (image value 0). The prologue was read before any `lea` was
//   resolved: `sub esp,0x2A0` with NO saved-register pushes and no stack probe, so the frame-relative shift is ZERO. A push-by-push simulation closes
//   exactly - 55 pushes (220 bytes) against 14+21+3+3+2+6 pushes removed by `add esp,0x38`/`0xC`x9/`0x8`/`0x18`, and the final `add esp,0x2B8`
//   removes the 0x2A0 frame plus the last append's six pushes; `esp` is 0 at the ret. All 18 distinct callees were swept against the tree's
//   `// Native:` index and every one resolves to an existing file with the return type used here.
// Behavior/CFG: one straight-line body with a 0x2A0-byte frame. Registers the "Terminal" task type of size 0x2A0 (equal to its own frame) with parent
//   Sys_GetGlobal16_481340() into the word at 0x0053C680, then links TWELVE handlers: 0x00443840/e0, 0x004438E0/e1 and 0x00443990/e2 through the
//   literal event ids 0, 1 and 2; then 0x004F1A60->0x00443A00, 0x004F1A70->0x00443A10, 0x004F1210->0x00443A60, 0x004F1220->0x00443B80,
//   0x004167F0->0x00443C80, 0x00416810->0x00443D70, 0x00416800->0x00443D80, 0x00416820->0x00443DA0 and 0x00467D90->0x00443DC0. It then grows the
//   parameter container - `push $0x2` is left outstanding across the `call 0x481350` so that it becomes Script_TaskContainer_Grow's SECOND argument
//   (the count) while the getter's DWORD becomes its FIRST (the container) - i.e.
//   `0x0057C1A0 = Script_TaskContainer_Grow((int32_t*)Sys_GetGlobal32_481350(), 2)`, and appends TWO records through 0x004F1A80 with recorded frame
//   deltas 0x250 (argument 4 from 0x004F0FC0) and 0x24C (argument 4 from 0x004F0F50), in that order. One basic block, no branch, no loop, 29 call
//   sites over 18 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. Census: 0x00401400 x8, 0x00401530 x4 (12 links), 0x00401900 x1, 0x004167F0 x1, 0x00416800 x1,
//   0x00416810 x1, 0x00416820 x1, 0x00467D90 x1, 0x00481340 x1, 0x00481350 x1, 0x004F0F50 x1, 0x004F0FC0 x1, 0x004F1210 x1, 0x004F1220 x1,
//   0x004F1980 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x2 = 29 sites / 18 distinct. Cleanup reconciliation: 0x004436D9 `add esp,0x38` = the five
//   registration pushes plus links 1-3; 0x004436F4, 0x0044370E, 0x00443729, 0x00443744, 0x0044375E, 0x00443779, 0x00443794, 0x004437AE and
//   0x004437C9 `add esp,0xC` NINE times = links 4-12, one each with no folding; 0x004437D9 `add esp,0x8` = the grow's two argument pushes;
//   0x00443804 `add esp,0x18` = the first append's six; 0x0044382A `add esp,0x2B8` = 0x2A0 + the second append's six.
//   **Three numbers: TWO static append sites, TWO dynamic records, capacity 2 requested by the `push $0x2` at 0x004437CC.** records == capacity, and
//   the equality is corroborated because the two sites are distinct static sites rather than a loop body and the capacity is a literal in a different
//   instruction; `records <= capacity` is the claim, equality an observation.
//   **Cross-body finding that closes an earlier open question: this body is the WRITER of 0x0057C1A0.** native_440710.cpp and native_440c20.cpp read
//   that slot through Sys_GetGlobal32_443E90 and grow it in place, and their provenance blocks record that they do not create it - this body does:
//   `0x0057C1A0 = Grow([0x5C1184], 2)`. The chain is therefore 0x005C1184 (allocated with thirteen records by 0x004813E0, core/native_4813e0.cpp)
//   -> grown here into 0x0057C1A0 -> grown again by the AlarmControl and SCameraControl bodies into their own slots.
//   **Ownership note for integration (a new file cannot fix this without a duplicate definition):** by the writer-owns rule the definitions of
//   0x0053C680 and 0x0057C1A0 belong to this file, but they currently live in the two getter files as uninitialised definitions -
//   `std::uint16_t g_53c680;` in sys/native_443e80.cpp and `std::uint32_t g_57c1a0;` in sys/native_443e90.cpp, both commented "nothing else in the
//   tree owns it yet", which this body makes false. This file therefore declares both `extern` and assigns them. Two things must be corrected when
//   those files are next touched: (1) `g_53c680` must be initialised to `0x181U`, because its image value at 0x0053C680 IS 0x0181 and the current
//   zero initialiser is not byte-faithful; (2) the definitions should move here and those files should declare them `extern`, though the emitted
//   code is identical either way because 0x004436A7 overwrites the id before any reader of this body's own output runs.
//   **This body is a third independent confirmation of the 0x0181 sentinel**: its id slot 0x0053C680 has image value 0x0181 and is overwritten by a
//   `mov word [slot],ax` right after TaskType_Register, and the parent getter it calls returns the value at 0x0054097C, which is also 0x0181 in the
//   image - i.e. the parent-type argument is the sentinel when no real parent has been registered yet, and 0x004813E0 later stores a real id there.
//   Documented shape differences, all value-preserving: (1) the getter callees ignore the arguments retail pushes at their call sites, and the tree's
//   transcriptions take no arguments, so those inert pushes are not reproduced - the getter's inert `$0x2` here doubles as Grow's count, expressed by
//   passing the literal two directly; (2) the `add esp` cleanups are reproduced implicitly by the tree's calling convention rather than by explicit
//   arithmetic, with the retail sizes recorded above; (3) byte-valued getters are zero-extended into the 32-bit link arguments in the port, whereas
//   retail leaves the upper bits of eax undefined - no effect on the links, because both link callees mask the event word.
//   Unestablished: the meaning of the two recorded deltas (they are retail's own frame-relative values, transcribed and not decoded); the meaning of
//   the parent type whose getter reads 0x0054097C; whether the grown container's first two slots mean what their later readers assume; the handler
//   immediates 0x00443840, 0x004438E0, 0x00443990, 0x00443A00, 0x00443A10, 0x00443A60, 0x00443B80, 0x00443C80, 0x00443D70, 0x00443D80, 0x00443DA0
//   and 0x00443DC0 are raw code immediates with no transcription in the tree yet, and this file deliberately does not reference them as symbols.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

// Callees, each with the return type and linkage of its own transcribed file. 0x00401400, 0x00401530 and 0x00401900 come from
// igi/retail/core/task_types.hpp (core/task_types.cpp) and are not redeclared here.
extern "C" std::uint16_t Sys_GetGlobal16_481340(void);            // 0x00481340: `mov ax,[0x54097C]; ret` (sys/native_481340.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);            // 0x00481350: `mov eax,[0x5C1184]; ret` (sys/native_481350.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);          // 0x004167F0 (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);          // 0x00416800 (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);          // 0x00416810 (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);          // 0x00416820 (config/native_416820.cpp)
extern "C" std::uint8_t Task_GetCachedEventId_467D90(void);       // 0x00467D90 (core/native_00467d90.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60 (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70 (script/native_4f1a70.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210 (sys/native_4f1210.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220 (sys/native_4f1220.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0 (sys/native_4f0fc0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50 (sys/native_4f0f50.cpp)
extern "C" void* Script_TaskContainer_Grow(std::int32_t* const container, std::int32_t const count); // 0x004F1980 (script/native_4f1980.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C680: the "Terminal" task type id. **Written by this body (0x004436A7) but DEFINED in sys/native_443e80.cpp**, whose getter reads it; that
// file's definition is currently uninitialised, while the retail image value here is 0x0181 (the unregistered sentinel) - recorded as a defect to fix
// in that file rather than papered over with a second definition here. Writable `.data`: `.rdata` ends at 0x005358F6 and `.data` starts at
// 0x00536000, and this image has no `.bss`, so `B`/`D` is the symbol classification rather than the section.
extern std::uint16_t g_53c680; // 0x0053C680

// 0x0057C1A0: the grown parameter container. **Written by this body (0x004437DC) but DEFINED in sys/native_443e90.cpp**, whose getter reads it; the
// image value is 0, so the initialiser there is 0 and the symbol class is `B`. The two bodies that later grow this container in place
// (native_440710.cpp and native_440c20.cpp) read it through that same getter and do not define it.
extern std::uint32_t g_57c1a0; // 0x0057C1A0

void Task_DefineTerminal()
{
    // 0x00443680: sub esp,0x2A0 - a plain frame: no saved-register pushes and no stack probe, so no frame-relative shift applies. The frame holds the
    // two argument templates this body hands to Script_TaskContainer_Append.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of both appends
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00443686: push 0 (TaskType_Register's fourth argument, pushed first); 0x00443688: push 0 (the parent getter's inert argument);
    // 0x0044368A: call 0x481340; 0x0044368F: push eax; 0x00443690: push 0x2A0; 0x00443695: push 0x0053C684; 0x0044369A: call 0x401900.
    const std::uint16_t terminal_id = TaskType_Register("Terminal",                // 0x00443695: push 0x0053C684
                                                        0x2A0U,                    // 0x00443690: push 0x2A0 (object size, equal to the frame)
                                                        Sys_GetGlobal16_481340(),  // 0x0044368A: call 0x481340 (parent type; `mov ax,[0x54097C]`)
                                                        0U);                       // 0x00443688: push 0
    ::g_53c680 = terminal_id; // 0x004436A7: mov word [0x53C680],ax - stored between the pushes of the first link, and reused as argument 1 below.

    // 0x0044369F: push 0x00443840 (handler, deferred argument 3); 0x004436A4: push $0x0 (argument 2, literal event id);
    // 0x004436A6: push eax (argument 1, still the id TaskType_Register returned); 0x004436AD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(terminal_id), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443840U)));

    // 0x004436B2: mov ax,[0x53C680] (the id RELOADED from the global); 0x004436B8: push 0x004438E0 (handler); 0x004436BD: push $0x1 (event id);
    // 0x004436BF: push eax; 0x004436C0: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c680), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004438E0U)));

    // 0x004436C5: mov cx,[0x53C680] (a second reload, and NO cleanup between this link and the previous one);
    // 0x004436CC: push 0x00443990 (handler); 0x004436D1: push $0x2 (event id); 0x004436D3: push ecx; 0x004436D4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c680), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443990U)));

    // 0x004436D9: add esp,0x38 - fourteen pushes: the five registration pushes plus the three links above.
    // 0x004436DC: push 0x00443A00 (handler, deferred argument 3); 0x004436E1: call 0x4F1A60 (argument 2, the event id);
    // 0x004436E6: mov dx,[0x53C680]; 0x004436ED: push eax; 0x004436EE: push edx; 0x004436EF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443A00U)));

    // 0x004436F4: add esp,0xC; 0x004436F7: push 0x00443A10 (handler); 0x004436FC: call 0x4F1A70; 0x00443701: push eax;
    // 0x00443702: mov ax,[0x53C680]; 0x00443708: push eax; 0x00443709: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443A10U)));

    // 0x0044370E: add esp,0xC; 0x00443711: push 0x00443A60 (handler); 0x00443716: call 0x4F1210; 0x0044371B: mov cx,[0x53C680];
    // 0x00443722: push eax; 0x00443723: push ecx; 0x00443724: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443A60U)));

    // 0x00443729: add esp,0xC; 0x0044372C: push 0x00443B80 (handler); 0x00443731: call 0x4F1220; 0x00443736: mov dx,[0x53C680];
    // 0x0044373D: push eax; 0x0044373E: push edx; 0x0044373F: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443B80U)));

    // 0x00443744: add esp,0xC; 0x00443747: push 0x00443C80 (handler); 0x0044374C: call 0x4167F0; 0x00443751: push eax;
    // 0x00443752: mov ax,[0x53C680]; 0x00443758: push eax; 0x00443759: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443C80U)));

    // 0x0044375E: add esp,0xC; 0x00443761: push 0x00443D70 (handler); 0x00443766: call 0x416810; 0x0044376B: mov cx,[0x53C680];
    // 0x00443772: push eax; 0x00443773: push ecx; 0x00443774: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetSettingByte_416810()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443D70U)));

    // 0x00443779: add esp,0xC; 0x0044377C: push 0x00443D80 (handler); 0x00443781: call 0x416800; 0x00443786: mov dx,[0x53C680];
    // 0x0044378D: push eax; 0x0044378E: push edx; 0x0044378F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetSettingByte_416800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443D80U)));

    // 0x00443794: add esp,0xC; 0x00443797: push 0x00443DA0 (handler); 0x0044379C: call 0x416820; 0x004437A1: push eax;
    // 0x004437A2: mov ax,[0x53C680]; 0x004437A8: push eax; 0x004437A9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Sys_GetSettingByte_416820()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443DA0U)));

    // 0x004437AE: add esp,0xC; 0x004437B1: push 0x00443DC0 (handler); 0x004437B6: call 0x467D90; 0x004437BB: mov cx,[0x53C680];
    // 0x004437C2: push eax; 0x004437C3: push ecx; 0x004437C4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c680), static_cast<std::uint32_t>(Task_GetCachedEventId_467D90()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00443DC0U)));

    // 0x004437C9: add esp,0xC - the twelfth link's three pushes, cleaned before the grow.
    // 0x004437CC: push $0x2 (Script_TaskContainer_Grow's SECOND argument, the count - pushed BEFORE the getter call and deliberately left
    // outstanding across it); 0x004437CE: call 0x481350 (argument 1, the container, arrives in eax from `mov eax,[0x5C1184]`);
    // 0x004437D3: push eax (Grow's FIRST argument); 0x004437D4: call 0x4F1980; 0x004437D9: add esp,0x8; 0x004437DC: mov [0x57C1A0],eax.
    ::g_57c1a0 = reinterpret_cast<std::uint32_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), 2)); // 0x004437CC: push 0x2

    // 0x004437E1: push $0x0 (arg6); 0x004437E3: push $0x7 (arg5); 0x004437E5: call 0x4F0FC0 (arg4); 0x004437EA: mov ecx,[0x57C1A0];
    // 0x004437F0: push eax; 0x004437F1: lea 0x25C(%esp),%edx resolves to frame+0x250 (arg3); 0x004437F8: lea 0xC(%esp),%eax resolves to
    // frame+0x00 (arg2); 0x004437FC-0x004437FE: push edx, push eax, push ecx; 0x004437FF: call 0x004F1A80; 0x00443804: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a0)),
                                append_frame, append_frame + 0x250,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x00443807: push $0x0 (arg6); 0x00443809: push $0x7 (arg5); 0x0044380B: call 0x4F0F50 (arg4); 0x00443810: mov ecx,[0x57C1A0];
    // 0x00443816: push eax; 0x00443817: lea 0x258(%esp),%edx resolves to frame+0x24C (arg3 - a LOWER delta than the append above, so the deltas
    // are not monotonic); 0x0044381E: lea 0xC(%esp),%eax resolves to frame+0x00 (arg2); 0x00443825: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c1a0)),
                                append_frame, append_frame + 0x24C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
    // 0x0044382A: add esp,0x2B8 - the 0x2A0 frame plus the second append's six outstanding pushes; 0x00443830: ret.
} // end of Task_DefineTerminal

} // extern "C"
