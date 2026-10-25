// Native: Task_DefineComputerObjective (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00466F70-0x004673A7.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 1080 bytes, 1 block, 342 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 342 instructions / 1080 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00466F70-0x004673A7; the ret is at 0x004673A7 and eight nops follow, and
//   0x004673B0 is the next function, whose address this body uses as its first handler); `ps` at 0x0053F564 for the name literal; `px 8 @
//   0x005BDC70` and `px 8 @ 0x005BDC6C` for the two globals (all zero); a full stack-pointer simulation over all 342 instructions, which returns
//   esp to its entry value at the ret, verifies that esp is exactly 0x18 below the frame base at every one of the 25 0x004F1A80 call sites, and
//   verifies that the frame+0x00 operand of all 25 appends resolves to 0x00. The 25 deltas were derived mechanically from the dump rather than by
//   hand: each `lea reg,[esp+N]` was resolved against the simulated esp at that instruction. The cleanups are 0x38 (fourteen pushes: the two
//   zeros, the parent-getter result, the size, the name and the three pushes of each of the first three links), 0xC, 0x10 (the second phase-id
//   link's three pushes plus the container count), 0x18 twenty-five times, and 0x550 = the 0x538 frame PLUS the last append's six pushes (0x18),
//   a combined frame-and-arguments cleanup like 0x004682F0's 0x4FC and 0x00466E40's 0x58. Every callee was re-read at its own VA, and all eight
//   are already implemented in this tree, so this file declares those symbols rather than duplicating them. Argument numbers were derived by
//   counting the pushes above each access.
// Behavior/CFG: one straight-line body with a 0x538-byte frame, which is also the registered object size. It registers the
//   "DefineComputerObjective" task type of size 0x538 with parent Sys_GetGlobal16_4C48C0() -> id at 0x005BDC70, links e0->0x004673B0 through
//   0x00401400, e1->0x00467410 and e2->0x004674D0 through 0x00401530, then links the event ids from 0x004F1A60->0x00467520 and
//   0x004F1A70->0x00467530 through 0x00401400, allocates a TWENTY-FIVE-entry parameter container with Script_TaskContainer_Alloc(0x19) into
//   0x005BDC6C, and appends exactly 25 records with Script_TaskContainer_Append. One basic block, no branch, no loop, 60 call sites over 11
//   distinct callees. It allocates no event id through QTask_AllocateEvent and has no 0x004B8810 script-name tail.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The container size 0x19 = 25 is exactly the number of appends, so no entry is left
//   uninitialised by this body. The id slot 0x005BDC70 holds image value 0, so it is zero-initialised data (`B`), and its accesses are 16-bit
//   (0x00466F97 store, 0x00466FA2/0x00466FB5/0x00466FD6/0x00466FF2 reloads). The parent Sys_GetGlobal16_4C48C0 loads `[0x547F42]`, which holds
//   385 = 0x0181 in the image - another 0x0181-cluster sighting on the parent side. **The 25 record deltas are not arbitrary; they fall into two
//   interleaved arithmetic progressions of stride 0xCC.** The seven single appends use deltas 0x20, 0xE4, 0x1B0, 0x27C, 0x348, 0x414, 0x4E0
//   (each 0xCC from the next) and the six groups of three use deltas 0x70/0x90/0x94, 0x13C/0x15C/0x160, 0x208/0x228/0x22C, 0x2D4/0x2F4/0x2F8,
//   0x3A0/0x3C0/0x3C4 and 0x46C/0x48C/0x490, whose first elements are also 0xCC apart and whose members are always +0x00/+0x20/+0x24 relative to
//   each other. The 0xCC stride is recorded here as a HYPOTHESIS about a record layout, not as a decoded layout, and it has since been
//   falsification-tested once from the registration side: the whole retail image contains ZERO occurrences of `push $0xCC`, so the stride is not
//   a task-type registration size, a container count or an allocation argument anywhere in the image. The test therefore neither confirms nor
//   contradicts the stride - it only removes that family of explanations, and the layout question stays open. The type argument follows the same split exactly: **every single append takes Sys_GetGlobal32_4F0FC0 and every group of three
//   takes Sys_GetGlobal32_4F0FE0, Core_GetRenderWordA_4F0FA0, Sys_GetGlobal32_4F0FC0 in that order**, which is why the image's per-target counts
//   are 13 / 6 / 6 for those three getters. Arg5 is 7 and arg6 is 0 in all 25 appends. **0x004F0FA0 is implemented in this tree under a
//   different name than its siblings**: it is `Core_GetRenderWordA_4F0FA0` (core/native_4f0fa0.cpp), not `Sys_GetGlobal32_4F0FA0`, while
//   0x004F0FC0 and 0x004F0FE0 are `Sys_GetGlobal32_*`; declaring a `Sys_GetGlobal32_4F0FA0` would have been an undefined symbol, and the
//   definition sweep is what caught it. All three bodies are argument-less dword getters. Handlers 0x004673B0, 0x00467410, 0x004674D0,
//   0x00467520 and 0x00467530 have one reference each and are raw code immediates in retail, not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx). Documented shape differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore
//   the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event
//   ids are zero-extended in the port, which cannot change behaviour because every link here goes through 0x00401400 or 0x00401530, which mask
//   the event word for the phase index; (3) retail's 0x538-byte frame is reproduced as a single local slot whose address stands in for the frame
//   base, the same convention the accepted 0x004682F0, 0x00466E40, 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0
//   files use; (4) the deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic,
//   since the port never pushes the inert arguments; the retail sizes 0x38, 0xC, 0x10, 0x18 and 0x550 are recorded here so a future reviewer can
//   check the arithmetic. Domain: core/, because it is one of the 0x00402930 startup registrars (call position 101 of 155).
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: `mov ax,[0x547F42]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint32_t Core_GetRenderWordA_4F0FA0(void);        // 0x004F0FA0: `mov eax,[0xA70C7C]; ret` (core/native_4f0fa0.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F0FC0(void);            // 0x004F0FC0: `mov eax,[0xA70C9C]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0FE0(void);            // 0x004F0FE0: `mov eax,[0xA71888]; ret`
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BDC70: the "DefineComputerObjective" task type id. Image value 0, so it is zero-initialised data (`B`); all five accesses are 16-bit.
std::uint16_t g_5bdc70 = 0U;

// 0x005BDC6C: the twenty-five-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5bdc6c = 0U;

void Task_DefineComputerObjective()
{
    // 0x00466F70: sub esp,0x538 - retail's 1336-byte frame, which is the registered object size. The 25 0x004F1A80 calls below pass frame+0x00
    // and the 25 deltas listed in the provenance block; the callee stores the differences. A single local slot stands in for the frame base, which
    // is the convention the accepted 0x004682F0, 0x00466E40, 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0 files
    // use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00466F76/0x00466F78: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00466F7A: call 0x4C48C0; 0x00466F7F: push eax; 0x00466F80: push 0x538; 0x00466F85: push "DefineComputerObjective";
    // 0x00466F8A: call 0x401900.
    const std::uint16_t computer_objective_id = TaskType_Register("DefineComputerObjective", // 0x00466F85: push 0x0053F564
                                                                 0x538U,                     // 0x00466F80: push 0x538 (object size, equal to the frame)
                                                                 Sys_GetGlobal16_4C48C0(),   // 0x00466F7A: call 0x4C48C0 (parent type)
                                                                 0U);                       // 0x00466F78: push 0 (fourth argument)

    // 0x00466F8F: push 0x004673B0 (pre-pushed handler); 0x00466F94: push 0; 0x00466F96: push eax; 0x00466F97: mov word [0x5BDC70],ax (the id
    // store lands between the pushes); 0x00466F9D: call 0x401400.
    ::g_5bdc70 = computer_objective_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc70), // 0x00466F97: mov ax,[0x5BDC70]
                                0U,                                     // 0x00466F94: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004673B0U))); // 0x00466F8F: push handler

    // 0x00466FA2: mov ax,[0x5BDC70]; 0x00466FA8: push 0x00467410; 0x00466FAD: push 1; 0x00466FAF: push eax; 0x00466FB0: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc70), // 0x00466FA2: mov ax,[0x5BDC70]
                                           1U,                                     // 0x00466FAD: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00467410U))); // 0x00466FA8: push handler

    // 0x00466FB5: mov cx,[0x5BDC70]; 0x00466FBC: push 0x004674D0; 0x00466FC1: push 2; 0x00466FC3: push ecx; 0x00466FC4: call 0x401530;
    // 0x00466FC9: add esp,0x38 - fourteen pushes: the two zeros, the parent-getter result, the size, the name and the three pushes of each of the
    // first three links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc70), // 0x00466FB5: mov cx,[0x5BDC70]
                                           2U,                                     // 0x00466FC1: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x004674D0U))); // 0x00466FBC: push handler

    // 0x00466FCC: push 0x00467520; 0x00466FD1: call 0x4F1A60; 0x00466FD6: mov dx,[0x5BDC70]; 0x00466FDD: push eax; 0x00466FDE: push edx;
    // 0x00466FDF: call 0x401400; 0x00466FE4: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc70), // 0x00466FD6: mov dx,[0x5BDC70]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00466FDD: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00467520U))); // 0x00466FCC: push handler

    // 0x00466FE7: push 0x00467530 (pre-pushed handler, and the inert argument of the byte getter); 0x00466FEC: call 0x4F1A70;
    // 0x00466FF1: push eax; 0x00466FF2: mov ax,[0x5BDC70]; 0x00466FF8: push eax; 0x00466FF9: call 0x401400. This link has no cleanup of its
    // own: the next add esp,0x10 covers it together with the container argument.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc70), // 0x00466FF2: mov ax,[0x5BDC70]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00466FF1: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00467530U))); // 0x00466FE7: push handler

    // 0x00466FFE: push 0x19 (25 - Script_TaskContainer_Alloc's argument, and exactly the number of appends below);
    // 0x00467000: call 0x4F1950; 0x00467005: add esp,0x10 - the phase-id link's three pushes plus this argument;
    // 0x00467008: mov [0x5BDC6C],eax.
    ::g_5bdc6c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(0x19)); // 0x00466FFE: push 0x19

    // Append 1: 0x46700D: push 0 (arg6); 0x467011: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467027: call 0x4F1A80 with frame+0x00
    // and frame+0x20; 0x46702C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 2: 0x46702F: push 0 (arg6); 0x467033: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x467049: call 0x4F1A80 with frame+0x00
    // and frame+0x70; 0x46704E: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x70,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 3: 0x467051: push 0 (arg6); 0x467055: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x46706E: call 0x4F1A80 with frame+0x00
    // and frame+0x90; 0x467073: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x90,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 4: 0x467076: push 0 (arg6); 0x46707A: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467093: call 0x4F1A80 with frame+0x00
    // and frame+0x94; 0x467098: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x94,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 5: 0x46709B: push 0 (arg6); 0x46709F: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x4670B8: call 0x4F1A80 with frame+0x00
    // and frame+0xE4; 0x4670BD: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0xE4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 6: 0x4670C0: push 0 (arg6); 0x4670C4: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x4670DD: call 0x4F1A80 with frame+0x00
    // and frame+0x13C; 0x4670E2: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x13C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 7: 0x4670E5: push 0 (arg6); 0x4670E9: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x467102: call 0x4F1A80 with frame+0x00
    // and frame+0x15C; 0x467107: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x15C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 8: 0x46710A: push 0 (arg6); 0x46710E: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467127: call 0x4F1A80 with frame+0x00
    // and frame+0x160; 0x46712C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x160,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 9: 0x46712F: push 0 (arg6); 0x467133: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x46714C: call 0x4F1A80 with frame+0x00
    // and frame+0x1B0; 0x467151: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x1B0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 10: 0x467154: push 0 (arg6); 0x467158: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x467171: call 0x4F1A80 with frame+0x00
    // and frame+0x208; 0x467176: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x208,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 11: 0x467179: push 0 (arg6); 0x46717D: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x467196: call 0x4F1A80 with frame+0x00
    // and frame+0x228; 0x46719B: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x228,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 12: 0x46719E: push 0 (arg6); 0x4671A2: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x4671BB: call 0x4F1A80 with frame+0x00
    // and frame+0x22C; 0x4671C0: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x22C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 13: 0x4671C3: push 0 (arg6); 0x4671C7: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x4671E0: call 0x4F1A80 with frame+0x00
    // and frame+0x27C; 0x4671E5: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x27C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 14: 0x4671E8: push 0 (arg6); 0x4671EC: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x467205: call 0x4F1A80 with frame+0x00
    // and frame+0x2D4; 0x46720A: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x2D4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 15: 0x46720D: push 0 (arg6); 0x467211: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x46722A: call 0x4F1A80 with frame+0x00
    // and frame+0x2F4; 0x46722F: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x2F4,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 16: 0x467232: push 0 (arg6); 0x467236: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x46724F: call 0x4F1A80 with frame+0x00
    // and frame+0x2F8; 0x467254: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x2F8,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 17: 0x467257: push 0 (arg6); 0x46725B: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467274: call 0x4F1A80 with frame+0x00
    // and frame+0x348; 0x467279: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x348,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 18: 0x46727C: push 0 (arg6); 0x467280: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x467299: call 0x4F1A80 with frame+0x00
    // and frame+0x3A0; 0x46729E: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x3A0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 19: 0x4672A1: push 0 (arg6); 0x4672A5: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x4672BE: call 0x4F1A80 with frame+0x00
    // and frame+0x3C0; 0x4672C3: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x3C0,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 20: 0x4672C6: push 0 (arg6); 0x4672CA: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x4672E3: call 0x4F1A80 with frame+0x00
    // and frame+0x3C4; 0x4672E8: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x3C4,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 21: 0x4672EB: push 0 (arg6); 0x4672EF: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467308: call 0x4F1A80 with frame+0x00
    // and frame+0x414; 0x46730D: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x414,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 22: 0x467310: push 0 (arg6); 0x467314: call Sys_GetGlobal32_4F0FE0 (the record type, arg4); 0x46732D: call 0x4F1A80 with frame+0x00
    // and frame+0x46C; 0x467332: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x46C,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FE0()), 7, 0);

    // Append 23: 0x467335: push 0 (arg6); 0x467339: call Core_GetRenderWordA_4F0FA0 (the record type, arg4); 0x467352: call 0x4F1A80 with frame+0x00
    // and frame+0x48C; 0x467357: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x48C,
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()), 7, 0);

    // Append 24: 0x46735A: push 0 (arg6); 0x46735E: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x467377: call 0x4F1A80 with frame+0x00
    // and frame+0x490; 0x46737C: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x490,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

    // Append 25: 0x46737F: push 0 (arg6); 0x467383: call Sys_GetGlobal32_4F0FC0 (the record type, arg4); 0x46739C: call 0x4F1A80 with frame+0x00
    // and frame+0x4E0; 0x4673A1: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc6c)),
                                append_frame, append_frame + 0x4E0,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0FC0()), 7, 0);

} // end of Task_DefineComputerObjective

} // extern "C"
