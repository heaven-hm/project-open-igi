// Native: MenuWindow_Register_423FC0 (placeholder name; retail symbol unknown; the class-name string it registers is "MenuWindow" at 0x0053B9EC). retail VA 0x00423FC0-0x004241B3 (500 bytes, 1 block, 0 edges, 126 instructions).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 `pd` over the exact image plus capstone 5.0.7 aligned from the verified entry 0x00423FC0; the two listings agree. Extent: entry `push 0` (there is NO frame allocation in this routine - no `sub esp` anywhere), `c3` at 0x004241B3, and the byte after the ret is 0x90 (0x004241B4 onward is alignment padding, NOT part of the routine), so the extent is 500 bytes. There is no single epilogue: this body cleans its stack incrementally at four sites, and a replay with esp simulated returns esp to its entry value at the ret with zero slack - `add esp,0x38` at 0x00424053 = 2 (the two surplus zero words before the parent getter) + 3 (0x00401900 with its pre-pushed fourth argument) + 1 + 1 + 1 + 1 + 1 + 1 (the six attribute calls, one push each) + 3 (the first 0x00401400 link); `add esp,0x3C` at 0x004240CA = 3+3+3+3+3 (the 0x004F1A70-driven link, the two byte-event links, the no-op link and the 0x0053B910 link); `add esp,0x44` at 0x0042414D = 3 (the 0x004E7530-driven link) + 3 (0x00488700) + 3 (0x0053B914) + 3 (0x0041A120) + 3 (0x0053B91C) + 2 (the first 0x004B8810 registration); `add esp,0x40` at 0x004241B0 = 2 x 8 (the remaining eight 0x004B8810 registrations). Call census from the 0xE8 enumeration, not from handler immediates: 0x004018F0 x1, 0x00401900 x1, 0x00401810 x3, 0x004017C0 x2, 0x00401530 x6, 0x00401400 x5, 0x004E7530 x1, 0x004F1A70 x1, 0x004B8810 x9; 30 call instructions in all, no branch of any kind.
// Behavior/CFG: one straight-line block, no branch, no loop. NOTE THE PARENT GETTER: this routine reads the parent type through 0x004018F0 (QTask_GetCurrentType, `mov ax,[0x54F928]; ret`), NOT through 0x00424850 the way every sibling registrar in this band does. It registers "MenuWindow" with a 0x54-byte object through 0x00401900 and stores the 16-bit id at 0x0053B90C. That store is the ONLY write to 0x0053B90C anywhere in .text (a byte census over 0x00401000-0x00532510 finds `66 a3 0c b9 53 00` at exactly one address, 0x00423FD7), and 0x00424850 is exactly `mov ax,[0x53B90C]; ret` - so THIS routine publishes the task type that the parent getter 0x00424850 returns to the whole band, which is a real structural relationship between this file and every sibling, not an inference from naming. It then fills five attribute slots on the new class: 0x0053B910, 0x0053B914, 0x0053B918 and 0x0053B91C from 0x00401810 (three of them) and 0x004017C0 (the dword slot), and the two BYTE slots 0x0053B920 and 0x0053B921 from 0x004017C0. Then eleven handler links on the class id, in retail order: 0x00401400 event 0 -> 0x004241C0; 0x00401400 the byte from 0x004F1A70 -> 0x00424260; 0x00401400 the byte at 0x0053B920 -> 0x00424420; 0x00401400 the byte at 0x0053B921 -> 0x00424400; 0x00401530 event 2 -> 0x004D3710; 0x00401530 the dword at 0x0053B910 -> 0x004244B0; 0x00401530 the event word from 0x004E7530 -> 0x00424280; 0x00401530 event 1 -> 0x00488700; 0x00401530 the dword at 0x0053B914 -> 0x00424300; 0x00401530 the dword at 0x0053B918 -> 0x0041A120; 0x00401530 the dword at 0x0053B91C -> 0x00424380. Finally nine script registrations through 0x004B8810 with the LINEUPMODE_* strings and the ids 0, 1, 2, 3, 7, 4, 5, 6, 8 in that order - note 7 comes fifth, so the ids are NOT ascending and must not be sorted; 7 is LINEUPMODE_VERT_FILLOUT and it sits between VERT_BOTTOM (3) and HOR_LEFT (4), which is retail's order. STRUCTURAL ABSENCES, both measured, neither an omission: (a) this registrar allocates NO parameter container - no 0x004F1950 call and no 0x004F1A80 call anywhere in the body; (b) it has NO frame. Both facts distinguish it from the five sibling registrars in this band, and a later reader must not "fix" them by adding a container or a scratch buffer.
// Partial-register note (measured, kept deliberately): the two byte-event links push ecx/eax that retail only partially defines - 0x0042406D is `mov cl,[0x53B920]` and 0x00424086 is `mov al,[0x53B921]`, each after a call that clobbers the rest of the register, so the upper bytes of the pushed word are whatever the callee left behind. The port passes the zero-extended byte. This cannot change behaviour because 0x00401400 masks the event word to its low byte when it computes the phase index, and the low byte is the only part retail defines. The same holds for the dword slots: 0x00401810 returns a full dword in EAX (its body sets `mov eax,0x20` and increments it), so storing it and re-reading it as a dword is value-identical to the port's zero-extended std::uint8_t return.
// Status/limits: TRANSCRIBED; LINKABLE (of the eleven handler-link addresses, eight are raw code immediates in retail that nobody has transcribed - 0x004241C0, 0x00424260, 0x00424420, 0x00424400, 0x004244B0, 0x00424300, 0x00424380 and the cross-band 0x0041A120 - so they are passed as cast integers and create no link dependency; they stay recorded gaps in the tree, not stubs. 0x00424280, 0x00488700 and 0x004D3710 ARE transcribed and are referenced as symbols, so they are real link edges). STRUCTURALLY_VERIFIED for the CFG only, which is one block with no edges. BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile clean). Runtime UNOBSERVED in both binaries. There are no `lea`-derived frame offsets in this file because there is no frame. The two byte-event links and the four attribute stores are the only places where a width conversion happens, and both are argued above.
// Ownership note (per the Lead's two-variable trap, here in its sharpest form): FIVE of the seven globals this routine writes are ALREADY DEFINED elsewhere in the tree and are declared here as C-linkage externs so that they bind to the existing definitions - 0x0053B90C (engine/src/retail/sys/native_424850.cpp), 0x0053B914 (sys/native_424870.cpp), 0x0053B918 (sys/native_424880.cpp), 0x0053B91C (sys/native_424890.cpp) and 0x0053B920 (sys/native_424b80.cpp); each of those files defines its slot inside `extern "C"`, and all five are in the build (symbols _g_53b90c, _g_53b914, _g_53b918, _g_53b91c, _g_53b920). Declaring any of them here inside `namespace igi::retail::core` would have created a SECOND variable with NO link error, and the getter natives would have kept reading the empty one. Only 0x0053B910 and 0x0053B921 have no owner in the tree; they are defined here, also with C linkage, to match the linkage of the sibling slots in the same 0x53B9xx block. 0x0053B910 has a retail getter at 0x00424860 (`mov eax,[0x53B910]; ret`) which is a separate, still untranscribed VA; 0x0053B921 is read by the untranscribed handler at 0x004244B0 (0x004244F1) and by nothing else.
#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x0053B90C: NOT defined here - engine/src/retail/sys/native_424850.cpp owns it. This routine is its only
// writer in the image (0x00423FD7) and 0x00424850 is its only reader native.
extern std::uint16_t g_53b90c;
// 0x0053B914, 0x0053B918, 0x0053B91C, 0x0053B920: NOT defined here - owned by sys/native_424870.cpp,
// sys/native_424880.cpp, sys/native_424890.cpp and sys/native_424b80.cpp respectively. C linkage on purpose.
extern std::uint32_t g_53b914;
extern std::uint32_t g_53b918;
extern std::uint32_t g_53b91c;
extern std::uint8_t g_53b920;

// 0x0053B910: no other owner in the tree. Image value 0x000000FF. Read at 0x004240B2 (this routine), at
// 0x004247EA and by its own untranscribed getter 0x00424860 (`mov eax,[0x53B910]; ret`).
std::uint32_t g_53b910 = 0x000000FFU; // 0x0053B910
// 0x0053B921: no other owner in the tree. Image value 0xFF (the byte after it belongs to the next string).
// Read at 0x00424086 (this routine) and at 0x004244F1 inside the untranscribed handler 0x004244B0.
std::uint8_t g_53b921 = 0xFFU; // 0x0053B921

extern std::uint16_t NATIVE_DECL QTask_GetCurrentType();                                  // 0x004018F0 (declared in igi/retail/core/task_types.hpp)
extern std::uint32_t NATIVE_DECL QTask_AllocateEvent(std::uint16_t value);                // 0x004017C0 (declared in igi/retail/core/task_types.hpp)
// 0x00401810: declared at its definition site only (engine/src/retail/core/native_401810.cpp); no header carries it.
extern std::uint8_t QTask_NewQTaskEventNoContextFn(std::uint16_t value);                  // 0x00401810
extern std::uint32_t NATIVE_DECL Core_GetEventWord_4E7530();                              // 0x004E7530: the current event word
extern std::uint8_t Script_PhaseIdGetE5_4F1A70(void);                                     // 0x004F1A70: argument-less byte getter
extern void Script_QHashNodeRegister(void* arg1, void* value);                            // 0x004B8810 (declared in engine/src/retail/core/native_402ff0.cpp:238)
// 0x00424280 and 0x00488700 ARE transcribed, so they are referenced as symbols and are real link edges.
extern void NATIVE_DECL Task_WalkAndDispatchEvent_424280(void* task);                     // 0x00424280
extern void NATIVE_DECL Task_WalkAndDispatch_488700(void* task);                          // 0x00488700
extern void NATIVE_DECL QTask_NoOpHandler(void);                                          // 0x004D3710: single `ret`

void NATIVE_DECL MenuWindow_Register_423FC0()
{
    NATIVE_LOG_ENTER("MenuWindow_Register_423FC0", "0x00423FC0");
    // 0x00423FC0/0x00423FC2: the two zeros are surplus for the argument-less getter and are consumed as
    // 0x00401900's fourth argument (task list = 0). The parent getter here is 0x004018F0, NOT 0x00424850.
    const std::uint16_t parent = QTask_GetCurrentType();                      // 0x00423FC4
    const std::uint16_t class_id =
        TaskType_Register("MenuWindow", 0x54U, parent, 0U);                   // 0x00423FD1
    g_53b90c = class_id;                                                      // 0x00423FD7 (the only write to 0x0053B90C in .text)

    // 0x00423FD6..0x00424035: six attribute stores on the new class, one push each.
    g_53b910 = QTask_NewQTaskEventNoContextFn(class_id);                      // 0x00423FDD
    g_53b920 = static_cast<std::uint8_t>(QTask_AllocateEvent(class_id));      // 0x00423FEE (byte store: 0x00423FFA)
    g_53b921 = static_cast<std::uint8_t>(QTask_AllocateEvent(class_id));      // 0x00424000 (byte store: 0x0042400C)
    g_53b914 = QTask_NewQTaskEventNoContextFn(class_id);                      // 0x00424012
    g_53b918 = QTask_NewQTaskEventNoContextFn(class_id);                      // 0x00424023
    g_53b91c = QTask_NewQTaskEventNoContextFn(class_id);                      // 0x00424035

    // 0x0042404E: the handler immediate is pre-pushed and is the surplus argument of the link's own frame.
    QTask_LinkQTaskEventHandler(
        class_id, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004241C0)); // 0x0042404E
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(Script_PhaseIdGetE5_4F1A70()),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00424260));       // 0x00424068
    // 0x0042406D/0x00424086: only CL/AL is defined in retail; the port passes the zero-extended byte.
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(g_53b920),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00424420));       // 0x00424081
    QTask_LinkQTaskEventHandler(
        class_id, static_cast<std::uint32_t>(g_53b921),
        reinterpret_cast<igi::retail::core::FrameHandler>(0x00424400));       // 0x00424099
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(&QTask_NoOpHandler)); // 0x004240AD
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, g_53b910, reinterpret_cast<igi::retail::core::FrameHandler>(0x004244B0)); // 0x004240C5
    // 0x004240CD: the handler immediate is pre-pushed and is the surplus argument of 0x004E7530, which reads none.
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, Core_GetEventWord_4E7530(),
        reinterpret_cast<igi::retail::core::FrameHandler>(&Task_WalkAndDispatchEvent_424280)); // 0x004240E0
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(&Task_WalkAndDispatch_488700)); // 0x004240F3
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, g_53b914, reinterpret_cast<igi::retail::core::FrameHandler>(0x00424300)); // 0x0042410C
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, g_53b918, reinterpret_cast<igi::retail::core::FrameHandler>(0x0041A120)); // 0x00424124
    QTask_LinkQTaskEventHandlerNoContextFn(
        class_id, g_53b91c, reinterpret_cast<igi::retail::core::FrameHandler>(0x00424380)); // 0x0042413C

    // 0x00424141..0x004241AB: nine script registrations, two pushes each. Retail pushes the id first and the
    // string last, so the string is argument 1 and the id is the value. Ids are in retail order, NOT sorted.
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_NONE"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(0U)));          // 0x00424148
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_VERT_TOP"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(1U)));      // 0x00424157
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_VERT_CENTRE"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(2U)));   // 0x00424163
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_VERT_BOTTOM"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(3U)));   // 0x0042416F
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_VERT_FILLOUT"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(7U)));  // 0x0042417B
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_HOR_LEFT"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(4U)));      // 0x00424187
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_HOR_CENTRE"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(5U)));    // 0x00424193
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_HOR_RIGHT"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(6U)));     // 0x0042419F
    Script_QHashNodeRegister(const_cast<char*>("LINEUPMODE_HOR_FILLOUT"), reinterpret_cast<void*>(static_cast<std::uintptr_t>(8U)));   // 0x004241AB
}

} // extern "C"
