// Native: Task_DefinePatrolPath (behavior-derived placeholder name from the byte-identical name literal this body registers; retail symbol unknown); retail VA 0x00453550-0x00453626.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 215 bytes (0xD7), 4 blocks, 4 edges, 68
//   listing instructions of which 14 are the alignment nop pad 0x004535F2-0x004535FF between the prologue and the loop label (54 real).
// Tools/evidence: radare2 6.2.0 af/afbj with bin.relocs.apply over the exact image - block 0x00453550 (162 B, 37 instrs) -> 0x00453600, block
//   0x00453600 (9 B, 4 instrs) -> 0x00453609, block 0x00453609 (27 B, 10 instrs) with the taken edge back to 0x00453609 and the fall-through to
//   0x00453624, block 0x00453624 (3 B, 3 instrs); the objdump listing build/retail-dis.txt was read over the whole extent, the `ret` at 0x00453626
//   closes it and the next function starts at 0x00453630. Image reads: 0x0053E29C holds 81 01 (the 0x0181 unregistered sentinel); the name literal
//   0x0053E3D0 holds 50 61 74 72 6f 6c 50 61 74 68 00 00 = "PatrolPath"; the 10-entry table at 0x0053E2A0 holds the pointers 0x0053E3B8, 0x0053E39C,
//   0x0053E384, 0x0053E368, 0x0053E350, 0x0053E334, 0x0053E314, 0x0053E2FC, 0x0053E2E4, 0x0053E2C8, each verified by reading the string it points
//   at (the ten "PATROLPATHCOMMAND_*" literals below, byte-identical); 0x005BDA70-0x005BDA7C lie past `.data`'s raw size, so their image value is
//   the zero fill and the port's initialisers are 0.
// Behavior/CFG: no frame at all (there is no `sub esp`; the only stack traffic is push/pop and the calls) and no stack probe, so the registered
//   object size 0x34 does not correspond to a frame here. In retail order: it seeds a three-dword list header at 0x005BDA70 (0x005BDA70 = 0x005BDA74,
//   0x005BDA78 = 0x005BDA70, 0x005BDA74 = 0) with the two zero arguments of the registration call pushed before the parent getter; registers the
//   "PatrolPath" type of size 0x34 with parent QTask_GetCurrentType() into the word at 0x0053E29C; links FOUR handlers - 0x00453630 on the literal
//   event 0 and 0x00453650 on the literal event 2 through 0x00401400 and 0x00401530 respectively, then 0x00453670 on the event from 0x004F1A60 and
//   0x00453680 on the event from 0x004F1A70, both through 0x00401400; clears the dword at 0x005BDA7C; and finally walks the ten-entry table at
//   0x0053E2A0 with a do-while loop whose test is at the bottom (`cmp $0x53E2C8,%esi; jl`), registering each entry through 0x004B8810 with the value
//   `index - 1`, i.e. -1 for the first entry and 0..8 for the rest. The loop bound 0x0053E2C8 is the table base plus 0x28, ten four-byte entries, so
//   the extent is measured rather than assumed.
//   THIS IS THE BODY THAT REGISTERS SCRIPT NAMES for the PatrolPath family: 0x004B8810 is reached here ten times in a loop, which is the same
//   registration call the 0x0044C8D0 provenance identified for the AIEvent/AIAction tables. The five registrars that do not call it (0x00449360,
//   0x00449D30, 0x0044C130, 0x0044C8D0, 0x004533B0) are therefore not evidence of a missing registration in themselves.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 68-instruction listing and the 4-block CFG; runtime UNOBSERVED. Census: 0x00401400
//   x3, 0x00401530 x1, 0x004018F0 x1, 0x00401900 x1, 0x004B8810 x1 (ten executions in the loop), 0x004F1A60 x1, 0x004F1A70 x1 = 9 sites / 7 distinct.
//   Cleanup reconciliation: `add esp,0x2c` at 0x004535AA = 11 dwords = the five registration pushes plus link 1's three plus link 2's three;
//   `add esp,0xc` at 0x004535C5 = link 3's three; `add esp,0xc` at 0x004535E0 = link 4's three; `add esp,0x8` at 0x00453615 = the loop call's two; and
//   `pop %edi`/`pop %esi` at 0x00453624/0x00453625 = the loop's two register saves. Total 21 pushes = 84 bytes, 21 popped or added = 84 bytes, esp
//   is back at its entry value at the `ret`.
//   Documented shape differences, all value-preserving: (1) retail pushes FIVE dwords for 0x00401900 (the two leading zeros are the task-list
//   argument and the fifth dword that callee never reads - see task_types.hpp) and cleans them in the 0x2c; the port's four-parameter signature emits
//   one zero for the task-list argument, so the port's compiler cleans ten dwords where retail cleans eleven; (2) the two globals that retail seeds
//   with the literals 0x5BDA74 and 0x5BDA70 are stored as the port addresses of `g_5bda74` and `g_5bda70`, because a retail address is not mapped in
//   the port - the stored values are the list header's own node addresses, so they must name the port's objects (this is the class
//   scripts/check_native_raw_addresses.py exists to catch, and this file reports zero hits); (3) `push %esi`/`push %edi` around the loop are the
//   compiler's register saves for the two loop variables, not call arguments; (4) the ten table entries are string literals, so the port's table is
//   `char const* [10]` in read-only data where retail's writable `.data` table holds pointers to its own strings - retail's entries are only ever
//   read, and the one `const_cast` at the call is required by the port's existing placeholder signature for 0x004B8810, whose whole callee chain
//   takes the name as `char const*` and never writes through it (`Script_QHashNodeCreate`'s own first parameter is `char const*`, and 0x004C0560
//   receives it as `char const* value`).
//   Global ownership from the image-wide immediate census (a byte scan of `.text` for each four-byte address): 0x005BDA70 is referenced at
//   0x00453556 and 0x00453564 (this body), 0x004536C9 (inside handler 0x00453680, the `Sys_InsertListTail_4AF8F0` head) and 0x00453841 (inside
//   0x00453840, which reads it as its list head); 0x005BDA74 at 0x0045355A and 0x0045356A (this body only); 0x005BDA78 at 0x00453560 (this body only);
//   0x005BDA7C at 0x004535E5 (this body) and 0x00453676 (inside handler 0x00453670, which returns it); 0x0053E29C at 0x0045358E, 0x00453599,
//   0x004535BA and 0x004535D5 (this body) plus 0x00453712 (inside 0x00453710); 0x0053E2A0 at 0x00453605 (this body only). The definitions below are
//   owned here because this body writes them first, and the other bodies listed must declare them `extern` rather than define a second copy.
//   Handlers 0x00453630, 0x00453650, 0x00453670 and 0x00453680 are raw code immediates in retail and are not transcribed yet (native gaps inside
//   0x00430000-0x0045FFFF, this lane's band). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {

// 0x004B8810 (script/native_4b8810.cpp): script hash-node registration. Its two arguments are (name, value): the callee loads argument 1 into ecx
// and argument 2 into eax and pushes them into 0x004BAAC0 as (name, handler, value, 4, 2, 1, 0). The parameter is `void*` in that file's signature,
// hence the const_cast at the call below.
void Script_QHashNodeRegister(void* const arg1, void* const value);

// 0x0053E29C: the "PatrolPath" task type id (word). Image bytes 81 01 = 0x0181, the sentinel that means "no type registered", so the initialiser is
// the image value and not zero. `.data`: `.rdata` ends at 0x005358F6 and `.data` starts at 0x00536000, and this image has no `.bss`.
std::uint16_t g_53e29c = 0x0181U;

// 0x0053E2A0: the ten-entry table of `PATROLPATHCOMMAND_*` script names this body registers. Each initialiser is the literal stored at the address
// the retail entry points to; the table's extent is measured from the loop bound 0x0053E2C8 (base + 0x28 = ten dwords) rather than assumed.
char const* g_53e2a0[10] = {
    "PATROLPATHCOMMAND_NONE",      // [0] retail 0x0053E2A0 -> 0x0053E3B8, registered with value -1
    "PATROLPATHCOMMAND_ANIMATION", // [1] retail 0x0053E2A4 -> 0x0053E39C, registered with value 0
    "PATROLPATHCOMMAND_DELAY",     // [2] retail 0x0053E2A8 -> 0x0053E384, registered with value 1
    "PATROLPATHCOMMAND_WALKTO",    // [3] retail 0x0053E2AC -> 0x0053E368, registered with value 2
    "PATROLPATHCOMMAND_RUNTO",     // [4] retail 0x0053E2B0 -> 0x0053E350, registered with value 3
    "PATROLPATHCOMMAND_CROUCH",    // [5] retail 0x0053E2B4 -> 0x0053E334, registered with value 4
    "PATROLPATHCOMMAND_LOOKATNODE",// [6] retail 0x0053E2B8 -> 0x0053E314, registered with value 5
    "PATROLPATHCOMMAND_END",       // [7] retail 0x0053E2BC -> 0x0053E2FC, registered with value 6
    "PATROLPATHCOMMAND_QUIT",      // [8] retail 0x0053E2C0 -> 0x0053E2E4, registered with value 7
    "PATROLPATHCOMMAND_SETSPEED",  // [9] retail 0x0053E2C4 -> 0x0053E2C8, registered with value 8
};

// 0x005BDA70, 0x005BDA74, 0x005BDA78: the three dwords of the PatrolPath list header this body seeds. All three are zero in the image (past
// `.data`'s raw size) and all three are written here, so the definitions are owned here; 0x005BDA70 is also read by handler 0x00453680 and by
// 0x00453840, which must declare it `extern`.
std::uint32_t g_5bda70 = 0U;
std::uint32_t g_5bda74 = 0U;
std::uint32_t g_5bda78 = 0U;

// 0x005BDA7C: cleared by this body and read by handler 0x00453670, which returns it to its caller. Zero in the image.
std::uint32_t g_5bda7c = 0U;

void Task_DefinePatrolPath()
{
    // 0x00453550: push 0; 0x00453552: push 0 - arguments 4 and 5 of the 0x00401900 call below, pushed BEFORE the parent getter: the deferred-push
    // inversion. 0x00453554: movl $0x5BDA74,0x5BDA70; 0x0045355E: movl $0x5BDA70,0x5BDA78; 0x00453568: movl $0,0x5BDA74.
    ::g_5bda70 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_5bda74)); // 0x00453554: the port address of the retail 0x005BDA74
    ::g_5bda78 = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&::g_5bda70)); // 0x0045355E: the port address of the retail 0x005BDA70
    ::g_5bda74 = 0U;                                                                        // 0x00453568

    // 0x00453572: call 0x004018F0 (parent type); 0x00453577: push %eax; 0x00453578: push 0x34; 0x0045357A: push 0x0053E3D0; 0x0045357F: call 0x00401900.
    const std::uint16_t patrol_path_id = TaskType_Register("PatrolPath", // 0x0045357A: push 0x0053E3D0
                                                          0x34U,         // 0x00453578: push 0x34 (object size; there is no frame in this body)
                                                          QTask_GetCurrentType(), // 0x00453572: call 0x004018F0 (parent type)
                                                          0U);           // 0x00453552: push 0 (task-list argument)

    // 0x00453584: push 0x00453630 (handler); 0x00453589: push 0x0 (argument 2, literal event id); 0x0045358B: push %eax (argument 1, the id);
    // 0x0045358C: mov word [0x0053E29C],ax; 0x00453592: call 0x00401400. The store precedes the link call in retail and is kept before it here.
    ::g_53e29c = patrol_path_id; // 0x0045358C
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(patrol_path_id), 0x0U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453630U)));

    // 0x00453597: mov word [0x0053E29C],%ax; 0x0045359D: push 0x00453650 (handler); 0x004535A2: push 0x2 (argument 2, literal event id);
    // 0x004535A4: push %eax (argument 1, the id RELOADED from the global); 0x004535A5: call 0x00401530 - the no-context link.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_53e29c), 0x2U,
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453650U)));

    // 0x004535AD: push 0x00453670 (handler, deferred argument 3); 0x004535B2: call 0x004F1A60 (supplies argument 2, the event id);
    // 0x004535B7: mov word [0x0053E29C],%cx; 0x004535BE: push %eax; 0x004535BF: push %ecx; 0x004535C0: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53e29c), static_cast<std::uint32_t>(Script_PhaseIdGetE4_4F1A60()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453670U)));

    // 0x004535C8: push 0x00453680 (handler, deferred argument 3); 0x004535CD: call 0x004F1A70 (supplies argument 2, the event id);
    // 0x004535D2: mov word [0x0053E29C],%dx; 0x004535D9: push %eax; 0x004535DA: push %edx; 0x004535DB: call 0x00401400.
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_53e29c), static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(static_cast<std::uintptr_t>(0x453680U)));

    // 0x004535E3: movl $0x0,0x005BDA7C. Retail then jumps over the alignment pad 0x004535F2-0x004535FF to the loop label at 0x00453600.
    ::g_5bda7c = 0U;

    // 0x00453600: push %esi; 0x00453601: push %edi (compiler register saves for the two loop variables); 0x00453602: xor %edi,%edi;
    // 0x00453604: mov $0x0053E2A0,%esi. The loop test is at the BOTTOM (0x0045361C `cmp $0x0053E2C8,%esi`; 0x00453622 `jl 0x00453609`), so this is a
    // do-while and the body always runs once. Each iteration: 0x00453609 `mov (%esi),%ecx` (the name pointer, argument 1); 0x0045360B
    // `lea -0x1(%edi),%eax` (the value, argument 2, so the first entry gets -1); 0x0045360E/0x0045360F push both; 0x00453610 `call 0x004B8810`;
    // 0x00453615 `add $0x8,%esp`; 0x00453618 `add $0x4,%esi`; 0x0045361B `inc %edi`.
    std::int32_t index = 0;
    char const* const* entry = ::g_53e2a0;
    do
    {
        // The const_cast is required only because the port's placeholder signature for 0x004B8810 takes `void*`; the callee chain reads the name as
        // `char const*` and never writes through it.
        Script_QHashNodeRegister(const_cast<char*>(*entry),
            reinterpret_cast<void*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(index - 1))));
        entry += 1;  // 0x00453618
        index += 1;  // 0x0045361B
    } while (entry < &::g_53e2a0[10]); // 0x0045361C/0x00453622: retail compares against 0x0053E2C8 = the table base plus ten four-byte entries
} // end of Task_DefinePatrolPath

} // extern "C"
