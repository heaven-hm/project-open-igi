// Native: Task_DefineFence (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043F900-0x0043FAD3.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 468 bytes, 1 block, 130 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 130 instructions / 468 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043FAD3 and 0x0043FAE0 is the next
//   function, which this body uses as its first handler). `ps` at 0x0053C510 (the name literal, byte-identical to "Fence") and at 0x0053C500 (the
//   script-hash key, byte-identical to "TASKTYPE_FENCE"); `px 4` at 0x0057C154 (the id slot) and 0x0057C150 (the container slot), both image value 0.
//   The prologue was read before any `lea` was resolved: `sub esp,0x178` followed immediately by argument pushes, with NO saved-register pushes,
//   so the frame-relative shift for this body is ZERO. A full push-by-push simulation closes exactly: the registration's `add esp,0x38` is fourteen
//   pushes (the five registration pushes plus the first three links), the fourth to ninth links' `add esp,0xC` is three each (six of them), the
//   container's `add esp,0x10` is its own count push plus the TENTH link's three pushes (whose cleanup is folded into it), the first three appends'
//   `add esp,0x18` is six each, and **the FOURTH append has no cleanup of its own: its six pushes plus the registration's two are removed by the
//   single `add esp,0x198` at 0x0043FACD** (0x198 = the 0x178 frame plus 0x20). Every callee was swept against the tree's `// Native:` provenance
//   index with no lane exemption and every one resolves to an existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x178-byte frame and no stack probe. It registers the "Fence" task type of size 0x178 with parent
//   Sys_GetGlobal16_4C48C0() -> id at 0x0057C154, stores the id, then links TEN handlers: e0->0x0043FAE0, e1->0x0043FC30 and e2->0x0043FE50 with
//   the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043FE90, 0x004F1A70->0x0043FEA0, 0x004167D0->0x0043FF70, 0x004167F0->0x004400C0,
//   0x00416810->0x004EF8B0, 0x00416800->0x004D3710 and 0x00416820->0x00440230. It then allocates a parameter container through
//   Script_TaskContainer_Alloc(0x4) into 0x0057C150 and appends FOUR records through 0x004F1A80 with recorded deltas 0xF0, 0x108, 0x10C and 0x120
//   (from `lea 0xFC/0x114/0x118/0x12C(%esp)` with three pushes outstanding), and finally registers the type in the script hash:
//   `mov eax,[0x57C154]; and 0xFFFF; push eax; push 0x0053C500; call 0x4B8810`, i.e. Script_QHashNodeRegister("TASKTYPE_FENCE", id) with the id
//   RELOADED from the global and masked to sixteen bits. One basic block, no branch, no loop, 29 call sites over 18 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check holds exactly here: capacity 0x4 = FOUR, static append sites FOUR, and
//   because the body is a single basic block the records written are FOUR too.** Per the correction recorded in native_43b850.cpp the allocation
//   argument is a CAPACITY (`[container+4]`, with `[container]` a live count starting at zero and incremented by each append), so the correct general
//   statement is `records <= capacity`; here the two meet exactly, so the capacity does confirm the record count for this body.
//   Both the id slot 0x0057C154 and the container slot 0x0057C150 are in writable `.data` (the section table puts `.rdata` at VA 0x00533000 with
//   virtual size 0x28F6, ending at 0x005358F6 and `.data` at VA 0x00536000) and both have image value 0, so their initialisers are 0 and their symbol
//   class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification
//   rather than the section. **The id is the WORD at 0x0057C154 and the container pointer is the DWORD at 0x0057C150 - the container slot is the
//   LOWER address here, the reverse of the previous body, so the two must be read from the stores and not from their order.** Arg6 of 0x004F1A80
//   is 0 at all four sites (checked per site), so this body takes the address of none of its own slots. Deferred pushes: at every getter-fed link
//   site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument. **Two of the ten handler
//   immediates are themselves transcribed natives elsewhere in the tree: 0x004EF8B0 is Script_Handler_4EF8B0 (script/native_4ef8b0.cpp) and
//   0x004D3710 is QTask_NoOpHandler (script/native_4d3710.cpp, the image-wide shared no-op with 163 call sites).** They are still passed as raw code
//   addresses here because retail pushes an address, and the tree's declarations of those two routines do not share a single header this file could
//   include; recording the mapping keeps the evidence explicit without inventing a coupling. The remaining immediates 0x0043FAE0, 0x0043FC30,
//   0x0043FE50, 0x0043FE90, 0x0043FEA0, 0x0043FF70, 0x004400C0 and 0x00440230 are raw code immediates with no implementation in the tree yet.
//   Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the
//   caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced
//   implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (six times), 0x10, 0x18
//   (three times) and 0x198 are recorded so a future reviewer can check the arithmetic; (3) the registration's identifier is reloaded from the global
//   and masked exactly as retail does, rather than reusing the local that was just stored.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167D0(void);          // 0x004167D0: argument-less byte getter (config/native_4167d0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);          // 0x004167F0: argument-less byte getter (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);          // 0x00416800: argument-less byte getter (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);          // 0x00416810: argument-less byte getter (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);          // 0x00416820: argument-less byte getter (config/native_416820.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F20(void);            // 0x004F0F20: `mov eax,[0xA70C84]; ret` (sys/native_4f0f20.cpp)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret` (image 0)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0057C154: the "Fence" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`. Only the low 16
// bits are ever stored.
std::uint16_t g_57c154 = 0U;

// 0x0057C150: the parameter container (four slots). Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c150 = 0U;

void Task_DefineFence()
{
    // 0x0043F900: sub esp,0x178 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043F906: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043F908: push 0 (the parent getter's inert argument);
    // 0x0043F90A: call 0x4C48C0; 0x0043F90F: push eax; 0x0043F910: push 0x178; 0x0043F915: push "Fence"; 0x0043F91A: call 0x401900.
    const std::uint16_t fence_id = TaskType_Register("Fence",                  // 0x0043F915: push 0x0053C510
                                                     0x178U,                   // 0x0043F910: push 0x178 (object size, equal to the frame)
                                                     Sys_GetGlobal16_4C48C0(), // 0x0043F90A: call 0x4C48C0 (parent type)
                                                     0U);                      // 0x0043F908: push 0 (fourth argument)
    ::g_57c154 = fence_id; // 0x0043F927: mov word [0x57C154],ax - between the pushes of the first link

    // 0x43F91F: push 0x43FAE0 (handler, deferred argument 3);
    // 0x43F924: push $0 (argument 2, literal event id); 0x43F92D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FAE0U)));

    // 0x43F938: push 0x43FC30 (handler, deferred argument 3);
    // 0x43F93D: push $1 (argument 2, literal event id); 0x43F940: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c154), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FC30U)));

    // 0x43F94C: push 0x43FE50 (handler, deferred argument 3);
    // 0x43F951: push $2 (argument 2, literal event id); 0x43F954: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c154), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FE50U)));

    // 0x43F95C: push 0x43FE90 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x43F96F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FE90U)));

    // 0x43F977: push 0x43FEA0 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x43F989: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FEA0U)));

    // 0x43F991: push 0x43FF70 (handler, deferred argument 3); the Sys_GetSettingByte_4167D0 getter supplies argument 2;
    // 0x43F9A4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Sys_GetSettingByte_4167D0()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43FF70U)));

    // 0x43F9AC: push 0x4400C0 (handler, deferred argument 3); the Sys_GetSettingByte_4167F0 getter supplies argument 2;
    // 0x43F9BF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4400C0U)));

    // 0x43F9C7: push 0x4EF8B0 (handler, deferred argument 3) (transcribed in the tree as script/native_4ef8b0.cpp, Script_Handler_4EF8B0); the Sys_GetSettingByte_416810 getter supplies argument 2;
    // 0x43F9D9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416810()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4EF8B0U)));

    // 0x43F9E1: push 0x4D3710 (handler, deferred argument 3) (the image-wide shared no-op handler, script/native_4d3710.cpp); the Sys_GetSettingByte_416800 getter supplies argument 2;
    // 0x43F9F4: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416800()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x43F9FC: push 0x440230 (handler, deferred argument 3); the Sys_GetSettingByte_416820 getter supplies argument 2;
    // 0x43FA0F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c154),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416820()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440230U)));


    // 0x0043FA14: push 0x4 (the container CAPACITY - four slots, matching the four appends below); 0x0043FA16: call 0x4F1950;
    // 0x0043FA1B: add esp,0x10 - the tenth link's three pushes plus this count push; 0x0043FA1E: mov [0x57C150],eax.
    ::g_57c150 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x4)); // 0x0043FA14: push 0x4

    // 0x43FA23: push $0x0 (arg6); 0x43FA25: push $0x7 (arg5); 0x43FA27: call 0x4F1010 (arg4);
    // 0x43FA2D: lea 0xFC(%esp),%eax resolves to frame+0xF0 (arg3, three pushes outstanding); 0x43FA3A: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x43FA41: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c150)),
                                append_frame, append_frame + 0xF0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43FA49: push $0x0 (arg6); 0x43FA4B: push $0x7 (arg5); 0x43FA4D: call 0x4F0F20 (arg4);
    // 0x43FA59: lea 0x114(%esp),%eax resolves to frame+0x108 (arg3, three pushes outstanding); 0x43FA60: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x43FA67: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c150)),
                                append_frame, append_frame + 0x108,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F20()), 7, 0);

    // 0x43FA6F: push $0x0 (arg6); 0x43FA71: push $0x7 (arg5); 0x43FA73: call 0x4F0FD0 (arg4);
    // 0x43FA7F: lea 0x118(%esp),%eax resolves to frame+0x10C (arg3, three pushes outstanding); 0x43FA86: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x43FA8D: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c150)),
                                append_frame, append_frame + 0x10C,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43FA95: push $0x0 (arg6); 0x43FA97: push $0x7 (arg5); 0x43FA99: call 0x4F0FC0 (arg4);
    // 0x43FAA5: lea 0x12C(%esp),%eax resolves to frame+0x120 (arg3, three pushes outstanding); 0x43FAAC: lea 0xc(%esp),%ecx
    // resolves to frame+0x00 (arg2); 0x43FAB3: call 0x004F1A80.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c150)),
                                append_frame, append_frame + 0x120,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);


    // 0x0043FAB8: mov eax,[0x57C154] (the id is RELOADED from the global); 0x0043FABD: and $0xffff,%eax; 0x0043FAC2: push eax (argument 2);
    // 0x0043FAC3: push 0x0053C500 ("TASKTYPE_FENCE", argument 1); 0x0043FAC8: call 0x4B8810; 0x0043FACD: add esp,0x198 - the fourth append's six
    // pushes plus this call's two, all removed at once; 0x0043FAD3: ret.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c154) & 0xFFFFU; // 0x0043FABD: and $0xffff,%eax
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_FENCE"),                           // 0x0043FAC3: push 0x0053C500
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x0043FAC2: push eax
} // end of Task_DefineFence

} // extern "C"
