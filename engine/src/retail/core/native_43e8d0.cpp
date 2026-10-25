// Native: Task_DefineGenericPhysicsObj (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043E8D0-0x0043EA9C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 461 bytes, 1 block, 133 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 133 instructions / 461 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043EA9C and 0x0043EAA0 is the next
//   function, which this body uses as its first handler). `ps` at 0x0053C4C8 (the name literal, byte-identical to "GenericPhysicsObj") and at
//   0x0053C4AC (byte-identical to "TASKTYPE_GENERICPHYSICSOBJ"); `px 4` at 0x0053C4A8 (image value 0x00000181 - a NON-ZERO initialiser, see the
//   status block) and at 0x0057C13C (image value 0). The prologue was read before any `lea` was resolved: `sub esp,0x720` followed immediately by
//   argument pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO. A full push-by-push simulation closes
//   exactly: the registration's `add esp,0x38` is fourteen pushes, the fourth, fifth, sixth and seventh links' `add esp,0xC` is three each, the
//   container's `add esp,0x10` is its own count push plus the SEVENTH link's three pushes (whose cleanup is folded into it), the first five
//   appends' `add esp,0x18` is six each, and **the SIXTH append has NO `add esp,0x18` of its own: its six pushes are still outstanding when the
//   registration's two pushes follow, and all eight are removed by the single `add esp,0x740` at 0x0043EA96** (0x740 = the 0x720 frame plus 0x20).
//   Every callee was swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an existing file
//   under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x720-byte frame and no stack probe. It registers the "GenericPhysicsObj" task type of size 0x720
//   with parent Sys_GetGlobal16_4FB5E0() -> id at 0x0053C4A8, stores the id, then links SEVEN handlers: e0->0x0043EAA0, e1->0x0043EBA0 and
//   e2->0x0043E650 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043EC20, 0x004F1A70->0x0043EC80, 0x00416840->0x0043EDF0 and
//   0x00416860->0x0043E7A0. It then allocates a parameter container through Script_TaskContainer_Alloc(0x14) into 0x0057C13C and appends SIX records
//   through 0x004F1A80 with recorded deltas 0x168, 0x70, 0x180, 0x154, 0x6D8 and 0x6D9, and finally registers the type in the script hash:
//   `mov eax,[0x53C4A8]; and 0xFFFF; push eax; push 0x0053C4AC; call 0x4B8810`, i.e. Script_QHashNodeRegister("TASKTYPE_GENERICPHYSICSOBJ", id)
//   with the id RELOADED from the global and masked to sixteen bits. One basic block, no branch, no loop, 27 call sites over 16 distinct callees.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The three-number check is a bound here, not an equality: capacity 0x14 = TWENTY, static append
//   sites SIX, and because the body is a single basic block the records written are SIX too, so 6 <= 20.** Per the correction recorded in
//   native_43b850.cpp the allocation argument is a CAPACITY (`[container+4]`, with `[container]` a live count starting at zero), so this body's
//   capacity does NOT confirm a record count - it only bounds one, and the count comes from the six sites. This is the same shape as 0x0043B850
//   (capacity 0xD, twelve records) and is the second counterexample to "allocation == record count".
//   **The id slot 0x0053C4A8 has a NON-ZERO image value, 0x00000181, so its initialiser is 0x181 and its symbol class is `D` rather than `B`** - the
//   first id slot in this lane that is not zero-initialised. **That value is now explained with evidence rather than left open: 0x0181 is the
//   unregistered sentinel of a word-sized task-id slot.** All six word-sized slots in this image that carry the image value 0x0181 - 0x0053C1DC,
//   0x0053C2D0, 0x0053C2FC, 0x0053C31C, 0x0053C2E0 and 0x0053C4A8 - are written by a registrar with `mov word [slot],ax` immediately after a
//   TaskType_Register call, and this body is one of them; the value is therefore a placeholder that every registrar overwrites, not a constant the
//   game reads. The slot is in writable `.data` (the section table puts `.rdata` at VA 0x00533000 with virtual size 0x28F6, ending at 0x005358F6,
//   and `.data` at VA 0x00536000), which is why a store to an address surrounded by string literals is legal at all; this image has no `.bss`
//   section, so a zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the section. The
//   container slot 0x0057C13C has image value 0, so its initialiser is 0 and its symbol class is `B`.
//   **The two final deltas differ by exactly ONE byte (0x6D8 and 0x6D9)** - the second-to-last site's `lea` is 0x6F0(%esp) and the last site's is
//   0x6E5(%esp) at a point where three pushes are outstanding, so the two records point one byte apart. This is the second body in this lane with an
//   adjacent odd/even delta pair (0x0043CC70 had 0x18C/0x18D), so it is a pattern rather than a slip, and both were verified against the bytes.
//   Arg6 of 0x004F1A80 is 0 at all six sites (checked per site), so this body takes the address of none of its own slots. Deferred pushes: at every
//   getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument. Handler
//   immediates 0x0043EAA0, 0x0043EBA0, 0x0043E650, 0x0043EC20, 0x0043EC80, 0x0043EDF0 and 0x0043E7A0 are raw code immediates with no
//   implementation in the tree yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail
//   that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the
//   cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC
//   (four times), 0x10, 0x18 (five times), 0x20 and 0x740 are recorded so a future reviewer can check the arithmetic; (3) the registration's
//   identifier is reloaded from the global and masked exactly as retail does, rather than reusing the local that was just stored.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4FB5E0(void);            // 0x004FB5E0: argument-less word getter (parent type, sys/native_4fb5e0.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416840(void);          // 0x00416840: argument-less byte getter (config/native_416840.cpp)
extern "C" std::uint8_t Sys_GetSettingByte_416860(void);          // 0x00416860: argument-less byte getter (config/native_416860.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret` (image 0)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F60(void);            // 0x004F0F60: `mov eax,[0xA70C68]; ret` (image 0)
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (image 0)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" void Script_QHashNodeRegister(void* const arg1, void* const value); // 0x004B8810 (script/native_4b8810.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C4A8: the "GenericPhysicsObj" task type id, in writable `.data`. **Image value 0x00000181, so the initialiser is 0x181 and the symbol class
// is `D`, not `B`.** Retail stores only the low 16 bits here and reloads the dword with an `and 0xFFFF` mask for the script-hash registration. 0x0181
// is the unregistered sentinel: every word-sized slot in this image carrying that image value is overwritten by its registrar.
std::uint32_t g_53c4a8 = 0x181U;

// 0x0057C13C: the parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57c13c = 0U;

void Task_DefineGenericPhysicsObj()
{
    // 0x0043E8D0: sub esp,0x720 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043E8D6: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043E8D8: push 0 (the parent getter's inert argument);
    // 0x0043E8DA: call 0x4FB5E0; 0x0043E8DF: push eax; 0x0043E8E0: push 0x720; 0x0043E8E5: push "GenericPhysicsObj"; 0x0043E8EA: call 0x401900.
    const std::uint16_t generic_physics_obj_id = TaskType_Register("GenericPhysicsObj",        // 0x0043E8E5: push 0x0053C4C8
                                                                   0x720U,                    // 0x0043E8E0: push 0x720 (object size, equal to the frame)
                                                                   Sys_GetGlobal16_4FB5E0(),  // 0x0043E8DA: call 0x4FB5E0 (parent type)
                                                                   0U);                      // 0x0043E8D8: push 0 (fourth argument)
    ::g_53c4a8 = generic_physics_obj_id; // 0x0043E8F7: mov word [0x53C4A8],ax - between the pushes of the first link

    // 0x43E8EF: push 0x43EAA0 (handler, deferred argument 3); 0x43E8F4: push $0 (argument 2, literal event id);
    // 0x43E8FD: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c4a8), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43EAA0U)));

    // 0x43E908: push 0x43EBA0 (handler, deferred argument 3); 0x43E90D: push $1 (argument 2, literal event id);
    // 0x43E910: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c4a8), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43EBA0U)));

    // 0x43E91C: push 0x43E650 (handler, deferred argument 3); 0x43E921: push $2 (argument 2, literal event id);
    // 0x43E924: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c4a8), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43E650U)));

    // 0x43E92C: push 0x43EC20 (handler, deferred argument 3); the Script_PhaseIdGetE4_4F1A60 getter supplies argument 2;
    // 0x43E93F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c4a8),
        static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43EC20U)));

    // 0x43E947: push 0x43EC80 (handler, deferred argument 3); the Script_PhaseIdGetE5_4F1A70 getter supplies argument 2;
    // 0x43E959: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c4a8),
        static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43EC80U)));

    // 0x43E961: push 0x43EDF0 (handler, deferred argument 3); the Sys_GetSettingByte_416840 getter supplies argument 2;
    // 0x43E974: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c4a8),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416840()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43EDF0U)));

    // 0x43E97C: push 0x43E7A0 (handler, deferred argument 3); the Sys_GetSettingByte_416860 getter supplies argument 2;
    // 0x43E98F: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c4a8),
        static_cast<std::uint32_t>(Sys_GetSettingByte_416860()), // argument 2
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x43E7A0U)));


    // 0x0043E994: push 0x14 (the container CAPACITY - twenty slots, a bound on the six records below); 0x0043E996: call 0x4F1950;
    // 0x0043E99B: add esp,0x10 - the seventh link's three pushes plus this count push; 0x0043E99E: mov [0x57C13C],eax.
    ::g_57c13c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x14)); // 0x0043E994: push 0x14

    // 0x43E9A3: push $0x0 (arg6); 0x43E9C1: call 0x004F1A80 (frame+0x00, frame+0x168); argument 4 is the result of Sys_GetGlobal32_4F1010().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x168,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43E9C9: push $0x0 (arg6); 0x43E9E4: call 0x004F1A80 (frame+0x00, frame+0x70); argument 4 is the result of Sys_GetGlobal32_4F0F70().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x43E9EC: push $0x0 (arg6); 0x43EA0A: call 0x004F1A80 (frame+0x00, frame+0x180); argument 4 is the result of Sys_GetGlobal32_4F0F60().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x180,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);

    // 0x43EA12: push $0x0 (arg6); 0x43EA30: call 0x004F1A80 (frame+0x00, frame+0x154); argument 4 is the result of Core_GetRenderWordB_4F0FD0().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x154,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43EA38: push $0x0 (arg6); 0x43EA56: call 0x004F1A80 (frame+0x00, frame+0x6D8); argument 4 is the result of Camera_Get_4F1000().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x6D8,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);

    // 0x43EA5E: push $0x0 (arg6); 0x43EA7C: call 0x004F1A80 (frame+0x00, frame+0x6D9); argument 4 is the result of Camera_Get_4F1000().
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57c13c)),
                                append_frame, append_frame + 0x6D9,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);


    // 0x0043EA81: mov eax,[0x53C4A8] (the id is RELOADED from the global, not taken from the local that was just stored);
    // 0x0043EA86: and $0xffff,%eax (zero-extended to a dword); 0x0043EA8B: push eax (argument 2);
    // 0x0043EA8C: push 0x0053C4AC ("TASKTYPE_GENERICPHYSICSOBJ", argument 1); 0x0043EA91: call 0x4B8810;
    // 0x0043EA96: add esp,0x740 - the sixth append's six pushes plus this call's two, all removed at once.
    const std::uint32_t registered_id = static_cast<std::uint32_t>(::g_53c4a8) & 0xFFFFU; // 0x0043EA86: and $0xffff,%eax
    Script_QHashNodeRegister(const_cast<char*>("TASKTYPE_GENERICPHYSICSOBJ"),               // 0x0043EA8C: push 0x0053C4AC
                             reinterpret_cast<void*>(static_cast<std::uintptr_t>(registered_id))); // 0x0043EA8B: push eax
    // 0x0043EA9C: ret - the frame is released by the `add esp,0x740` above; the port's locals take its place.
} // end of Task_DefineGenericPhysicsObj

} // extern "C"
