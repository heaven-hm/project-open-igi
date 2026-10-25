// Native: Task_DefinePatrolPathCommand (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x004533B0-0x00453366.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 183 bytes (0xB7), 1 block, 56 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - 1 block, 56 instructions, 0 edges, straight-line; the objdump
//   listing build/retail-dis.txt was read over the whole extent 0x004533B0-0x00453366 (the ret at 0x00453366 is followed by the nop pad
//   0x00453367-0x0045336F, so the extent is closed by the ret and not by a guess). `ps` at 0x0053E288 gives "PatrolPathCommand" byte-identical to
//   the literal below. The image bytes at 0x0053DFCC are 81 01 00 00, i.e. the unregistered-type sentinel 0x0181, so the port's word carries that
//   initialiser rather than zero. 0x005BDA68 lies beyond `.data`'s raw size (raw ends at 0x00552000), so its image value is the zero fill and the
//   port's initialiser is 0. A full push-by-push simulation closes exactly (below).
// Behavior/CFG: a 0x28-byte frame (`sub esp,0x28` at 0x004533B0, no saved-register pushes and no stack probe). It registers the "PatrolPathCommand"
//   task type of size 0x28 - equal to its own frame - with parent QTask_GetCurrentType() into the word at 0x0053DFCC, links THREE handlers through
//   0x00401400, allocates a TWO-record container through 0x004F1950 into 0x005BDA68, and appends two templates through 0x004F1A80 with the
//   recorded differences 0x20 and 0x24 (argument 2 is frame+0x00 at both, argument 4 is Core_GetRenderWordA_4F0FA0() at both, argument 5 is the
//   literal 7 at both and argument 6 is 0 at both). Links: 0x00453470 on the literal event 0, 0x00453480 on the event from 0x004F1A60, and
//   0x00453490 on the event from 0x004F1A70 - all three through 0x00401400, i.e. the context-handler link, in retail's order. Link 1 is passed the
//   id that 0x00401900 returned (the `push %eax` at 0x004533D0 reads it before the store to the global); links 2 and 3 re-load the id from
//   0x0053DFCC (`mov 0x53dfcc,%ax` at 0x004533EA, `mov 0x53dfcc,%cx` at 0x00453403). Straight-line: one block, no branch, no loop, 12 call sites
//   over 8 distinct callees. The body does NOT call 0x004B8810: that is the FIFTH instance of the skip pattern in this lane (after 0x00449360
//   Siren, 0x00449D30 GlassShatter, 0x0044C130 AmbientArea and 0x0044C8D0 HumanAI), and the 0x0044C8D0 finding showed how the names are
//   registered anyway - by a separate body the registrar calls. Nothing here calls such a body either, so this type's script name is registered by
//   a site outside this native (unestablished).
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 56-instruction listing; runtime UNOBSERVED. Census: 0x00401400 x3, 0x004018F0 x1,
//   0x00401900 x1, 0x004F0FA0 x2, 0x004F1950 x1, 0x004F1A60 x1, 0x004F1A70 x1, 0x004F1A80 x2 = 12 sites / 8 distinct.
//   Cleanup reconciliation, each `add esp` matched to the pushes it removes: `add esp,0x20` at 0x004533DC = the five registration pushes (two
//   literal zeros, the parent getter's result, the size and the name) plus link 1's three; `add esp,0xC` at 0x004533F6 = link 2's three;
//   `add esp,0x10` at 0x00453418 = link 3's three, which have no cleanup of their own, PLUS the container-capacity push; `add esp,0x18` at
//   0x00453440 = append 1's six; `add esp,0x40` at 0x00453463 = append 2's six outstanding pushes PLUS the whole 0x28 frame (ten dwords). Totals:
//   27 pushes = 108 bytes, 148 bytes cleaned = 108 + 40, and esp is back at its entry value at the ret.
//   Documented shape differences, all value-preserving: (1) retail pushes two zeros before the parent getter (the task-list argument and a fifth
//   dword 0x00401900 never reads - see task_types.hpp); the port's four-parameter signature emits one zero for the task-list argument, because the
//   fifth slot is pushed and cleaned by every retail call site and read by no callee; (2) 0x004F1A80 reads argument 2 and argument 3 ONLY as their
//   difference (`sub ecx,edx` at 0x004F1A88) and never dereferences either, so retail's 0x28-byte frame is reproduced as one addressable dword
//   whose address stands in for the frame base plus the retail offsets 0x20 and 0x24 - no byte of the frame is read by the callee, so nothing is
//   lost; (3) the argument evaluation order of the append calls is the compiler's, not retail's, and every argument is a pure value (a constant, a
//   frame offset or the result of an argument-less getter), so no observable order changes.
//   Handlers 0x00453470, 0x00453480 and 0x00453490 are raw code immediates in retail and are not transcribed yet (native gaps inside
//   0x00430000-0x0045FFFF, this lane's band). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {

// 0x004F0FA0 (core/native_4f0fa0.cpp): argument-less dword getter, argument 4 of both appends.
extern std::uint32_t Core_GetRenderWordA_4F0FA0();

// 0x004F1950 (script/native_4f1950.cpp): allocates the task-parameter container.
void* Script_TaskContainer_Alloc(std::int32_t count);

// 0x004F1A80 (script/native_4f1a80.cpp): reads argument 2 and argument 3 only as `arg3 - arg2` and stores that difference in the record's first
// field; argument 5 is read as a byte (record +0x0C) and argument 6 as a dword (record +0x08).
extern "C" void* Script_TaskContainer_Append(std::int32_t* const container, std::int32_t const arg2, std::int32_t const arg3,
                                             std::int32_t const arg4, std::int32_t const arg5, std::int32_t const arg6);

// 0x0053DFCC: the "PatrolPathCommand" task type id (word). Image bytes 81 01 00 00 = 0x0181, the sentinel that means "no type registered", so the
// initialiser is the image value and not zero. Writable `.data`: `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000, and this image has
// no `.bss`, so a global here is `.data` and `B`/`D` is the symbol classification rather than the section. No other file in the tree owns it.
std::uint16_t g_53dfcc = 0x0181U;

// 0x005BDA68: the "PatrolPathCommand" parameter container, two records (capacity 2, the literal pushed at 0x00453411). Its image bytes are the
// zero fill beyond `.data`'s raw size, so the initialiser is 0. No other file in the tree owns it.
std::uint32_t g_5bda68 = 0U;

void Task_DefinePatrolPathCommand()
{
    // 0x004533B0: sub esp,0x28 - a plain frame: no saved-register pushes and no stack probe, so the frame-relative shift is ZERO.
    std::int32_t append_frame_base = 0; // retail frame slot at +0x00 - argument 2 of both appends in this body
    const std::int32_t append_frame = reinterpret_cast<std::int32_t>(&append_frame_base);

    // 0x004533B3: push 0; 0x004533B5: push 0 - the task-list argument and a fifth dword the callee never reads (see the provenance block);
    // 0x004533B7: call 0x004018F0 (parent type); 0x004533BC: push %eax; 0x004533BD: push 0x28; 0x004533BF: push 0x0053E288;
    // 0x004533C4: call 0x00401900.
    const std::uint16_t patrol_path_command_id = TaskType_Register("PatrolPathCommand",            // 0x004533BF: push 0x0053E288
                                                                  0x28U,                          // 0x004533BD: push 0x28 (object size, equal to the frame)
                                                                  QTask_GetCurrentType(),         // 0x004533B7: call 0x004018F0 (parent type)
                                                                  0U);                            // 0x004533B5: push 0 (task-list argument)

    // 0x004533C9: push 0x00453470 (handler, deferred argument 3); 0x004533CE: push 0x0 (argument 2, literal event id);
    // 0x004533D0: push %eax (argument 1, still the id 0x00401900 returned); 0x004533D1: mov word [0x0053DFCC],ax. The store executes BEFORE the
    // link call in retail and is kept before it here, so 0x00401400 is reached with the global already written.
    ::g_53dfcc = patrol_path_command_id; // 0x004533D1: mov word [0x53DFCC],ax
    // 0x004533D7: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(patrol_path_command_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453470U)));

    // 0x004533DF: push 0x00453480 (handler, deferred argument 3); 0x004533E4: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x004533E9: push %eax; 0x004533EA: mov 0x53DFCC into a register (argument 1, the id RELOADED from the global);
    // 0x004533F0: push %eax; 0x004533F1: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53dfcc), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453480U)));

    // 0x004533F9: push 0x00453490 (handler, deferred argument 3); 0x004533FE: call 0x004F1A70 (supplies argument 2, the event id);
    // 0x00453403: mov 0x53DFCC into a register (argument 1, the id RELOADED from the global); 0x0045340A: push %eax; 0x0045340B: push %ecx;
    // 0x0045340C: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53dfcc), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453490U)));

    // 0x00453411: push 0x2 (container CAPACITY, two records - the same literal as the number of appends); 0x00453413: call 0x004F1950;
    // 0x00453418: add esp,0x10; 0x0045341B: mov [0x5BDA68],eax.
    ::g_5bda68 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(Script_TaskContainer_Alloc(2)));

    // Append 1: 0x00453420: push 0 (argument 6); 0x00453422: push 0x7 (argument 5); 0x00453424: call 0x004F0FA0 (argument 4);
    // 0x00453429: mov 0x5BDA68 into a register (argument 1); 0x0045342F: push %eax; 0x00453430: lea 0x2c(%esp) (argument 3, frame+0x20);
    // 0x00453434: lea 0xc(%esp) (argument 2, frame+0x00); 0x00453438/0x00453439: push both; 0x0045343A: push %ecx; 0x0045343B: call 0x004F1A80;
    // 0x00453440: add esp,0x18. The arg-3 `lea` executes before the arg-4 `push %eax` in retail, so the recorded difference is measured with
    // three pushes outstanding and the two displacements differ by that amount.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5bda68),
                                append_frame,
                                append_frame + 0x20,                          // 0x00453430: frame+0x20 (recorded difference 0x20)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, 0);

    // Append 2: 0x00453443: push 0 (argument 6); 0x00453445: push 0x7 (argument 5); 0x00453447: call 0x004F0FA0 (argument 4);
    // 0x0045344C: mov 0x5BDA68 into a register (argument 1); 0x00453452: push %eax; 0x00453453: lea 0x30(%esp) (argument 3, frame+0x24);
    // 0x00453457: lea 0xc(%esp) (argument 2, frame+0x00); 0x0045345B/0x0045345C: push both; 0x0045345D: push %ecx; 0x0045345E: call 0x004F1A80.
    // This append has no cleanup of its own: 0x00453463's `add esp,0x40` covers its six pushes and the frame, and the ret follows at 0x00453466.
    Script_TaskContainer_Append(reinterpret_cast<std::int32_t*>(::g_5bda68),
                                append_frame,
                                append_frame + 0x24,                          // 0x00453453: frame+0x24 (recorded difference 0x24)
                                static_cast<std::int32_t>(Core_GetRenderWordA_4F0FA0()),
                                7, 0);
} // end of Task_DefinePatrolPathCommand

} // extern "C"
