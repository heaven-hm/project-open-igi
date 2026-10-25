// Native: Task_ClearClassTypeAndDeleteList_42C4E0 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x0042C4E0-0x0042C4FC (28 bytes, 1 block, 0 edges, 8 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x0042C4E0 plus capstone 5.0.7 aligned from the verified entry; both agree on 8 instructions. Exact bytes 66 a1 f8 bc 57 00 50 e8 34 55 fd ff 8b 0d 64 23 bc 00 51 e8 e8 b3 06 00 83 c4 08 c3 = `mov ax,[0x0057BCF8]` / `push %eax` / `call 0x00401A20` / `mov ecx,[0x00BC2364]` / `push %ecx` / `call 0x004978E0` / `add $0x8,%esp` / `ret`. Both call displacements resolve as v+5+rel32: 0x0042C4EC + 0xFFFD5534 = 0x00401A20; 0x0042C4F8 + 0x0006B3E8 = 0x004978E0. The ret is at 0x0042C4FB and the extent is 28 bytes. Call census from the whole image's 0xE8 enumeration: 0x00401A20 at 0x0042C4E7, 0x004978E0 at 0x0042C4F3. Stack replay over the 8 instructions returns esp to its entry value at the ret with zero slack: two argument dwords pushed and one `add $0x8,%esp` at 0x0042C4F8. Caller: the startup guard 0x00402930's ASCENDING teardown block from 0x00402C40, and this body sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x0042BEB0, the "Afterburner" registrar).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It clears the registered class type whose 16-bit id the Afterburner registrar stored at 0x0057BCF8 through 0x00401A20, then releases the task-parameter container whose handle that registrar stored at 0x00BC2364 through 0x004978E0 (sys/native_4978e0.cpp), and it WRITES NO 0x0181 RESET. BOTH ORDER AND OMISSION ARE MEASURED, NOT INFERRED: the type is cleared BEFORE the container is released, which is the opposite of the shape-A teardowns lower in the band (0x00420C90 and its siblings delete the container first), and the missing sentinel write is retail's - this body sits ABOVE 0x00424790, the last teardown that writes it, which is the second independent confirmation of the band's reset law. THE CONTAINER IS RELEASED THROUGH 0x004978E0, NOT THROUGH 0x004C1830: the two are different routines (0x004978E0 forwards to 0x004B0EB0, which zeroes a byte in a slot table; 0x004C1830 deletes a task list), and this body's choice of one over the other is a real distinction recorded rather than normalised into the majority shape.
// Status/limits: TRANSCRIBED; LINKABLE (both callees 0x00401A20 and 0x004978E0 and both globals are tree natives); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The two slots this body touches are declared extern and defined nowhere here: exactly one definition per retail address, and both are defined with C linkage by core/native_42beb0.cpp, so they are declared inside extern "C" here rather than in a namespace (the two-variable trap has a linkage axis as well as a namespace axis). Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint16_t g_57bcf8; // 0x0057BCF8: the 16-bit class id of the Afterburner class, owned by core/native_42beb0.cpp
extern std::uint32_t g_bc2364; // 0x00BC2364: the container handle of the Afterburner class, owned by core/native_42beb0.cpp

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks to the low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

// 0x004978E0: the twelve-byte forwarder that clears a slot byte (engine/src/retail/sys/native_4978e0.cpp).
extern void NATIVE_DECL Sys_ClearSlotByte_4978E0(std::int32_t index);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_42C4E0()
{
    // 0x0042C4E0: `mov ax,[0x57bcf8]` / 0x0042C4E6: `push eax` / 0x0042C4E7: `call 0x00401A20`
    TaskType_Clear_401A20(g_57bcf8);
    // 0x0042C4EC: `mov ecx,[0xbc2364]` / 0x0042C4F2: `push ecx` / 0x0042C4F3: `call 0x004978E0`. The container handle is
    // a std::uint32_t read as retail's dword; the callee takes std::int32_t and the 32-bit value is passed unchanged.
    Sys_ClearSlotByte_4978E0(static_cast<std::int32_t>(g_bc2364));
    // 0x0042C4F8: `add esp,0x8` is the cdecl cleanup for the two argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. No 0x0181 sentinel write follows, and that absence is retail's.
    // 0x0042C4FB: `ret`
}

} // extern "C"
