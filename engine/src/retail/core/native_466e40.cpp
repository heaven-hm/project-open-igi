// Native: Task_DefineComputerLimit (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00466E40-0x00466F0A.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 203 bytes, 1 block, 61 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 61 instructions / 203 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x00466E40-0x00466F0A; the ret is at 0x00466F0A and five nops follow);
//   `ps` at 0x0053F550 for the name literal; `px 8 @ 0x005BDC60` for the id slot (all zero); a full stack-pointer simulation over all 61
//   instructions, which returns esp to its entry value at the ret and resolves both `lea reg,[esp+N]` pairs - at 0x00466ED4/0x00466ED8 esp has
//   already dropped 0xC, so 0x2C and 0xC are frame+0x20 and frame+0x00, and at 0x00466EF7/0x00466EFB the same 0xC drop makes 0x44 and 0xC
//   frame+0x38 and frame+0x00. The cleanups are 0x2C (eleven pushes: the two zeros, the parent-getter result, the size, the name and the three
//   pushes of each of the first two links), 0xC twice, 0x10 (the second 0x004F1A70 link's three pushes plus the container count) and 0x58, which
//   is the 0x40 frame PLUS the last append's six pushes (0x18) - a combined frame-and-arguments cleanup, the same shape as 0x004682F0's 0x4FC.
//   Every callee was re-read at its own VA, and all six are already implemented in this tree, so this file declares those symbols rather than
//   duplicating them. Argument numbers were derived by counting the pushes above each access.
// Behavior/CFG: one straight-line body with a 0x40-byte frame, which is also the registered object size. It registers the "DefineComputerLimit"
//   task type of size 0x40 with parent Sys_GetGlobal16_4C48C0() -> id at 0x005BDC60, links e0->0x00466F10 through 0x00401400 and event 2
//   (event 1 skipped, as in 0x004682F0)->0x00466F20 through 0x00401530, links the event ids from 0x004F1A60->0x00466F30 and
//   0x004F1A70->0x004D3710 through 0x00401400, allocates a two-entry parameter container with Script_TaskContainer_Alloc(2) into 0x005BDC5C, and
//   appends exactly TWO records with Script_TaskContainer_Append: (container, frame+0x00, frame+0x20, Sys_GetGlobal32_4F1010(), 7, 0) and
//   (container, frame+0x00, frame+0x38, Sys_GetGlobal32_4F0F50(), 7, 0). One basic block, no branch, no loop, 13 call sites over 10 distinct
//   callees. It allocates no event id through QTask_AllocateEvent and has no 0x004B8810 script-name tail.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BDC60 holds image value 0, so it is zero-initialised data (`B`), unlike the
//   0x0053F5xx id slots in the literal region; its accesses are 16-bit (0x00466E61 store, 0x00466E6C/0x00466E8C/0x00466EA7 reloads). The parent
//   Sys_GetGlobal16_4C48C0 loads `[0x547F42]`, which holds 385 = 0x0181 in the image, so this registrar is another sighting of the 0x0181 cluster
//   on the parent side. **0x005BDC5C holds image value 0 and is the two-entry parameter container**: it receives Script_TaskContainer_Alloc(2)'s
//   return at 0x00466EBF and is reloaded as the first argument of both appends. The two record deltas are 0x20 and 0x38 and their type argument
//   differs (0x004F1010 then 0x004F0F50), while arg5 is 7 and arg6 is 0 in both. **0x004F0F50 is now established rather than unknown**: its body
//   is `mov eax,[0xA71870]; ret`, an argument-less dword getter that ignores the `push 0` its caller places under the `push 7` - the same inert
//   argument shape as 0x004F1010's. **0x004D3710 has 163 references image-wide and is the most-shared handler seen in this lane** (0x004064B1,
//   0x004064DC, 0x00406542, 0x00406555, 0x004072D4 and others), and it lies above 0x0048FFFF, so it belongs to another lane. Handlers
//   0x00466F10, 0x00466F20 and 0x00466F30 have one reference each and are raw code immediates in retail, not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx). Documented shape differences, all value-preserving: (1) the getters are argument-less loads in retail that ignore the
//   arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert pushes are not reproduced; each is
//   simultaneously the next link call's pre-pushed handler, which the port keeps as the handler value in the same order; (2) byte-valued event ids
//   are zero-extended in the port, which cannot change behaviour because every link here goes through 0x00401400 or 0x00401530, which mask the
//   event word for the phase index; (3) retail's 0x40-byte frame is reproduced as a single local slot whose address stands in for the frame base,
//   the same convention the accepted 0x004682F0, 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0 files use; (4) the
//   deferred cleanup is reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic, since the port never
//   pushes the inert arguments; the retail sizes 0x2C, 0xC, 0x10 and 0x58 are recorded here so a future reviewer can check the arithmetic. Domain:
//   core/, because it is one of the 0x00402930 startup registrars (call position 100 of 155).
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint16_t Sys_GetGlobal16_4C48C0(void);            // 0x004C48C0: `mov ax,[0x547F42]; ret` (parent type; image 385 = 0x0181)
extern "C" std::uint8_t Script_PhaseIdGetE4_4F1A60(void);         // 0x004F1A60: argument-less byte getter (script_state.hpp)
extern "C" std::uint8_t Script_PhaseIdGetE5_4F1A70(void);         // 0x004F1A70: argument-less byte getter (script_state.hpp)
extern "C" void* Script_TaskContainer_Alloc(std::int32_t count);  // 0x004F1950 (script/native_4f1950.cpp)
extern "C" std::uint32_t Sys_GetGlobal32_4F1010(void);            // 0x004F1010: `mov eax,[0xA70C88]; ret`
extern "C" std::uint32_t Sys_GetGlobal32_4F0F50(void);            // 0x004F0F50: `mov eax,[0xA71870]; ret` (ignores the pushed 0)
// 0x004F1A80 (script/native_4f1a80.cpp). SIX arguments: arg5 is read as a byte (record +0x0C) and arg6 as a dword (record +0x08); the two
// reads are both `[esp+0x20]` in retail at different push depths with a `pop edi` between them.
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2,
                                             std::int32_t const arg3, std::int32_t const arg4,
                                             std::int32_t const arg5, std::int32_t const arg6);

extern "C" {

// 0x005BDC60: the "DefineComputerLimit" task type id. Image value 0, so it is zero-initialised data (`B`); all four accesses are 16-bit.
std::uint16_t g_5bdc60 = 0U;

// 0x005BDC5C: the two-entry parameter container. Image value 0, written and read as a dword.
std::uint32_t g_5bdc5c = 0U;

void Task_DefineComputerLimit()
{
    // 0x00466E40: sub esp,0x40 - retail's 64-byte frame, which is the registered object size. The two 0x004F1A80 calls below pass frame+0x00
    // and frame+0x20 / +0x38; the callee stores the differences. A single local slot stands in for the frame base, which is the convention the
    // accepted 0x004682F0, 0x004813E0, 0x00470FA0, 0x0046DB40, 0x0046D420, 0x0046CB70 and 0x0046C0C0 files use.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x00466E43/0x00466E45: push 0; push 0 (the parent getter's inert argument, then TaskType_Register's fourth argument);
    // 0x00466E47: call 0x4C48C0; 0x00466E4C: push eax; 0x00466E4D: push 0x40; 0x00466E4F: push "DefineComputerLimit"; 0x00466E54: call 0x401900.
    const std::uint16_t computer_limit_id = TaskType_Register("DefineComputerLimit", // 0x00466E4F: push 0x0053F550
                                                             0x40U,                  // 0x00466E4D: push 0x40 (object size, equal to the frame)
                                                             Sys_GetGlobal16_4C48C0(), // 0x00466E47: call 0x4C48C0 (parent type)
                                                             0U);                    // 0x00466E45: push 0 (fourth argument)

    // 0x00466E59: push 0x00466F10 (pre-pushed handler); 0x00466E5E: push 0; 0x00466E60: push eax; 0x00466E61: mov word [0x5BDC60],ax (the id
    // store lands between the pushes); 0x00466E67: call 0x401400.
    ::g_5bdc60 = computer_limit_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc60), // 0x00466E61: mov ax,[0x5BDC60]
                                0U,                                     // 0x00466E5E: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00466F10U))); // 0x00466E59: push handler

    // 0x00466E6C: mov ax,[0x5BDC60]; 0x00466E72: push 0x00466F20; 0x00466E77: push 2; 0x00466E79: push eax; 0x00466E7A: call 0x401530;
    // 0x00466E7F: add esp,0x2C - eleven pushes: the two zeros, the parent-getter result, the size, the name and the three pushes of each of the
    // first two links. Event 1 is skipped, as in 0x004682F0.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdc60), // 0x00466E6C: mov ax,[0x5BDC60]
                                           2U,                                     // 0x00466E77: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00466F20U))); // 0x00466E72: push handler

    // 0x00466E82: push 0x00466F30; 0x00466E87: call 0x4F1A60; 0x00466E8C: mov cx,[0x5BDC60]; 0x00466E93: push eax; 0x00466E94: push ecx;
    // 0x00466E95: call 0x401400; 0x00466E9A: add esp,0xC.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc60), // 0x00466E8C: mov cx,[0x5BDC60]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()), // 0x00466E93: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00466F30U))); // 0x00466E82: push handler

    // 0x00466E9D: push 0x004D3710 (pre-pushed handler, and the inert argument of the byte getter); 0x00466EA2: call 0x4F1A70;
    // 0x00466EA7: mov dx,[0x5BDC60]; 0x00466EAE: push eax; 0x00466EAF: push edx; 0x00466EB0: call 0x401400. 0x004D3710 has 163 references
    // image-wide and lies above 0x0048FFFF. This link has no cleanup of its own: the next add esp,0x10 covers it together with the container
    // argument.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdc60), // 0x00466EA7: mov dx,[0x5BDC60]
                                static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()), // 0x00466EAE: push eax (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x004D3710U))); // 0x00466E9D: push handler

    // 0x00466EB5: push 2 (Script_TaskContainer_Alloc's argument); 0x00466EB7: call 0x4F1950; 0x00466EBC: add esp,0x10 - the link's three
    // pushes plus this argument; 0x00466EBF: mov [0x5BDC5C],eax.
    ::g_5bdc5c = reinterpret_cast<std::uint32_t>(Script_TaskContainer_Alloc(2)); // 0x00466EB5: push 2

    // Append 1: 0x00466EC4/0x00466EC6: push 0/7; 0x00466EC8: call 0x4F1010; 0x00466ED3: push eax (the record type); 0x00466ED4: lea eax,[esp+0x2C]
    // -> frame+0x20; 0x00466ED8: lea ecx,[esp+0xC] -> frame+0x00; 0x00466EDC: push eax; 0x00466EDD: push ecx; 0x00466EDE: push edx (the
    // container, reloaded at 0x00466ECD); 0x00466EDF: call 0x4F1A80; 0x00466EE4: add esp,0x18.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc5c)),
                                append_frame, append_frame + 0x20,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F1010()), 7, 0);

    // Append 2: 0x00466EE7/0x00466EE9: push 0/7; 0x00466EEB: call 0x4F0F50; 0x00466EF6: push eax (the record type); 0x00466EF7: lea eax,[esp+0x44]
    // -> frame+0x38; 0x00466EFB: lea ecx,[esp+0xC] -> frame+0x00; 0x00466EFF: push eax; 0x00466F00: push ecx; 0x00466F01: push edx (the
    // container, reloaded at 0x00466EF0); 0x00466F02: call 0x4F1A80; 0x00466F07: add esp,0x58 - the 0x40 frame plus this append's six pushes;
    // 0x00466F0A: ret.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(static_cast<std::uintptr_t>(::g_5bdc5c)),
                                append_frame, append_frame + 0x38,
                                static_cast<std::int32_t>(Sys_GetGlobal32_4F0F50()), 7, 0);
} // end of Task_DefineComputerLimit

} // extern "C"
