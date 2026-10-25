// Native: Task_DefineGenerator (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043B2F0-0x0043B48D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 414 bytes, 1 block, 119 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 119 instructions / 414 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043B48D and 0x0043B490 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C300 for the name literal (byte-identical to "Generator"); `px 4` at
//   0x0053C2FC (image value 0x00000181) and 0x0057BF3C (image value 0). **The prologue was read before any `lea` was resolved: it is
//   `sub esp,0x2C0` followed immediately by argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO** -
//   the opposite of 0x004397C0 (two saved registers, shift 8) and 0x0043AD80 (three saved registers, shift 12). Each prologue has to be read;
//   neither the presence nor the absence of a shift can be assumed. A full push-by-push simulation closes exactly: the registration's
//   `add esp,0x38` is fourteen pushes, the container's `add esp,0x8` is its own two, each link's `add esp,0xC` is its three, and the final
//   `add esp,0x2D8` is the 0x2C0 frame plus the last append's six pushes. Every callee was swept against the tree's `// Native:` provenance index
//   with no lane exemption and every one resolves to an existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x2C0-byte frame and no stack probe. It registers the "Generator" task type of size 0x2C0 with parent
//   Sys_GetGlobal16_481340() -> id at 0x0053C2FC, stores the id, then links SEVEN handlers: e0->0x0043B490, e1->0x0043B500 and e2->0x0043B650
//   with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043B680, 0x004F1A70->0x0043B690, 0x004F1210->0x0043B6E0 and
//   0x004F1220->0x0043B790. It grows an existing parameter container by FIVE entries - `Script_TaskContainer_Grow(Sys_GetGlobal32_481350(), 5)`,
//   the container coming from the getter and the count being the pushed 5 - stores the returned pointer at 0x0057BF3C and appends five records
//   through 0x004F1A80 with recorded deltas 0x290, 0x2A0, 0x240, 0x2B4 and 0x2B0. One basic block, no branch, no loop, 25 call sites over
//   15 distinct callees. There is NO 0x004B8810 script-name registration and NO 0x004169D0 lookup in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The three-number check for this body: FIVE static append sites, FIVE records written, container
//   grown by FIVE - all three agree, and because there is no loop the static and dynamic counts coincide by construction. **The id slot 0x0053C2FC
//   is in writable `.data`** (the section table puts `.rdata` at VA 0x00533000 with virtual size 0x28F6, ending at 0x005358F6) and its image value is
//   0x00000181, so its initialiser is 0x0181U and its symbol class is `D`. The container slot 0x0057BF3C has image value 0, so its initialiser is 0
//   and its symbol class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the
//   symbol classification rather than the section. The deltas are not monotonic (0x290, 0x2A0, 0x240, 0x2B4, 0x2B0), so each was resolved from its
//   own `lea` rather than from a stride. Arg6 of 0x004F1A80 is 0 at all five sites here (checked per site). Deferred pushes: at every getter-fed
//   link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument. Handler immediates
//   0x0043B490, 0x0043B500, 0x0043B650, 0x0043B680, 0x0043B690, 0x0043B6E0 and 0x0043B790 are raw code immediates with no implementation in the
//   tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the
//   arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are
//   reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (four times),
//   0x8, 0x18 and 0x2D8 are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port, which
//   cannot change behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the phase
//   index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_481340(void);            // 0x00481340: parent type getter (sys/native_481340.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_481350(void);            // 0x00481350: the existing parameter container (sys/native_481350.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1210(void);            // 0x004F1210: `mov eax,[0x54DDA8]; ret` (image 255)
extern "C" std::uint32_t Sys_GetGlobal32_4F1220(void);            // 0x004F1220: `mov eax,[0x54DDAC]; ret` (image 255)
// 0x004F1980 (script/native_4f1980.cpp): takes the container and a count, returns the container pointer.
extern "C" void* Script_TaskContainer_Grow(std::int32_t* container, std::int32_t count);
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C2FC: the "Generator" task type id, in writable `.data`; image value 0x00000181, so its initialiser is 0x0181U and its symbol class is `D`.
// Only the low 16 bits are ever written and read.
std::uint16_t g_53c2fc = 0x0181U;

// 0x0057BF3C: the grown parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57bf3c = 0U;

void Task_DefineGenerator()
{
    // 0x0043B2F0: sub esp,0x2C0 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043B2F6: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043B2F8: push 0 (the parent getter's inert argument);
    // 0x0043B2FA: call 0x481340; 0x0043B2FF: push eax; 0x0043B300: push 0x2C0; 0x0043B305: push "Generator"; 0x0043B30A: call 0x401900.
    const std::uint16_t generator_id = TaskType_Register("Generator",             // 0x0043B305: push 0x0053C300
                                                         0x2C0U,                   // 0x0043B300: push 0x2C0 (object size, equal to the frame)
                                                         Sys_GetGlobal16_481340(), // 0x0043B2FA: call 0x481340 (parent type)
                                                         0U);                     // 0x0043B2F8: push 0 (fourth argument)
    ::g_53c2fc = generator_id; // 0x0043B317: mov word [0x53C2FC],ax - between the pushes of the first link

    // 0x43B30F: push 0x0043B490 (handler, deferred argument 3); 0x43B314: push 0x0 (argument 2); 0x43B31D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2fc), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B490U)));

    // 0x43B328: push 0x0043B500 (handler, deferred argument 3); 0x43B32D: push 0x1 (argument 2); 0x43B330: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2fc), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B500U)));

    // 0x43B33C: push 0x0043B650 (handler, deferred argument 3); 0x43B341: push 0x2 (argument 2); 0x43B344: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2fc), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B650U)));

    // 0x43B34C: push 0x0043B680 (handler, deferred argument 3); 0x43B351: call 0x4F1A60 -> argument 2; 0x43B35F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2fc), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B680U)));

    // 0x43B367: push 0x0043B690 (handler, deferred argument 3); 0x43B36C: call 0x4F1A70 -> argument 2; 0x43B379: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2fc), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B690U)));

    // 0x43B381: push 0x0043B6E0 (handler, deferred argument 3); 0x43B386: call 0x4F1210 -> argument 2; 0x43B394: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2fc), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1210()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B6E0U)));

    // 0x43B39C: push 0x0043B790 (handler, deferred argument 3); 0x43B3A1: call 0x4F1220 -> argument 2; 0x43B3AF: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2fc), static_cast<std::uint32_t>(Sys_GetGlobal32_4F1220()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043B790U)));

    // 0x0043B3B7: push 0x5 (argument 2 = the number of entries to grow by); 0x0043B3B9: call 0x481350 (argument 1 = the existing container);
    // 0x0043B3BE: push eax (argument 1); 0x0043B3BF: call 0x4F1980; 0x0043B3C4: add esp,0x8; 0x0043B3C7: mov [0x57BF3C],eax.
    ::g_57bf3c = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(
        Script_TaskContainer_Grow(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(Sys_GetGlobal32_481350())), 5))); // 0x0043B3B7: push 0x5

    // 0x43B3CC: push $0x0 (arg6); 0x43B3D0: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43B3EA: call 0x004F1A80 (frame+0x00, frame+0x290).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf3c)),
                                append_frame, append_frame + 0x290,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43B3F2: push $0x0 (arg6); 0x43B3F6: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43B410: call 0x004F1A80 (frame+0x00, frame+0x2A0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf3c)),
                                append_frame, append_frame + 0x2A0,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43B418: push $0x0 (arg6); 0x43B41C: call Sys_GetGlobal32_4F0FC0 (arg4); 0x43B436: call 0x004F1A80 (frame+0x00, frame+0x240).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf3c)),
                                append_frame, append_frame + 0x240,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43B43E: push $0x0 (arg6); 0x43B442: call Camera_Get_4F1000 (arg4); 0x43B45C: call 0x004F1A80 (frame+0x00, frame+0x2B4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf3c)),
                                append_frame, append_frame + 0x2B4,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43B464: push $0x0 (arg6); 0x43B468: call Sys_GetGlobal32_4F0F50 (arg4); 0x43B482: call 0x004F1A80 (frame+0x00, frame+0x2B0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf3c)),
                                append_frame, append_frame + 0x2B0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
} // end of Task_DefineGenerator

} // extern "C"
