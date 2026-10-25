// Native: Task_ClearClassTypeAndDeleteList_42BC00 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x0042BC00-0x0042BC1C (28 bytes, 1 block, 0 edges, 8 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x0042BC00 plus capstone 5.0.7 aligned from the verified entry; both agree on 8 instructions. Exact bytes 66 a1 b8 bc 57 00 50 e8 14 5e fd ff 8b 0d bc bc 57 00 51 e8 18 5c 09 00 83 c4 08 c3 = `mov ax,[0x0057BCB8]` / `push %eax` / `call 0x00401A20` / `mov ecx,[0x0057BCBC]` / `push %ecx` / `call 0x004C1830` / `add $0x8,%esp` / `ret`. Both call displacements resolve as v+5+rel32: 0x0042BC0C + 0xFFFD5E14 = 0x00401A20; 0x0042BC18 + 0x00095C18 = 0x004C1830. The ret is at 0x0042BC1B and the extent is 28 bytes. Call census from the whole image's 0xE8 enumeration: 0x00401A20 at 0x0042BC07, 0x004C1830 at 0x0042BC13. Stack replay over the 8 instructions returns esp to its entry value at the ret with zero slack: two argument dwords pushed and one `add $0x8,%esp` at 0x0042BC18. Caller: the startup guard 0x00402930's ASCENDING teardown block from 0x00402C40, and this body sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x0042ACC0, the "Train" registrar, 870 bytes / 246 instructions, owned by native_42acc0.cpp).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It clears the registered class type whose 16-bit id the Train registrar stored at 0x0057BCB8 through 0x00401A20, then deletes the task-parameter container whose handle that registrar stored at 0x0057BCBC through 0x004C1830 (QTaskList_Delete, core/native_4c1830.cpp), and it WRITES NO 0x0181 RESET - this body sits above 0x00424790, the last teardown in the band that writes the sentinel, so its absent reset is a second independent confirmation of the reset law. THE TWO SLOTS ARE THE REGISTRAR OWNER'S, NOT THIS FILE'S: 0x0057BCB8 is a std::uint16_t written once at 0x0042ACE7 with `mov word [0x57bcb8],ax` and every one of its 17 accesses is `66`-prefixed, and 0x0057BCBC is a std::uint32_t written at 0x0042AE91 with `mov dword [0x57bcbc],eax` across its 11 accesses; both are defined by native_42acc0.cpp WITH C LINKAGE by its owner's explicit decision, so both are declared inside extern "C" here.
// Status/limits: TRANSCRIBED; NOT LINKABLE YET, AND FOR A MEASURED REASON: both of its globals are defined by core/native_42acc0.cpp, which its owner has not written yet, so this file stays fail-closed until that file lands - the declarations here are the linkage its owner fixed, not a guess. STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The declaration shape matters here in the same way it does in the rest of the band: the two-variable trap has a linkage axis as well as a namespace axis, and the owner's file decides the linkage - this file reads the owner's decision rather than copying a sibling's. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint16_t g_57bcb8; // 0x0057BCB8: the 16-bit class id of the Train class, owned by core/native_42acc0.cpp
extern std::uint32_t g_57bcbc; // 0x0057BCBC: the container handle of the Train class, owned by core/native_42acc0.cpp

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks to the low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp).
extern void QTaskList_Delete_4C1830(std::uint32_t list);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_42BC00()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_42BC00", "0x0042BC00");
    // 0x0042BC00: `mov ax,[0x57bcb8]` / 0x0042BC06: `push eax` / 0x0042BC07: `call 0x00401A20`
    TaskType_Clear_401A20(g_57bcb8);
    // 0x0042BC0C: `mov ecx,[0x57bcbc]` / 0x0042BC12: `push ecx` / 0x0042BC13: `call 0x004C1830`
    QTaskList_Delete_4C1830(g_57bcbc);
    // 0x0042BC18: `add esp,0x8` is the cdecl cleanup for the two argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. No 0x0181 sentinel write follows, and that absence is retail's.
    // 0x0042BC1B: `ret`
}

} // extern "C"
