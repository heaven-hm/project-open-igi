// Native: Task_ClearClassType_449290 (behavior-derived placeholder: it clears the task type whose id is the single global it reads; retail symbol unknown); retail VA 0x00449290-0x0044929D.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; 14 bytes (0xE), 1 block, 0 edges,
//   5 instructions, counted over the exact image. The body ends at the `ret` at 0x0044929D; the next function starts at 0x004492A0.
// Tools/evidence: build/retail-dis.txt, the complete extent: `mov 0x57c20c,%ax` (0x00449290, a 16-bit absolute load into AX),
//   `push %eax` (0x00449296), `call 0x00401A20` (0x00449297), `pop %ecx` (0x0044929C), `ret` (0x0044929D). No branch, no other callee.
//   The callee is TaskType_Clear_401A20(std::uint16_t) (core/native_401a20.cpp), and the argument slot is released by `pop %ecx`.
// Behavior/CFG: no argument is read and no value returns. It loads the 16-bit word at 0x0057C20C and calls the type-clear routine with it.
//   The load is 16-bit, so retail's upper EAX bits are whatever the preceding instruction left; the callee masks its argument with 0xFFFF at its
//   own 0x00401A25, so only the low word is observable and the port passes the std::uint16_t value.
// Status/limits: TRANSCRIBED; STRUCTURALLY_VERIFIED against the 5-instruction listing; runtime UNOBSERVED. Recorded limits: (a) the name is a
//   behavior-derived placeholder with the address kept as its suffix; the retail symbol is not established. (b) 0x0057C20C is declared here
//   but not defined: its writer is a class registrar that is not transcribed yet, so this file correctly reports that global as undefined
//   until that registrar lands. (c) What 0x00401A20 does beyond releasing the type's slot is that file's scope.
#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

extern void TaskType_Clear_401A20(std::uint16_t class_id); // 0x00401A20: clears the type slot for a class id (core/native_401a20.cpp)

// 0x0057C20C: a class id this routine clears; its writer is an untranscribed class registrar.
extern std::uint16_t g_57c20c;

void NATIVE_DECL Task_ClearClassType_449290()
{
    // 0x00449290: mov 0x57c20c,%ax; 0x00449296: push %eax; 0x00449297: call 0x401a20; 0x0044929C: pop %ecx; 0x0044929D: ret.
    TaskType_Clear_401A20(g_57c20c);
}

} // extern "C"

#undef NATIVE_DECL
