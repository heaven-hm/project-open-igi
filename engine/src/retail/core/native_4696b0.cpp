// Native: Task_DefineComputerMap (behavior-derived placeholder name; retail symbol unknown); retail VA 0x004696B0-0x0046973C.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 141 bytes, 1 block, 37 instructions.
// Tools/evidence: radare2 6.2.0 af/afbj/pdf with bin.relocs.apply over the exact image (1 block / 37 instructions / 141 bytes / 0 edges) plus
//   the objdump listing build/retail-dis.txt for the same extent (0x004696B0-0x0046973C; the ret is at 0x0046973C, three nops follow, and
//   0x00469740 is the next function, which this body uses as a handler); `ps` at 0x0053F778 for the type name and at 0x0053F764 for the
//   registered name (byte-identical to the two literals below), and `px 24 @ 0x0053F760` for their adjacency to the allocated-event byte;
//   `pxw` at 0x005BDD90 and 0x005BDDA0 for the image values; a stack-pointer simulation over all 37 instructions, which returns esp to its
//   entry value at the ret and accounts for both cleanups (0x38 = fourteen pushes: the two inert arguments, the three registration arguments
//   and three three-push links; 0x18 = six pushes: the fourth link's three, the QTask_AllocateEvent argument and the last call's two). Every
//   callee was re-read at its own VA, which is how 0x004E7530 was established as an argument-less dword load, how 0x004B7F40 was established as
//   a two-argument wrapper over 0x004BAAC0, and how 0x005BDDA0 was established as a 16-bit object. Argument numbers were derived by counting the
//   pushes above each access, not by reading an [esp+N] offset as an argument number.
// Behavior/CFG: one straight-line body, no frame of its own, 37 instructions. It registers the "ComputerMap" task type of size 0xAC with parent
//   QTask_GetCurrentType() -> id at 0x005BDD90, links FOUR handlers - e0->0x00469740 (0x00401400), e1->0x0046A0D0 and e2->0x0046AA80
//   (0x00401530), and 0x00424280 on the event id from the argument-less getter 0x004E7530 (0x00401530) - then allocates one event id through
//   QTask_AllocateEvent(0) and stores its low byte at 0x0053F760, and finally registers the named 16-bit object 0x005BDDA0 under the name
//   "Computer_nZoomLevel" through 0x004B7F40. One basic block, no branch, no loop, 9 call sites over 7 distinct callees. It allocates no
//   parameter container, appends no records, and has NO 0x004B8810 script-name tail.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The id slot 0x005BDD90 holds image value 0 and is written by the 16-bit store at 0x004696D1;
//   it is read back by 0x004696DC, 0x004696EF and 0x00469710 here and by 0x0046AAD0/0x0046AAF0 in the e2 handler. 0x005BDDA0 holds image value 0
//   and is declared here as std::uint16_t, which is what every access to it in the image shows: 0x004697B7 reads it with `movswl 0x5BDDA0,%eax`,
//   0x00469E8B with `cmpw $0x3,0x5BDDA0`, 0x00469E95 with `incw`, 0x00469EA8 with `cmpw $0x0` and 0x00469EB2 with `decw` - all 16-bit, no
//   32-bit access anywhere - and the callback 0x004B7F60 that 0x004B7F40 installs for it also reads it with `movsx eax,word [eax]`. The byte at
//   0x0053F760 holds image value 0xFF and is written by the QTask_AllocateEvent result at 0x0046972F; the "Computer_nZoomLevel" literal begins
//   four bytes later at 0x0053F764, the third case in this lane of a name literal adjacent to that byte (after 0x0053FE28/0x0053FE2C and
//   0x0053FDE0/0x0053FDE8). 0x004B7F40 is above 0x0048FFFF and has exactly ONE caller in the whole image, this one; its body reads its second
//   argument from `[esp+8]` and its first from `[esp+4]`, then calls 0x004BAAC0 with (name, 0x004B7F60, word_pointer, 2, 0, 1, 1), so the pair is
//   (name, object pointer) and this file declares it accordingly. Its return value is discarded at this call site - the next instruction is the
//   cleanup - so the declaration says void, which cannot change behaviour. The registered name literal is passed as a pointer to the byte-identical
//   literal, as in the other registrar files. Documented shape differences, all value-preserving: (1) the getters 0x004E7530 and 0x004018F0 are
//   argument-less loads in retail that ignore the arguments the caller pushes, and the tree's transcriptions take no arguments, so those inert
//   pushes are not reproduced; the pushes that precede 0x004E7530 are simultaneously the next 0x00401530 link's pre-pushed handler, which the port
//   keeps as the handler value in the same order; (2) byte-valued event ids are zero-extended in the port, which cannot change behaviour because
//   every link fed by a byte getter here goes through 0x00401400 or 0x00401530, which mask the event word for the phase index; (3) the two
//   deferred cleanups are reproduced implicitly by the tree's calling convention rather than by explicit `add esp` arithmetic, since the port
//   never pushes the inert arguments; the retail sizes are recorded here as 0x38 and 0x18 so a future reviewer can check the arithmetic.
//   Handlers 0x00469740, 0x0046A0D0 and 0x0046AA80 are raw code immediates in retail and are not transcribed yet (native gaps in
//   0x0046xxxx-0x0048xxxx); 0x00424280 lies below 0x00460000 and is referenced 5 times image-wide (0x004240CD, 0x00469706, 0x00485203,
//   0x00504E5A, 0x00505683 - other lane). Domain: core/, because it is one of the 0x00402930 startup registrars.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" std::uint32_t Core_GetEventWord_4E7530(void);          // 0x004E7530: `mov eax,[0x5488C8]; ret` (event id)
// 0x004B7F40: above 0x0048FFFF and not transcribed in this lane. Proven shape only: it forwards (name, object pointer) to 0x004BAAC0 together
// with the callback 0x004B7F60 and the constants 2, 0, 1, 1. Its return value is discarded at the only call site in the image.
extern "C" void Sys_RegisterNamedWord_4B7F40(char const* name, std::uint16_t* value); // 0x004B7F40 (above 0x0048FFFF)

extern "C" {

// 0x005BDD90: the "ComputerMap" task type id. Image value 0, overwritten by a 16-bit store from TaskType_Register's return value.
std::uint16_t g_5bdd90 = 0U;

// 0x005BDDA0: the 16-bit object registered under the name "Computer_nZoomLevel" through 0x004B7F40. Image value 0. Every access in the image is
// a word access (see Status/limits), and the callback 0x004B7F60 reads it with `movsx eax,word [eax]`.
std::uint16_t g_5bdda0 = 0U;

// 0x0053F760: the low byte of the event id allocated by QTask_AllocateEvent(0). Image value 0xFF, written at 0x0046972F by a byte store, and the
// "Computer_nZoomLevel" name literal starts four bytes later at 0x0053F764.
std::uint8_t g_53f760 = 0xFFU;

void Task_DefineComputerMap()
{
    // 0x004696B0/0x004696B2: push 0; push 0 (both inert arguments of the parent getter); 0x004696B4: call 0x4018F0; 0x004696B9: push eax;
    // 0x004696BA: push 0xAC; 0x004696BF: push "ComputerMap"; 0x004696C4: call 0x401900.
    const std::uint16_t computer_map_id = TaskType_Register("ComputerMap", // 0x004696BF: push 0x0053F778
                                                            0xACU,          // 0x004696BA: push 0xAC (object size)
                                                            static_cast<std::uint16_t>(QTask_GetCurrentType()), // 0x004696B4: call 0x4018F0
                                                            0U);            // 0x004696B2: push 0 (inert)

    // 0x004696C9: push 0x00469740 (pre-pushed handler); 0x004696CE: push 0; 0x004696D0: push eax; 0x004696D1: mov word [0x5BDD90],ax (the id
    // store lands between the pushes); 0x004696D7: call 0x401400.
    ::g_5bdd90 = computer_map_id;
    QTask_LinkQTaskEventHandler(static_cast<std::uint16_t>(::g_5bdd90), // 0x004696D1: mov ax,[0x5BDD90]
                                0U,                                     // 0x004696CE: push 0 (argument 2)
                                reinterpret_cast<igi::retail::core::FrameHandler>(
                                    static_cast<std::uintptr_t>(0x00469740U))); // 0x004696C9: push handler

    // 0x004696DC: mov ax,[0x5BDD90]; 0x004696E2: push 0x0046A0D0; 0x004696E7: push 1; 0x004696E9: push eax; 0x004696EA: call 0x401530.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdd90), // 0x004696DC: mov ax,[0x5BDD90]
                                           1U,                                     // 0x004696E7: push 1 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046A0D0U))); // 0x004696E2: push handler

    // 0x004696EF: mov cx,[0x5BDD90]; 0x004696F6: push 0x0046AA80; 0x004696FB: push 2; 0x004696FD: push ecx; 0x004696FE: call 0x401530;
    // 0x00469703: add esp,0x38 - fourteen pushes: the two inert arguments, the three registration arguments and three three-push links.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdd90), // 0x004696EF: mov cx,[0x5BDD90]
                                           2U,                                     // 0x004696FB: push 2 (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x0046AA80U))); // 0x004696F6: push handler

    // 0x00469706: push 0x00424280 (the handler, below 0x00460000, and the inert argument of the getter); 0x0046970B: call 0x4E7530;
    // 0x00469710: mov dx,[0x5BDD90]; 0x00469717: push eax; 0x00469718: push edx; 0x00469719: call 0x401530. This link has no cleanup of its own:
    // the final add esp,0x18 covers it together with the next two calls.
    QTask_LinkQTaskEventHandlerNoContextFn(static_cast<std::uint16_t>(::g_5bdd90), // 0x00469710: mov dx,[0x5BDD90]
                                           static_cast<std::uint32_t>(Core_GetEventWord_4E7530()), // 0x00469717: push eax (argument 2)
                                           reinterpret_cast<igi::retail::core::FrameHandler>(
                                               static_cast<std::uintptr_t>(0x00424280U))); // 0x00469706: push handler (below 0x00460000)

    // 0x0046971E: push 0 (QTask_AllocateEvent's argument); 0x00469720: call 0x4017C0; 0x00469725: push 0x005BDDA0;
    // 0x0046972A: push "Computer_nZoomLevel" (0x0053F764); 0x0046972F: mov byte [0x0053F760],al - only the low byte of the returned id is kept,
    // and the store lands between the two pushes of the next call; 0x00469734: call 0x4B7F40; 0x00469739: add esp,0x18 - six pushes: this link's
    // three, the QTask_AllocateEvent argument and the last call's two.
    ::g_53f760 = static_cast<std::uint8_t>(QTask_AllocateEvent(0));
    Sys_RegisterNamedWord_4B7F40("Computer_nZoomLevel", // 0x0046972A: push 0x0053F764 (argument 1)
                                 &::g_5bdda0);          // 0x00469725: push 0x005BDDA0 (argument 2)

    // 0x0046973C: ret.
} // end of Task_DefineComputerMap

} // extern "C"
