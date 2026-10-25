// Native: Task_ClearClassTypeAndDeleteList_425580 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x00425580-0x0042559B (28 bytes, 1 block, 0 edges, 8 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x00425580 plus capstone 5.0.7 aligned from the verified entry; both agree on 8 instructions. Exact bytes 66 a1 68 bc 57 00 50 e8 94 c4 fd ff 8b 0d 6c bc 57 00 51 e8 98 c2 09 00 83 c4 08 c3. The ret is at 0x0042559B, the 4 bytes after it are 0x90 alignment nops (not part of the routine), and the extent is 28 bytes. Callees in retail order: 0x401a20, 0x4c1830. Stack replay returns esp to its entry value at the ret with zero slack. Caller: this body is reached from the startup guard 0x00402930's ASCENDING teardown block that begins at 0x00402C40, and it sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the ConditionalSound registrar core/native_424e80.cpp: it clears the registered class type through 0x00401A20 and deletes the task-parameter container through 0x004C1830, in that retail order: the id slot first, then the container slot. The callee cleanup at the end is the cdecl cleanup for the argument dwords this body pushed and is emitted by the compiler rather than written by hand.
// Status/limits: TRANSCRIBED; LINKABLE (every undefined symbol it names is already a tree native, so it adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: 0x0057BC6C (the container handle) and 0x0057BC68 (the 16-bit class id) - the registrar's own provenance records the same addresses, and the two globals match that registrar exactly. This file only reads and rewrites them, so they are declared extern and defined nowhere: exactly one definition per retail address. LINKAGE IS PART OF THAT EVIDENCE: the owning file defines these slots inside a namespace block, and they are declared here with the same linkage, because declaring them under the other linkage would silently create a second, distinct symbol with no link error (the two-variable trap). MEASURED ABSENCE OF THE RESET: retail does NOT write 0x0181 back into the id slot here, and this is the one shape in the family whose two operations are ordered against the reset family's order. Adding the reset would be behaviour retail does not perform. id-first Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

namespace igi::retail::core {

extern std::uint32_t g_57bc6c; // 0x0057BC6C: the container handle of the ConditionalSound class, owned by core/native_424e80.cpp
extern std::uint16_t g_57bc68; // 0x0057BC68: the 16-bit class id of the ConditionalSound class, owned by core/native_424e80.cpp

} // namespace igi::retail::core

extern "C" {

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp).
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_425580()
{
    // 0x00425580: `mov ax, word ptr [0x57bc68]`
    // 0x00425586: `push eax`
    // 0x00425587: `call 0x401a20`
    TaskType_Clear_401A20(igi::retail::core::g_57bc68);
    // 0x0042558C: `mov ecx, dword ptr [0x57bc6c]`
    // 0x00425592: `push ecx`
    // 0x00425593: `call 0x4c1830`
    QTaskList_Delete_4C1830(igi::retail::core::g_57bc6c);
    // 0x00425598: `add esp, 8`
    // 0x0042559B: `ret `
}

} // extern "C"
