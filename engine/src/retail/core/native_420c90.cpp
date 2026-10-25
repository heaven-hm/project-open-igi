// Native: Task_ClearClassTypeAndDeleteList_420C90 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x00420C90-0x00420CB4 (37 bytes, 1 block, 0 edges, 9 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x00420C90 plus capstone 5.0.7 aligned from the verified entry; both agree on 9 instructions. Exact bytes a1 ec bb 57 00 50 e8 95 0b 0a 00 66 8b 0d 00 b7 53 00 51 e8 78 0d fe ff 83 c4 08 66 c7 05 00 b7 53 00 81 01 c3. The ret is at 0x00420CB4, the 11 bytes after it are 0x90 alignment nops (not part of the routine), and the extent is 37 bytes. Callees in retail order: 0x401a20, 0x4c1830. Stack replay returns esp to its entry value at the ret with zero slack. Caller: this body is reached from the startup guard 0x00402930's ASCENDING teardown block that begins at 0x00402C40, and it sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one.
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the MenuTextConditional registrar core/native_420300.cpp: it deletes the task-parameter container whose handle the registrar stored at 0x0057BBEC through 0x004C1830 (QTaskList_Delete, core/native_4c1830.cpp), then clears the registered class type whose 16-bit id the registrar stored at 0x0053B700 through 0x00401A20 (TaskType_Clear, core/native_401a20.cpp), and finally writes the 0x0181 "not registered" sentinel back into the id slot. The callee cleanup at the end is the cdecl cleanup for the argument dwords this body pushed and is emitted by the compiler rather than written by hand.
// Status/limits: TRANSCRIBED; LINKABLE (every undefined symbol it names is already a tree native, so it adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: 0x0057BBEC (the container handle) and 0x0053B700 (the 16-bit class id) - the registrar's own provenance records the same addresses, and the two globals match that registrar exactly. This file only reads and rewrites them, so they are declared extern and defined nowhere: exactly one definition per retail address. LINKAGE IS PART OF THAT EVIDENCE: the owning file defines these slots inside a namespace block, and they are declared here with the same linkage, because declaring them under the other linkage would silently create a second, distinct symbol with no link error (the two-variable trap). THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its container was deleted, and no build or link check can catch that. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.
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

namespace igi::retail::core {

extern std::uint32_t g_57bbec; // 0x0057BBEC: the container handle of the MenuTextConditional class, owned by core/native_420300.cpp
extern std::uint16_t g_53b700; // 0x0053B700: the 16-bit class id of the MenuTextConditional class, owned by core/native_420300.cpp

} // namespace igi::retail::core

extern "C" {

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp).
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_420C90()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_420C90", "0x00420C90");
    // 0x00420C90: `mov eax, dword ptr [0x57bbec]`
    // 0x00420C95: `push eax`
    // 0x00420C96: `call 0x4c1830`
    QTaskList_Delete_4C1830(igi::retail::core::g_57bbec);
    // 0x00420C9B: `mov cx, word ptr [0x53b700]`
    // 0x00420CA2: `push ecx`
    // 0x00420CA3: `call 0x401a20`
    TaskType_Clear_401A20(igi::retail::core::g_53b700);
    // 0x00420CA8: `add esp, 8`
    // 0x00420CAB: `mov word ptr [0x53b700], 0x181`
    igi::retail::core::g_53b700 = 0x0181U;
    // 0x00420CB4: `ret `
}

} // extern "C"
