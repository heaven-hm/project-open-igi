// Native: Task_DefineElevatorFloorIndicator (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043C6D0-0x0043C8C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 497 bytes, 1 block, 147 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 147 instructions / 497 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043C8C0 and 0x0043C8D0 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C364 for the name literal (byte-identical to "ElevatorFloorIndicator");
//   `px 4` at 0x0057BF84 and 0x0057BF88 (both image value 0). The prologue was read before any `lea` was resolved: `sub esp,0x248` followed
//   immediately by argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO. A full push-by-push
//   simulation closes exactly: the registration's `add esp,0x38` is fourteen pushes, the fourth, fifth, sixth and seventh links' `add esp,0xC` is
//   three each, and the container's `add esp,0x10` is its own count push plus the three pushes of the eighth link, whose cleanup is folded into it.
//   **The EIGHTH link is a NULL-HANDLER link and the mechanical link detector did not find it**: at 0x0043C797 retail pushes the handler immediate
//   0x0, not a code address, so a detector keyed on `push $imm >= 0x400000` sees only seven links where retail has eight. It was found by
//   simulating the pushes and by the call census (0x00401400 appears SIX times, not five), and it is transcribed by hand here. Every callee was
//   swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file under the same name,
//   return type and linkage.
// Behavior/CFG: one straight-line body with a 0x248-byte frame and no stack probe. It registers the "ElevatorFloorIndicator" task type of size 0x248
//   with parent Sys_GetGlobal16_4C48C0() -> id at 0x0057BF84, stores the id, then links EIGHT handlers: e0->0x0043C8D0, e1->0x0043CA10 and
//   e2->0x0043CB30 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043CB60, 0x004F1A70->0x0043CB70, 0x00416840->0x004D3710 (the
//   image's shared no-op handler), 0x004C5800->0x0043C390 and finally 0x0047E6F0->**0x00000000** (a null handler). It then allocates a parameter
//   container through Script_TaskContainer_Alloc(0x7) into 0x0057BF88 and appends SEVEN records through 0x004F1A80 with recorded deltas 0xF0, 0x108,
//   0x170, 0x180, 0x190, 0x1A4 and 0x1F4. One basic block, no branch, no loop, 30 call sites over 15 distinct callees. There is NO 0x004B8810
//   script-name registration and NO 0x004169D0 lookup in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check holds exactly here: capacity 0x7 = SEVEN, static append sites SEVEN, and
//   because the body is a single basic block the records written are SEVEN too.** Given the correction recorded in native_43b850.cpp (the argument
//   of Script_TaskContainer_Alloc is a CAPACITY - `[container+4]` - while `[container]` is a live count starting at zero and incremented by each
//   append), the correct reading is `records <= capacity`, and here the two meet exactly, so the capacity does confirm the record count for this
//   body. The id slot 0x0057BF84 and the container slot 0x0057BF88 are both in writable `.data` (the section table puts `.rdata` at VA 0x00533000
//   with virtual size 0x28F6, ending at 0x005358F6) and both have image value 0, so their initialisers are 0 and their symbol class is `B`; this
//   image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the
//   section. **The two globals are adjacent and must not be conflated: the id is the WORD at 0x0057BF84 (only `mov word [0x57BF84],ax` ever
//   writes it) and the container pointer is the DWORD at 0x0057BF88.** Arg6 of 0x004F1A80 is 0 at all seven sites here (checked per site), so
//   this body takes the address of none of its own slots. Deferred pushes: at every getter-fed link site the handler immediate is pushed before the
//   event-id getter call and consumed as the link's third argument - including the null-handler site, where the pushed immediate is zero and the
//   getter still ignores it. Handler immediates 0x0043C8D0, 0x0043CA10, 0x0043CB30, 0x0043CB60, 0x0043CB70, 0x004D3710 and 0x0043C390 are raw code
//   immediates; 0x004D3710 is the image-wide shared no-op handler (163 call sites) and is not transcribed in this lane. Documented shape
//   differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and
//   the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (four times), 0x10 and 0x18 (seven times) are
//   recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port, which cannot change behaviour
//   because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetGlobal8_47E6F0(void);              // 0x0047E6F0: argument-less byte getter (sys/native_47e6f0.cpp)
extern "C" std::uint8_t DynObj_GetByte_4C5800(void);              // 0x004C5800: argument-less byte getter (dynobj/native_4c5800.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057BF84: the "ElevatorFloorIndicator" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`.
// Only the low 16 bits are ever stored. The container pointer lives at the next dword, 0x0057BF88.
std::uint16_t g_57bf84 = 0U;

// 0x0057BF88: the parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57bf88 = 0U;

void Task_DefineElevatorFloorIndicator()
{
    // 0x0043C6D0: sub esp,0x248 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043C6D6: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043C6D8: push 0 (the parent getter's inert argument);
    // 0x0043C6DA: call 0x4C48C0; 0x0043C6DF: push eax; 0x0043C6E0: push 0x248; 0x0043C6E5: push "ElevatorFloorIndicator"; 0x0043C6EA: call 0x401900.
    const std::uint16_t elevator_floor_indicator_id = TaskType_Register("ElevatorFloorIndicator", // 0x0043C6E5: push 0x0053C364
                                                                       0x248U,                  // 0x0043C6E0: push 0x248 (object size, equal to the frame)
                                                                       Sys_GetGlobal16_4C48C0(), // 0x0043C6DA: call 0x4C48C0 (parent type)
                                                                       0U);                     // 0x0043C6D8: push 0 (fourth argument)
    ::g_57bf84 = elevator_floor_indicator_id; // 0x0043C6F7: mov word [0x57BF84],ax - between the pushes of the first link

    // 0x43C6EF: push 0x0043C8D0 (handler, deferred argument 3); 0x43C6F4: push 0x0 (argument 2); 0x43C6FD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C8D0U)));

    // 0x43C708: push 0x0043CA10 (handler, deferred argument 3); 0x43C70D: push 0x1 (argument 2); 0x43C710: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bf84), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043CA10U)));

    // 0x43C71C: push 0x0043CB30 (handler, deferred argument 3); 0x43C721: push 0x2 (argument 2); 0x43C724: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57bf84), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043CB30U)));

    // 0x43C72C: push 0x0043CB60 (handler, deferred argument 3); 0x43C731: call 0x4F1A60 -> argument 2; 0x43C73F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043CB60U)));

    // 0x43C747: push 0x0043CB70 (handler, deferred argument 3); 0x43C74C: call 0x4F1A70 -> argument 2; 0x43C759: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043CB70U)));

    // 0x43C761: push 0x004D3710 (handler, deferred argument 3); 0x43C766: call 0x416840 -> argument 2; 0x43C774: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84), static_cast<std::uint32_t>(Sys_GetSettingByte_416840()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x004D3710U)));

    // 0x43C77C: push 0x0043C390 (handler, deferred argument 3); 0x43C781: call 0x4C5800 -> argument 2; 0x43C78F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84), static_cast<std::uint32_t>(DynObj_GetByte_4C5800()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043C390U)));
    // 0x43C797: push 0x0 (the handler immediate is ZERO - this is a NULL-HANDLER link, and the generator's link detection misses it because it
    // requires a handler immediate >= 0x400000); 0x43C799: call 0x47E6F0 (event id; the pushed zero is not read); 0x43C79E: push eax (argument 2);
    // 0x43C79F: mov ax,[0x57BF84]; 0x43C7A5: push eax (argument 1); 0x43C7A6: call 0x401400; 0x43C7AB: push 0x7 (the container capacity, next).
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57bf84),
                                static_cast<std::uint32_t>(Sys_GetGlobal8_47E6F0()), // 0x43C79E: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0U))); // 0x43C797: push 0x0


    // 0x0043C7AB: push 0x7 (the container CAPACITY - seven slots, matching the seven appends below); 0x0043C7AD: call 0x4F1950;
    // 0x0043C7B2: add esp,0x10 - the eighth link's three pushes plus this count push; 0x0043C7B5: mov [0x57BF88],eax.
    ::g_57bf88 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x7)); // 0x0043C7AB: push 0x7

    // 0x43C7BA: push $0x0 (arg6); 0x43C7BE: call Sys_GetGlobal32_4F1010 (arg4); 0x43C7D7: call 0x004F1A80 (frame+0x00, frame+0xF0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43C7DF: push $0x0 (arg6); 0x43C7E3: call Sys_GetGlobal32_4F0F70 (arg4); 0x43C7FC: call 0x004F1A80 (frame+0x00, frame+0x108).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x43C804: push $0x0 (arg6); 0x43C808: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43C821: call 0x004F1A80 (frame+0x00, frame+0x170).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x170,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43C829: push $0x0 (arg6); 0x43C82D: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43C846: call 0x004F1A80 (frame+0x00, frame+0x180).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43C84E: push $0x0 (arg6); 0x43C852: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43C86B: call 0x004F1A80 (frame+0x00, frame+0x190).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x190,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43C873: push $0x0 (arg6); 0x43C877: call Sys_GetGlobal32_4F0FC0 (arg4); 0x43C890: call 0x004F1A80 (frame+0x00, frame+0x1A4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x1A4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43C898: push $0x0 (arg6); 0x43C89C: call Sys_GetGlobal32_4F0FC0 (arg4); 0x43C8B5: call 0x004F1A80 (frame+0x00, frame+0x1F4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf88)),
                                append_frame, append_frame + 0x1F4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);
} // end of Task_DefineElevatorFloorIndicator

} // extern "C"
