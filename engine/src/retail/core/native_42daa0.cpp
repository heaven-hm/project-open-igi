// Native: Task_ClearClassTypeAndDeleteList_42DAA0 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x0042DAA0-0x0042DABC (28 bytes, 1 block, 0 edges, 8 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x0042DAA0 plus capstone 5.0.7 aligned from the verified entry; both agree on 8 instructions. Exact bytes 66 a1 10 bd 57 00 50 e8 74 3f fd ff 8b 0d 14 bd 57 00 51 e8 28 9e 06 00 83 c4 08 c3 = `mov ax,[0x0057BD10]` / `push %eax` / `call 0x00401A20` / `mov ecx,[0x0057BD14]` / `push %ecx` / `call 0x004978E0` / `add $0x8,%esp` / `ret`. Both call displacements resolve as v+5+rel32: 0x0042DAAC + 0xFFFD3F74 = 0x00401A20; 0x0042DAB8 + 0x00069E28 = 0x004978E0. The ret is at 0x0042DABB and the extent is 28 bytes. Call census from the whole image's 0xE8 enumeration: 0x00401A20 at 0x0042DAA7, 0x004978E0 at 0x0042DAB3. Stack replay over the 8 instructions returns esp to its entry value at the ret with zero slack: two argument dwords pushed and one `add $0x8,%esp` at 0x0042DAB8. Caller: the startup guard 0x00402930's ASCENDING teardown block from 0x00402C40, and this body sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x0042D2B0, the "Rotor" registrar).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register, and it is the byte-for-byte twin of 0x0042C4E0 apart from its two slots: it clears the registered class type whose 16-bit id the Rotor registrar stored at 0x0057BD10 through 0x00401A20, then releases the task-parameter container whose handle that registrar stored at 0x0057BD14 through 0x004978E0 (sys/native_4978e0.cpp), and it WRITES NO 0x0181 RESET. THE TWIN PAIR IS RECORDED RATHER THAN DEDUPLICATED: these two bodies differ in nothing but their slot addresses, yet they are two retail routines at two retail addresses and are transcribed as two natives (rule 13); what they share is a shape, not an identity, and the two slots must not be conflated - 0x0042C4E0's container handle lives in the BSS region at 0x00BC2364 while this one's lives at 0x0057BD14 in .data, which is why the same instruction text loads them with different addressing forms. The container is released through 0x004978E0, not through 0x004C1830, and the type is cleared BEFORE the container is released - both measured, both preserved.
// Status/limits: TRANSCRIBED; LINKABLE (both callees 0x00401A20 and 0x004978E0 and both globals are tree natives); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The two slots this body touches are declared extern and defined nowhere here: exactly one definition per retail address, and both are defined with C linkage by core/native_42d2b0.cpp, so they are declared inside extern "C" here rather than in a namespace. Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint16_t g_57bd10; // 0x0057BD10: the 16-bit class id of the Rotor class, owned by core/native_42d2b0.cpp
extern std::uint32_t g_57bd14; // 0x0057BD14: the container handle of the Rotor class, owned by core/native_42d2b0.cpp

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks to the low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

// 0x004978E0: the twelve-byte forwarder that clears a slot byte (engine/src/retail/sys/native_4978e0.cpp).
extern void NATIVE_DECL Sys_ClearSlotByte_4978E0(std::int32_t index);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_42DAA0()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_42DAA0", "0x0042DAA0");
    // 0x0042DAA0: `mov ax,[0x57bd10]` / 0x0042DAA6: `push eax` / 0x0042DAA7: `call 0x00401A20`
    TaskType_Clear_401A20(g_57bd10);
    // 0x0042DAAC: `mov ecx,[0x57bd14]` / 0x0042DAB2: `push ecx` / 0x0042DAB3: `call 0x004978E0`. The container handle is
    // a std::uint32_t read as retail's dword; the callee takes std::int32_t and the 32-bit value is passed unchanged.
    Sys_ClearSlotByte_4978E0(static_cast<std::int32_t>(g_57bd14));
    // 0x0042DAB8: `add esp,0x8` is the cdecl cleanup for the two argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. No 0x0181 sentinel write follows, and that absence is retail's.
    // 0x0042DABB: `ret`
}

} // extern "C"
