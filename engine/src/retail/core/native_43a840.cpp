// Native: Task_DefineMineField (behavior-derived placeholder name from the byte-identical name literal; retail symbol unknown); retail VA 0x0043A840-0x0043AB4E.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 735 bytes, 1 block, 202 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 202 instructions / 735 bytes / 0 edges) plus the
//   objdump listing build/retail-dis.txt for the same extent, instruction by instruction (the ret is at 0x0043AB4E and 0x0043AB50 is the next
//   function, which this body uses as its first handler); `ps` at 0x0053C2D4 for the name literal (byte-identical to "MineField"); `px 4` at
//   0x0053C2D0 (image value 0x00000181) and at all seventeen 0x0057BExx slots (all image value 0). Every one of the fourteen callees was swept
//   against the tree's `// Native:` provenance index before being declared, with no lane exemption, and every one resolves to an existing file
//   under the same name, return type and linkage; every callee VA was also re-read at its own address to confirm the return width.
//   A full push-by-push simulation of the body, closing exactly: the registration's `add esp,0x38` is fourteen pushes, the container's
//   `add esp,0x10` is the third link's three plus the count push, each getter-fed link's `add esp,0xC` is its three, and the final `add esp,0xE8`
//   is the 0xD0 frame plus the last append's six pushes. The five link sites and the twelve append deltas were generated mechanically with
//   assertions and then re-checked against retail.
// Behavior/CFG: one straight-line body with a 0xD0-byte frame and no stack probe. It registers the "MineField" task type of size 0xD0 with parent
//   Sys_GetGlobal16_5034D0() -> id at 0x0053C2D0, stores the id, then links five handlers through 0x00401400/0x00401530: e0->0x0043AB50,
//   e1->0x0043ABC0 and e2->0x0043AD20 with the literal event ids 0, 1 and 2, then 0x004F1A60->0x0043AD30 and 0x004F1A70->0x0043AD40. It then
//   allocates a TWELVE-entry parameter container through Script_TaskContainer_Alloc(0xC) into 0x0057BEF4 and appends twelve records through
//   0x004F1A80 with recorded deltas 0x20, 0x38, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0, 0xC0, 0xC4 and 0xC8. One basic block, no branch, no
//   loop, 34 call sites over 14 distinct callees. There is NO 0x004B8810 script-name registration and NO 0x004169D0 lookup in this body.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. **The id slot 0x0053C2D0 is in `.data`** - the section table puts `.rdata` at VA 0x00533000 with
//   virtual size 0x28F6, so it ends at 0x005358F6 and every 0x0053xxxx address above that is writable `.data`; its image value is 0x00000181, which
//   is non-zero, so its initialiser is 0x0181U and its symbol class is `D`, not `B`. The seventeen 0x0057BExx slots all have image value 0, so
//   their initialiser is 0 and their symbol class is `B`; note that this image has no `.bss` section at all - zero-image globals live in `.data`
//   with a zero initialiser, and `B`/`D` is the symbol classification, not the section. **Sixteen of those slots are initialised by this body, in
//   a single contiguous run of dword stores, and eight of them are ADDRESS-TAKEN: 0x57BEC0, 0x57BEE0, 0x57BED0, 0x57BEE8, 0x57BEC8, 0x57BEB8,
//   0x57BEB0 and 0x57BED8 are passed as the sixth argument of an append (0x43A9C1, 0x43A9E9, 0x43AA11, 0x43AA39, 0x43AA61, 0x43AA89,
//   0x43AAD6, 0x43AAFE)**, while the other eight are written and never referenced by address here. The stored dwords include the exact bit patterns
//   0x42C80000, 0x41200000, 0x42700000, 0xBF800000 and 0x461C4000 (100.0f, 10.0f, 60.0f, -1.0f and 10000.0f read as floats) and the integers
//   0x3E8, 0x64, 0x1000 and 0xFFFFFFFF; they are transcribed as raw dwords in the port, with no float or integer reinterpretation, so the stored
//   bits are exact either way. **The store run is interleaved with the first append's two argument pushes** (0x43A8F1 pushes arg6 = 0 and
//   0x43A8F3 pushes arg5 = 7 in the middle of the run) and it completes before the first append's getter is called at 0x43A981; because those two
//   arguments are literals, the port keeps the run's order exactly and places it before the first append, which is where retail finishes it.
//   Arg6 is per-site: 0 at four appends (deltas 0x20, 0x38, 0xA0, 0xC8) and one of the eight address-taken globals at the other eight. Deferred
//   pushes: at every link site the handler immediate is pushed before the event-id getter call and consumed as the link's third argument.
//   Handler immediates 0x0043AB50, 0x0043ABC0, 0x0043AD20, 0x0043AD30 and 0x0043AD40 are raw code immediates with no implementation in the tree
//   yet. Documented shape differences, all value-preserving: (1) every getter callee is an argument-less load in retail that ignores the arguments
//   the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; (2) the cleanups are reproduced
//   implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic; the retail sizes 0x38, 0xC (twice), 0x10 and 0xE8
//   are recorded so a future reviewer can check the arithmetic; (3) byte-valued event ids are zero-extended in the port, which cannot change
//   behaviour because every link fed by a byte getter goes through 0x00401400 or 0x00401530, which mask the event word for the phase index.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_5034D0(void);            // 0x005034D0: `mov ax,[0x54E634]; ret` (parent type; image 385)
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret`
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA71944]; ret` (core/native_4f0fa0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Core_GetRenderWordB_4F0FD0(void);        // 0x004F0FD0: `mov eax,[0xA71884]; ret` (core/native_4f0fd0.cpp)
extern "C" std::uint32_t Camera_Get_4F1000(void);                 // 0x004F1000: `mov eax,[0xA70C90]; ret` (render/native_4f1000.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70D10]; ret`
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script/native_4f1a60.cpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script/native_4f1a70.cpp)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x0053C2D0: the "MineField" task type id, in writable `.data`; image value 0x00000181, so its initialiser is 0x0181U and its symbol class is `D`.
// Only the low 16 bits are ever written and read.
std::uint16_t g_53c2d0 = 0x0181U;

// 0x0057BEF4: the twelve-entry parameter container. Image value 0, so the initialiser is 0 and the symbol class is `B`.
std::uint32_t g_57bef4 = 0U;

// The sixteen 0x0057BExx slots this body initialises. Image value 0 for all of them, so every initialiser is 0 and every symbol class is `B`.
// The first eight are ADDRESS-TAKEN (passed as the sixth argument of an append) and therefore cannot be folded away.
std::uint32_t g_57bec0 = 0U; // address-taken at 0x0043A9C1 (append delta 0x88)
std::uint32_t g_57bec4 = 0U; // written only
std::uint32_t g_57bec8 = 0U; // address-taken at 0x0043AA61 (append delta 0x98)
std::uint32_t g_57becc = 0U; // written only
std::uint32_t g_57bed0 = 0U; // address-taken at 0x0043AA11 (append delta 0x90)
std::uint32_t g_57bed4 = 0U; // written only
std::uint32_t g_57bed8 = 0U; // address-taken at 0x0043AAFE (append delta 0xC4)
std::uint32_t g_57bedc = 0U; // written only
std::uint32_t g_57bee0 = 0U; // address-taken at 0x0043A9E9 (append delta 0x8C)
std::uint32_t g_57bee4 = 0U; // written only
std::uint32_t g_57bee8 = 0U; // address-taken at 0x0043AA39 (append delta 0x94)
std::uint32_t g_57beec = 0U; // written only
std::uint32_t g_57beb0 = 0U; // address-taken at 0x0043AAD6 (append delta 0xC0)
std::uint32_t g_57beb4 = 0U; // written only
std::uint32_t g_57beb8 = 0U; // address-taken at 0x0043AA89 (append delta 0x9C)
std::uint32_t g_57bebc = 0U; // written only

void Task_DefineMineField()
{
    // 0x0043A840: sub esp,0xD0 - a plain frame; this body has NO stack probe.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x0043A846: push 0 (TaskType_Register's fourth argument, pushed first); 0x0043A848: push 0 (the parent getter's inert argument);
    // 0x0043A84A: call 0x5034D0; 0x0043A84F: push eax; 0x0043A850: push 0xD0; 0x0043A855: push "MineField"; 0x0043A85A: call 0x401900.
    const std::uint16_t mine_field_id = TaskType_Register("MineField",           // 0x0043A855: push 0x0053C2D4
                                                          0xD0U,                  // 0x0043A850: push 0xD0 (object size, equal to the frame)
                                                          Sys_GetGlobal16_5034D0(), // 0x0043A84A: call 0x5034D0 (parent type)
                                                          0U);                    // 0x0043A848: push 0 (fourth argument)
    ::g_53c2d0 = mine_field_id; // 0x0043A867: mov word [0x53C2D0],ax - between the pushes of the first link

    // 0x43A85F: push 0x0043AB50 (handler, deferred argument 3); 0x43A864: push 0x0 (argument 2); 0x43A86D: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2d0), 0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AB50U)));

    // 0x43A878: push 0x0043ABC0 (handler, deferred argument 3); 0x43A87D: push 0x1 (argument 2); 0x43A880: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2d0), 1U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043ABC0U)));

    // 0x43A88C: push 0x0043AD20 (handler, deferred argument 3); 0x43A891: push 0x2 (argument 2); 0x43A894: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53c2d0), 2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AD20U)));

    // 0x43A89C: push 0x0043AD30 (handler, deferred argument 3); 0x43A8A1: call 0x4F1A60 -> argument 2; 0x43A8AF: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2d0), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AD30U)));

    // 0x43A8B7: push 0x0043AD40 (handler, deferred argument 3); 0x43A8BC: call 0x4F1A70 -> argument 2; 0x43A8C9: call 0x401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53c2d0), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x0043AD40U)));

    // 0x0043A8CE: push 0xC (the container entry count, exactly the number of appends below); 0x0043A8D0: call 0x4F1950;
    // 0x0043A8D5: add esp,0x10 - the last link's three pushes (0xC) plus this count push; 0x0043A8D8: mov [0x57BEF4],eax.
    ::g_57bef4 = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0xC)); // 0x0043A8CE: push 0xC

    // 0x0043A8DD-0x0043A977: sixteen dword stores into the 0x0057BExx block, in retail order. The first append's two literal argument pushes
    // (0x0043A8F1: arg6 = 0, 0x0043A8F3: arg5 = 7) sit inside this run in retail; they are reproduced as the first append's constants below, so
    // the run's own order is preserved exactly and nothing observable moves.
    ::g_57bec0 = 0x0U;          // 0x0043A8DD
    ::g_57bec4 = 0x42C80000U;   // 0x0043A8E7 (100.0f as bits)
    ::g_57bee0 = 0x0U;          // 0x0043A8F5
    ::g_57bee4 = 0x42C80000U;   // 0x0043A8FF (100.0f as bits)
    ::g_57bed0 = 0x0U;          // 0x0043A909
    ::g_57bed4 = 0x41200000U;   // 0x0043A913 (10.0f as bits)
    ::g_57bee8 = 0x0U;          // 0x0043A91D
    ::g_57beec = 0x42700000U;   // 0x0043A927 (60.0f as bits)
    ::g_57bed8 = 0xBF800000U;   // 0x0043A931 (-1.0f as bits)
    ::g_57bedc = 0x461C4000U;   // 0x0043A93B (10000.0f as bits)
    ::g_57bec8 = 0x0U;          // 0x0043A945
    ::g_57becc = 0x3E8U;        // 0x0043A94F (1000)
    ::g_57beb8 = 0x0U;          // 0x0043A959
    ::g_57bebc = 0x64U;         // 0x0043A963 (100)
    ::g_57beb0 = 0xFFFFFFFFU;   // 0x0043A96D (-1)
    ::g_57beb4 = 0x1000U;       // 0x0043A977 (4096)

    // 0x43A8F1: push $0x0 (arg6); 0x43A981: call Sys_GetGlobal32_4F1010 (arg4); 0x43A997: call 0x004F1A80 (frame+0x00, frame+0x20).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // 0x43A99F: push $0x0 (arg6); 0x43A9A3: call Sys_GetGlobal32_4F0FC0 (arg4); 0x43A9B9: call 0x004F1A80 (frame+0x00, frame+0x38).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x38,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // 0x43A9C1: push $0x57bec0 (arg6); 0x43A9C8: call Sys_GetGlobal32_4F0F50 (arg4); 0x43A9E1: call 0x004F1A80 (frame+0x00, frame+0x88).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x88,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bec0)));

    // 0x43A9E9: push $0x57bee0 (arg6); 0x43A9F0: call Sys_GetGlobal32_4F0F50 (arg4); 0x43AA09: call 0x004F1A80 (frame+0x00, frame+0x8C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x8C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bee0)));

    // 0x43AA11: push $0x57bed0 (arg6); 0x43AA18: call Sys_GetGlobal32_4F0F50 (arg4); 0x43AA31: call 0x004F1A80 (frame+0x00, frame+0x90).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x90,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bed0)));

    // 0x43AA39: push $0x57bee8 (arg6); 0x43AA40: call Sys_GetGlobal32_4F0F50 (arg4); 0x43AA59: call 0x004F1A80 (frame+0x00, frame+0x94).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bee8)));

    // 0x43AA61: push $0x57bec8 (arg6); 0x43AA68: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43AA81: call 0x004F1A80 (frame+0x00, frame+0x98).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x98,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bec8)));

    // 0x43AA89: push $0x57beb8 (arg6); 0x43AA90: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43AAA9: call 0x004F1A80 (frame+0x00, frame+0x9C).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0x9C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57beb8)));

    // 0x43AAB1: push $0x0 (arg6); 0x43AAB5: call Core_GetRenderWordB_4F0FD0 (arg4); 0x43AACE: call 0x004F1A80 (frame+0x00, frame+0xA0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0xA0,
                                static_cast<std::int32_t>(Core_GetRenderWordB_4F0FD0()), 7, 0);

    // 0x43AAD6: push $0x57beb0 (arg6); 0x43AADD: call Core_GetRenderWordA_4F0FA0 (arg4); 0x43AAF6: call 0x004F1A80 (frame+0x00, frame+0xC0).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0xC0,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57beb0)));

    // 0x43AAFE: push $0x57bed8 (arg6); 0x43AB05: call Sys_GetGlobal32_4F0F50 (arg4); 0x43AB1E: call 0x004F1A80 (frame+0x00, frame+0xC4).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0xC4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, static_cast<std::int32_t>(reinterpret_cast<std::uintptr_t>(&::g_57bed8)));

    // 0x43AB26: push $0x0 (arg6); 0x43AB2A: call Camera_Get_4F1000 (arg4); 0x43AB43: call 0x004F1A80 (frame+0x00, frame+0xC8).
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_57bef4)),
                                append_frame, append_frame + 0xC8,
                                static_cast<std::int32_t>(Camera_Get_4F1000()), 7, 0);
} // end of Task_DefineMineField

} // extern "C"
