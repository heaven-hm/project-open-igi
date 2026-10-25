// Native: Task_DefineSimpointObjGenerator (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043B850-0x0043BB19.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 684 bytes, 1 block, 196 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 196 instructions / 684 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043BB19 and 0x0043BB20 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C320 for the name literal (byte-identical to "SimpointObjGenerator");
//   `px 4` at 0x0053C31C (image value 0x00000181), at 0x0057BF60 (image value 0) and at each of the ten 0x0057BF4x/0x0057BF6x slots this body
//   writes (all image value 0). **The prologue was read before any `lea` was resolved: it is `sub esp,0x158` followed immediately by argument
//   pushes, with NO saved-register pushes, so the frame-relative shift for this body is ZERO.** A full push-by-push simulation closes exactly: the
//   registration's `add esp,0x38` is fourteen pushes, the container's `add esp,0x10` is its own count push plus the three pushes of the preceding
//   link, each remaining link's `add esp,0xC` is its three, and the final `add esp,0x158`-equivalent epilogue matches the prologue with no saved
//   registers to pop. Every callee was swept against the tree's `// Native:` provenance index with no lane exemption and every one resolves to an
//   existing file under the same name, return type and linkage.
// Behavior/CFG: one straight-line body with a 0x158-byte frame and no stack probe. It registers the "SimpointObjGenerator" task type of size 0x158
//   with parent Sys_GetGlobal16_4C48C0() -> id at 0x0053C31C, stores the id, then links FIVE handlers: e0->0x0043BB20, e1->0x0043BCA0 and
//   e2->0x00426860 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043BE50 and 0x004F1A70->0x0043BDA0. It then allocates a parameter
//   container through Script_TaskContainer_Alloc(0xD) into 0x0057BF60, initialises ten dword slots of the 0x0057BF4x/0x0057BF6x block, and appends
//   TWELVE records through 0x004F1A80 with recorded deltas 0x20, 0x70, 0xF4, 0x104, 0x114, 0x124, 0x128, 0x12C, 0x130, 0x134, 0x13C and 0x148.
//   One basic block, no branch, no loop, 34 call sites over 14 distinct callees. There is NO 0x004B8810 script-name registration and NO
//   0x004169D0 lookup in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **This body is the case that limits the container-count check, and the limit is a fact about
//   Script_TaskContainer_Alloc rather than about this transcription: the allocation argument is a CAPACITY, not the number of records.** Reading
//   0x004F1950 (`size = count*16 + 12`; then `[base+4] = count`, `[base] = 0`, `[base+8] = base+12`) together with 0x004F1A80 (which writes each
//   16-byte record at `payload + [container]*16` and then increments `[container]`) shows that `[container]` is a LIVE COUNT starting at zero and
//   `[container+4]` is the capacity. Here the capacity is 0xD = THIRTEEN while this body appends TWELVE records, so capacity and record count are
//   NOT equal in general: the extra slot is headroom (this body is a single basic block, so twelve static sites are exactly twelve records at
//   runtime, with no loop that could make the two numbers differ). In the earlier bodies where the two agreed - 0x004397C0 (8), 0x0043AD80 (6),
//   0x0043A840 (12), 0x0043B2F0 (5) - the equality was a true observation but not a law, and the check is properly stated as `records <= capacity`
//   with a loop's trip count confirmed only when the two happen to meet exactly. This file records all three numbers anyway (capacity 13, static
//   sites 12, records 12) so the next reader sees the discrepancy and its explanation rather than a false equality.
//   **The id slot 0x0053C31C is in writable `.data`** (the section table puts `.rdata` at VA 0x00533000 with virtual size 0x28F6, ending at
//   0x005358F6) and its image value is 0x00000181, so its initialiser is 0x0181U and its symbol class is `D`. The container slot 0x0057BF60 and
//   the ten initialised slots have image value 0, so their initialisers are 0 and their symbol class is `B`; this image has no `.bss` section, so a
//   zero-image global is `.data` with a zero initialiser and `B`/`D` is the symbol classification rather than the section. **FIVE of the ten
//   initialised slots are ADDRESS-TAKEN** - 0x57BF48, 0x57BF50, 0x57BF58, 0x57BF68 and 0x57BF70 are each passed as an append's sixth argument -
//   so half of this block is not "just constants": the appended records point back into it. The remaining five (0x57BF4C, 0x57BF54, 0x57BF5C,
//   0x57BF6C, 0x57BF74) are written and never referenced by address in this body. The stored dwords include the exact bit patterns 0x3F800000
//   (1.0f read as a float) and 0x447A0000 (1000.0f) and the integers 0x258 (600) and 0x12C (300); they are transcribed as raw dwords with no
//   reinterpretation. **The store run is interleaved with the first append's two argument pushes** (0x43B901 pushes arg6 = 0 and 0x43B903 pushes
//   arg5 = 7 in the middle of the run); because those two arguments are literals the port keeps the run's order exactly and places it where retail
//   finishes it. Arg6 is per-site: 0 at seven appends and one of the five address-taken globals at the other five. Deferred pushes: at every
//   getter-fed link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument. Handler
//   immediates 0x0043BB20, 0x0043BCA0, 0x00426860, 0x0043BE50 and 0x0043BDA0 are raw code immediates with no implementation in the tree yet;
//   0x00426860 is one of the image's shared handlers. Documented shape differences, all value-preserving: (1) every getter callee is an
//   argument-less load in retail that ignores the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; (2) the cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp`
//   arithmetic; the retail sizes 0x38, 0xC (three times), 0x10 and 0x18 (twelve times) are recorded so a future reviewer can check the arithmetic;
//   (3) byte-valued event ids are zero-extended in the port, which cannot change behaviour because every link fed by a byte getter goes through
//   0x00401400 or 0x00401530, which mask the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: parent type getter (sys/native_4c48c0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F60(void);            // 0x004F0F60: `mov eax,[0xA70C68]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F70(void);            // 0x004F0F70: `mov eax,[0xA70C6C]; ret`
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` (core/native_4f0fa0.cpp)
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

// 0x0053C31C: the "SimpointObjGenerator" task type id, in writable `.data`; image value 0x00000181, so its initialiser is 0x0181U and its symbol class
// is `D`. Only the low 16 bits are ever written and read.
std::uint16_t g_53c31c = 0x0181U;

// 0x0057BF60: the parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`. Its CAPACITY is 0xD (the argument passed
// to Script_TaskContainer_Alloc below) while this body appends twelve records, so the capacity is not the record count here.
std::uint32_t g_57bf60 = 0U;

// The ten 0x0057BF4x/0x0057BF6x slots this body initialises. Image value 0 for all of them, so every initialiser is 0 and every symbol class is `B`.
// The first five are ADDRESS-TAKEN (passed as the sixth argument of an append) and therefore cannot be folded away.
std::uint32_t g_57bf48 = 0U; // address-taken (append delta 0x20)
std::uint32_t g_57bf4c = 0U; // written only
std::uint32_t g_57bf50 = 0U; // address-taken (append delta 0x104)
std::uint32_t g_57bf54 = 0U; // written only
std::uint32_t g_57bf58 = 0U; // address-taken (append delta 0x114)
std::uint32_t g_57bf5c = 0U; // written only
std::uint32_t g_57bf68 = 0U; // address-taken (append delta 0x128)
std::uint32_t g_57bf6c = 0U; // written only
std::uint32_t g_57bf70 = 0U; // address-taken (append delta 0x130)
std::uint32_t g_57bf74 = 0U; // written only

void Task_DefineSimpointObjGenerator()
{
    // 0x0043B850: sub esp,0x158 - a plain frame; this body has NO stack probe and NO saved-register pushes, so no frame-relative shift applies.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043B856: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043B858: push 0 (the parent getter's inert argument);
    // 0x0043B85A: call 0x4C48C0; 0x0043B85F: push eax; 0x0043B860: push 0x158; 0x0043B865: push "SimpointObjGenerator"; 0x0043B86A: call 0x401900.
    const std::uint16_t simpoint_obj_generator_id = TaskType_Register("SimpointObjGenerator", // 0x0043B865: push 0x0053C320
                                                                     0x158U,                  // 0x0043B860: push 0x158 (object size, equal to the frame)
                                                                     Sys_GetGlobal16_4C48C0(), // 0x0043B85A: call 0x4C48C0 (parent type)
                                                                     0U);                     // 0x0043B858: push 0 (fourth argument)
    ::g_53c31c = simpoint_obj_generator_id; // 0x0043B877: mov word [0x53C31C],ax - between the pushes of the first link

    // 0x43B86F: push 0x0043BB20 (handler, deferred argument 3); 0x43B874: push 0x0 (argument 2); 0x43B87D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c31c), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043BB20U)));

    // 0x43B888: push 0x0043BCA0 (handler, deferred argument 3); 0x43B88D: push 0x1 (argument 2); 0x43B890: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c31c), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043BCA0U)));

    // 0x43B89C: push 0x00426860 (handler, deferred argument 3); 0x43B8A1: push 0x2 (argument 2); 0x43B8A4: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c31c), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x00426860U)));

    // 0x43B8AC: push 0x0043BE50 (handler, deferred argument 3); 0x43B8B1: call 0x4F1A60 -> argument 2; 0x43B8BF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c31c), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043BE50U)));

    // 0x43B8C7: push 0x0043BDA0 (handler, deferred argument 3); 0x43B8CC: call 0x4F1A70 -> argument 2; 0x43B8D9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c31c), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043BDA0U)));

    // 0x0043B8DE: push 0xD (the container CAPACITY - thirteen slots, of which this body fills twelve); 0x0043B8E0: call 0x4F1950;
    // 0x0043B8E5: add esp,0x10; 0x0043B8E8: mov [0x57BF60],eax.
    ::g_57bf60 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xD)); // 0x0043B8DE: push 0xD

    // 0x0043B8ED-0x0043B94B: ten dword stores into the 0x0057BF4x/0x0057BF6x block, in retail order. The first append's two literal argument
    // pushes (0x0043B901: arg6 = 0, 0x0043B903: arg5 = 7) sit inside this run in retail; they are reproduced as the first append's constants
    // below, so the run's own order is preserved exactly and nothing observable moves.
    ::g_57bf48 = 0x0U;          // 0x0043B8ED
    ::g_57bf4c = 0x3F800000U;   // 0x0043B8F7 (1.0f as bits)
    ::g_57bf68 = 0x0U;          // 0x0043B905
    ::g_57bf6c = 0x3F800000U;   // 0x0043B90F (1.0f as bits)
    ::g_57bf70 = 0x0U;          // 0x0043B919
    ::g_57bf74 = 0x447A0000U;   // 0x0043B923 (1000.0f as bits)
    ::g_57bf58 = 0x0U;          // 0x0043B92D
    ::g_57bf5c = 0x258U;        // 0x0043B937 (600)
    ::g_57bf50 = 0x0U;          // 0x0043B941
    ::g_57bf54 = 0x12CU;        // 0x0043B94B (300)

    // 0x43B901: push $0x0 (arg6); 0x43B955: call Sys_GetGlobal32_4F1010 (arg4); 0x43B96B: call 0x004F1A80 (frame+0x00, frame+0x20).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43B973: push $0x0 (arg6); 0x43B977: call Sys_GetGlobal32_4F0F70 (arg4); 0x43B98D: call 0x004F1A80 (frame+0x00, frame+0x70).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F70()), 7, 0);

    // 0x43B995: push $0x0 (arg6); 0x43B999: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43B9B2: call 0x004F1A80 (frame+0x00, frame+0xF4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0xF4,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43B9BA: push $0x0 (arg6); 0x43B9BE: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43B9D7: call 0x004F1A80 (frame+0x00, frame+0x104).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x104,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43B9DF: push $0x0 (arg6); 0x43B9E3: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43B9FC: call 0x004F1A80 (frame+0x00, frame+0x114).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x114,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43BA04: push $0x57bf58 (arg6); 0x43BA0B: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43BA24: call 0x004F1A80 (frame+0x00, frame+0x124).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x124,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bf58)));

    // 0x43BA2C: push $0x57bf50 (arg6); 0x43BA33: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43BA4C: call 0x004F1A80 (frame+0x00, frame+0x128).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x128,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bf50)));

    // 0x43BA54: push $0x57bf48 (arg6); 0x43BA5B: call Sys_GetGlobal32_4F0F50 (arg4); 0x43BA74: call 0x004F1A80 (frame+0x00, frame+0x12C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x12C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bf48)));

    // 0x43BA7C: push $0x57bf68 (arg6); 0x43BA83: call Sys_GetGlobal32_4F0F50 (arg4); 0x43BA9C: call 0x004F1A80 (frame+0x00, frame+0x130).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x130,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bf68)));

    // 0x43BAA4: push $0x57bf70 (arg6); 0x43BAAB: call Sys_GetGlobal32_4F0F50 (arg4); 0x43BAC4: call 0x004F1A80 (frame+0x00, frame+0x134).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x134,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bf70)));

    // 0x43BACC: push $0x0 (arg6); 0x43BAD0: call Sys_GetGlobal32_4F0F60 (arg4); 0x43BAE9: call 0x004F1A80 (frame+0x00, frame+0x13C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x13C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);

    // 0x43BAF1: push $0x0 (arg6); 0x43BAF5: call Sys_GetGlobal32_4F0F60 (arg4); 0x43BB0E: call 0x004F1A80 (frame+0x00, frame+0x148).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bf60)),
                                append_frame, append_frame + 0x148,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F60()), 7, 0);
} // end of Task_DefineSimpointObjGenerator

} // extern "C"
