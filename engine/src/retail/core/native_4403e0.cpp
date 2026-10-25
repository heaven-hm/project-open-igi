// Native: Task_DefineLadder (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x004403E0-0x004404BD.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 222 bytes, 1 block, 59 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 59 instructions / 222 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x004404BD and 0x004404C0 is the next
//   function, which this body uses as a handler). `ps` at 0x0053C538 (the name literal, byte-identical to "Ladder") and at 0x0053C528 (the
//   script-hash key, byte-identical to "TASKTYPE_LADDER"); `px 4` at 0x0057C15C (the id slot, image value 0). **This body has NO frame at all: the
//   first instruction is a push, not a `sub esp`, and there are no saved-register pushes either** - so there is no `lea` in this body at all, no
//   frame-relative address, and no frame-relative shift to apply. A full push-by-push simulation closes exactly: the registration's `add esp,0x2C`
//   is eleven pushes (the five registration pushes plus the first and second links' three each), the third to sixth links' `add esp,0xC` is three
//   each (four of them), and the tail's `add esp,0x14` is the SEVENTH link's three pushes plus the registration call's two. Every callee was swept
//   against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file under the same name, return
//   type and linkage.
// Behavior/CFG: one straight-line body with no frame, no stack probe, no parameter container and NO append calls at all - the first task-registrar in
//   this lane with an empty parameter list. It registers the "Ladder" task type of size 0x140 with parent Sys_GetGlobal16_4D9FA0() -> id at
//   0x0057C15C, stores the id and links SEVEN handlers: e0->0x00440280 and **e2->0x004403C0** with the literal event ids 0 and 2, then
//   0x004F1A70->0x004D3710, 0x004167F0->0x004404C0, 0x00416810->0x004406C0, 0x00416800->0x004D3710 and 0x00416820->0x004406D0. It then registers the
//   type in the script hash: `mov eax,[0x57C15C]; and 0xFFFF; push eax; push 0x0053C528; call 0x4B8810`, i.e. Script_QHashNodeRegister("TASKTYPE_LADDER",
//   id) with the id RELOADED from the global and masked to sixteen bits. One basic block, no branch, no loop, 15 call sites over 10 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check is vacuous here and is reported as such rather than skipped: there is no
//   container, no allocation and no append site, so capacity and record count are both zero because nothing is ever appended.** That is a different
//   statement from "capacity equals records" and should not be counted as evidence for that rule.
//   **The literal event ids are 0 and 2 - there is NO event id 1 link.** A reader who assumes the family's usual 0/1/2 triple would invent a handler
//   that retail never links; the census (0x00401530 x1, not x2) is what makes the gap visible.
//   **Two of the seven links use the image-wide shared no-op handler 0x004D3710** (QTask_NoOpHandler, 163 call sites image-wide) - it is a legitimate
//   handler at both sites, fed by different getters, and is not a transcription gap. The id slot 0x0057C15C is in writable `.data` (the section table
//   puts `.rdata` at VA 0x00533000 with virtual size 0x28F6, ending at 0x005358F6 and `.data` at VA 0x00536000) and has image value 0, so its
//   initialiser is 0 and its symbol class is `B`; this image has no `.bss` section, so a zero-image global is `.data` with a zero initialiser and
//   `B`/`D` is the symbol classification rather than the section.
//   Deferred pushes: at every getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's third
//   argument. Handler immediates 0x00440280, 0x004403C0, 0x004404C0, 0x004406C0 and 0x004406D0 are raw code immediates with no implementation in
//   the tree yet; 0x004D3710 is transcribed as script/native_4d3710.cpp and 0x004403C0 is the next-but-one sibling in this family. Documented shape
//   differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments the caller pushes, and
//   the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's
//   calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x2C, 0xC (four times) and 0x14 are recorded so a future
//   reviewer can check the arithmetic; (3) the registration's identifier is reloaded from the global and masked exactly as retail does.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4D9FA0(void);            // 0x004D9FA0: argument-less word getter (parent type, sys/native_4d9fa0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_4167F0(void);          // 0x004167F0: argument-less byte getter (config/native_4167f0.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416800(void);          // 0x00416800: argument-less byte getter (config/native_416800.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416810(void);          // 0x00416810: argument-less byte getter (config/native_416810.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416820(void);          // 0x00416820: argument-less byte getter (config/native_416820.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)

extern "C" {

// 0x0057C15C: the "Ladder" task type id, in writable `.data`. Image value 0, so the initialiser is 0 and the symbol class is `B`. Only the low 16
// bits are ever stored.
std::uint16_t g_57c15c = 0U;

void Task_DefineLadder()
{
    // 0x004403E0: push 0 (TaskType_Register's fourth argument, pushed first); 0x004403E2: push 0 (the parent getter's inert argument);
    // 0x004403E4: call 0x4D9FA0; 0x004403E9: push eax; 0x004403EA: push 0x140; 0x004403EF: push "Ladder"; 0x004403F4: call 0x401900.
    // There is NO `sub esp` in this body, so it has no frame of its own and needs none of the append_frame machinery.
    const std::uint16_t ladder_id = TaskType_Register("Ladder",                 // 0x004403EF: push 0x0053C538
                                                      0x140U,                   // 0x004403EA: push 0x140 (object size)
                                                      Sys_GetGlobal16_4D9FA0(), // 0x004403E4: call 0x4D9FA0 (parent type)
                                                      0U);                      // 0x004403E2: push 0 (fourth argument)
    ::g_57c15c = ladder_id; // 0x00440401: mov word [0x57C15C],ax - between the pushes of the first link

    // 0x4403F9: push 0x440280 (handler, deferred argument 3);
    // 0x4403FE: push $0 (argument 2, literal event id); 0x440407: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x440280U)));

    // 0x440412: push 0x4403C0 (handler, deferred argument 3);
    // 0x440417: push $2 (argument 2, literal event id); 0x44041A: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_57c15c), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4403C0U)));

    // 0x440422: push 0x4D3710 (handler, deferred argument 3) (the image-wide shared no-op handler, script/native_4d3710.cpp); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x440435: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x44043D: push 0x4404C0 (handler, deferred argument 3); the Sys_GetSettingByte_4167F0 getter supplies argument 2;
    // 0x440450: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c),
        static_cast<std::uint32_t>(Sys_GetSettingByte_4167F0()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4404C0U)));

    // 0x440458: push 0x4406C0 (handler, deferred argument 3); the Sys_GetSettingByte_416810 getter supplies argument 2;
    // 0x44046A: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416810()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4406C0U)));

    // 0x440472: push 0x4D3710 (handler, deferred argument 3) (the image-wide shared no-op handler, script/native_4d3710.cpp); the Sys_GetSettingByte_416800 getter supplies argument 2;
    // 0x440485: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416800()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4D3710U)));

    // 0x44048D: push 0x4406D0 (handler, deferred argument 3); the Sys_GetSettingByte_416820 getter supplies argument 2;
    // 0x4404A0: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_57c15c),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416820()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x4406D0U)));


    // 0x004404A5: mov eax,[0x57C15C] (the id is RELOADED from the global); 0x004404AA: and $0xffff,%eax; 0x004404AF: push eax (argument 2);
    // 0x004404B0: push 0x0053C528 ("TASKTYPE_LADDER", argument 1); 0x004404B5: call 0x4B8810;
    // 0x004404BA: add esp,0x14 - the seventh link's three pushes plus this call's two; 0x004404BD: ret.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_57c15c) & 0xFFFFU; // 0x004404AA: and $0xffff,%eax
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_LADDER"),                          // 0x004404B0: push 0x0053C528
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x004404AF: push eax
} // end of Task_DefineLadder

} // extern "C"
