// Native: Task_ClearClassTypeAndDeleteList_4497D0 (behavior-derived placeholder: it clears the task type whose id is at 0x0057C214 and then deletes the task list whose handle is at 0x0057C21C; retail symbol unknown); retail VA 0x004497D0-0x004497EB.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 28 bytes (0x1C), 1 block, 0 edges,
//   7 instructions, counted over the exact image. The body ends at the `ret` at 0x004497EB; 0x004497EC starts the next function.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x57c214,%ax` (0x004497D0, a 16-bit absolute load into AX), `push %eax`
//   (0x004497D6), `call 0x00401A20` (0x004497D7), `mov 0x57c21c,%ecx` (0x004497DC, a 32-bit load), `push %ecx` (0x004497E2),
//   `call 0x004C1830` (0x004497E3), `add $0x8,%esp` (0x004497E8), `ret` (0x004497EB). No branch. Two call sites: 0x00401A20
//   (TaskType_Clear_401A20) and 0x004C1830 (QTaskList_Delete, named by engine/include/igi/retail/core/qtask_list.hpp:18; that file records
//   it as deliberately not reconstructed yet). The single `add $0x8,%esp` releases BOTH pushed argument dwords, so neither callee pops its
//   own argument and the port calls both as ordinary cdecl calls.
// Behavior/CFG: no loop, no return value, no arguments read. It reads the 16-bit word at 0x0057C214 and passes it to the type-clear routine,
//   then reads the 32-bit dword at 0x0057C21C and passes it to the task-list delete. The 16-bit load leaves retail's upper EAX bits
//   untouched; the callee masks with 0xFFFF at its own 0x00401A25, so only the low word is observable.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 7-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) 0x0057C214 and 0x0057C21C are
//   defined by core/native_449360.cpp (the class registrar: +0x57C214 receives the 16-bit type id it registers), and both are declared here
//   with those exact types. (c) QTaskList_Delete is not reconstructed yet, so this file reports it as undefined in the tree; that is the true
//   state, not a defect in this file. (d) The type of the value passed to QTaskList_Delete is not established by this body - it is loaded and
//   pushed as one 32-bit dword, so it is carried as a 32-bit value and no pointer claim is made.
#include "dev_log.hpp"
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern void TaskType_Clear_401A20(std::uint16_t class_id);        // 0x00401A20: clears the type slot for a class id (core/native_401a20.cpp)
extern void QTaskList_Delete_4C1830(std::uint32_t list);          // 0x004C1830: deletes a task list (not reconstructed yet)

// 0x0057C214 and 0x0057C21C: class id and list handle, both defined by the class registrar core/native_449360.cpp.
extern std::uint16_t g_57c214;
extern std::uint32_t g_57c21c;

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_4497D0()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_4497D0", "0x004497D0");
    // 0x004497D0: mov 0x57c214,%ax; 0x004497D6: push %eax; 0x004497D7: call 0x401a20.
    TaskType_Clear_401A20(::g_57c214);

    // 0x004497DC: mov 0x57c21c,%ecx; 0x004497E2: push %ecx; 0x004497E3: call 0x4c1830; 0x004497E8: add $0x8,%esp.
    QTaskList_Delete_4C1830(::g_57c21c);
}

} // extern "C"

#undef NATIVE_DECL
