// Native: Task_ClearClassTypeAndDeleteList_4201B0 (behavior-derived placeholder name; retail symbol unknown; the form is reused deliberately from the byte-identical sibling core/native_4192b0.cpp - same 37 bytes, same 9 instructions, same two callees - and the VA suffix disambiguates). retail VA 0x004201B0-0x004201D4 (37 bytes, 1 block, 0 edges, 9 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004201B0 plus capstone 5.0.7 aligned from the verified entry; both agree on 9 instructions. Exact bytes a1 e4 bb 57 00 50 e8 75 16 0a 00 66 8b 0d bc b6 53 00 51 e8 58 18 fe ff 83 c4 08 66 c7 05 bc b6 53 00 81 01 c3 = `mov eax,[0x0057BBE4]` / `push %eax` / `call 0x004C1830` / `mov cx,[0x0053B6BC]` / `push %ecx` / `call 0x00401A20` / `add $0x8,%esp` / `mov word [0x0053B6BC],0x0181` / `ret`. Both call displacements resolve as v+5+rel32 over the raw bytes: 0x004201BB + 0x000A1675 = 0x004C1830 (displacement at file offset 0x4201B7) and 0x004201C8 + 0xFFFE1858 = 0x00401A20 (at 0x4201C4). The ret is at 0x004201D4, the bytes 0x004201D5-0x004201DC are eight 0x90 alignment nops (not part of the routine), and the extent is 37 bytes. Stack replay over the 9 instructions returns esp to its entry value at the ret with zero slack: two argument dwords pushed (0x004201B5, 0x004201C2) and one `add $0x8,%esp` at 0x004201C8, and 0x004C1830 cleans only its own internal push, so the container push is still live at the 0x00401A20 call. Caller: 0x00402C7C, a direct `E8` call inside the startup guard 0x00402930 (every E8 in .text decoded as v+5+rel32; 0x004201B0 has exactly one call site in the whole image, and it is inside the guard's ASCENDING teardown block that begins at 0x00402C40).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the SlideBar registrar core/native_41f340.cpp: it deletes the task-parameter container whose handle that registrar stored at 0x0057BBE4 through 0x004C1830 (QTaskList_Delete, core/native_4c1830.cpp), then clears the registered class type whose 16-bit id that registrar stored at 0x0053B6BC through 0x00401A20 (TaskType_Clear, core/native_401a20.cpp), and finally writes the 0x0181 "not registered" sentinel back into the id slot. The `add $0x8,%esp` is the cdecl cleanup for the two arguments this body pushed and is emitted by the compiler for the two calls below rather than written by hand.
// MEASURED ABSENCE, NOT AN OMISSION: retail does NOT clear the container slot 0x0057BBE4 here. Only the id slot is reset. Zeroing the container handle would be a behaviour retail does not perform, so this file does not do it.
// Status/limits: TRANSCRIBED; LINKABLE (two undefined symbols, 0x004C1830 and 0x00401A20, both already tree natives, so this file adds no new dependency); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. PAIRING EVIDENCE, NOT AN ASSUMPTION: the two slots this body touches are exactly the two slots core/native_41f340.cpp writes - it defines g_53b6bc (0x0053B6BC, std::uint16_t, image word 0x0181 at the file bytes immediately before the "SlideBar" literal at 0x0053B6C0) and g_57bbe4 (0x0057BBE4, std::uint32_t, above the file-backed end of .data, image value 0) - and its provenance records the same two addresses. This file only reads and rewrites them, so they are declared extern and defined nowhere: exactly one definition per retail address. LINKAGE IS PART OF THAT EVIDENCE: native_41f340.cpp defines both globals INSIDE its `extern "C"` block, so they are C-linkage symbols and they are declared here inside `extern "C"` as well; declaring them inside a namespace would silently create a second, distinct symbol (the two-variable trap). THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its container was deleted, and no build or link check can catch that. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family, and it is the first of the guard's 0x0042xxxx teardowns.
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

// 0x0053B6BC and 0x0057BBE4 are DEFINED by the SlideBar registrar engine/src/retail/core/native_41f340.cpp, inside
// that file's `extern "C"` block, so these are C-linkage symbols. This teardown only reads and rewrites them.
extern std::uint16_t g_53b6bc; // 0x0053B6BC: the SlideBar class id (image value 0x0181)
extern std::uint32_t g_57bbe4; // 0x0057BBE4: the SlideBar task-parameter container handle (image value 0)

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp); takes its one argument cdecl-style and
// cleans up only its own internal push, which is why the caller's push at 0x004201B5 survives to the `add esp,8`.
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_4201B0()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_4201B0", "0x004201B0");
    // 0x004201B0: `mov eax,[0x0057BBE4]`; 0x004201B5: `push %eax`; 0x004201B6: `call 0x004C1830`.
    QTaskList_Delete_4C1830(g_57bbe4);

    // 0x004201BB: `mov cx,[0x0053B6BC]`; 0x004201C2: `push %ecx`; 0x004201C3: `call 0x00401A20`.
    TaskType_Clear_401A20(g_53b6bc);

    // 0x004201CB: `mov word [0x0053B6BC],0x0181` - restore the "not registered" sentinel. The 0x004201C8 `add $0x8,%esp`
    // between the call and this store is the cdecl cleanup for the two pushed arguments and is emitted by the compiler.
    // 0x004201D4: `ret`. Retail does NOT write 0x0057BBE4 here.
    g_53b6bc = 0x0181U;
}

} // extern "C"
