// Native: Task_ClearClassType_424790 (behavior-derived placeholder name; retail symbol unknown; the tree's convention omits the "AndDeleteList" half of the sibling name because this body has no container to delete, and the VA suffix disambiguates). retail VA 0x00424790-0x00424848 (185 bytes, 1 block, 0 edges, 42 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x00424790 plus capstone 5.0.7 aligned from the verified entry; both agree on 42 instructions. Exact bytes 68 dc b9 53 00 e8 86 41 09 00 68 c8 b9 53 00 e8 7c 41 09 00 68 b0 b9 53 00 e8 72 41 09 00 68 98 b9 53 00 e8 68 41 09 00 68 80 b9 53 00 e8 5e 41 09 00 68 6c b9 53 00 e8 54 41 09 00 68 54 b9 53 00 e8 4a 41 09 00 68 3c b9 53 00 e8 40 41 09 00 68 24 b9 53 00 e8 36 41 09 00 a1 10 b9 53 00 50 e8 eb d0 fd ff 8b 0d 14 b9 53 00 51 e8 df d0 fd ff 8b 15 18 b9 53 00 52 e8 d3 d0 fd ff a1 1c b9 53 00 50 e8 c8 d0 fd ff 8a 0d 20 b9 53 00 51 e8 9c d0 fd ff 8a 15 21 b9 53 00 52 e8 90 d0 fd ff 66 a1 0c b9 53 00 50 e8 e4 d1 fd ff 83 c4 40 66 c7 05 0c b9 53 00 81 01 c3. Call census from the whole image's 0xE8 enumeration: 0x004B8920 at 0x00424795, 0x0042479F, 0x004247A9, 0x004247B3, 0x004247BD, 0x004247C7, 0x004247D1, 0x004247DB, 0x004247E5; 0x004018E0 at 0x004247F0, 0x004247FC, 0x00424808, 0x00424813; 0x004018C0 at 0x0042481F, 0x0042482B; 0x00401A20 at 0x00424837. Stack replay over the 42 instructions returns esp to its entry value at the ret with zero slack: sixteen argument dwords pushed and one `add $0x40,%esp` at 0x0042483C. Caller: the startup guard 0x00402930's ASCENDING teardown block from 0x00402C40, and this body sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x00423FC0).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register, and the largest teardown in the band. It is the teardown counterpart of the MenuWindow registrar core/native_423fc0.cpp and does four things in retail's order: (1) removes the nine script symbols that registrar published, through 0x004B8920 (script/native_4b8920.cpp); (2) clears four dword event-table slots through the 0x004018E0 forwarder; (3) clears two BYTE slots through the 0x004018C0 body itself; (4) clears the registered class type whose 16-bit id the registrar stored at 0x0053B90C, through 0x00401A20, then writes the 0x0181 "not registered" sentinel back. THE NINE NAMES ARE THE PAIRING PROOF AND THEY ARE EXACT: the registrar registered exactly these nine LINEUPMODE_* strings in this order (its provenance records the same nine addresses), so this teardown cannot belong to a different class. THE `mov word [..],0x0181` IS LOAD-BEARING. TWO DIFFERENT FORWARDING ROUTINES ARE USED FOR THE SAME OPERATION AND THAT IS RETAIL'S, NOT A TYPO: the four dword slots go through the twelve-byte forwarder 0x004018E0 while the two byte slots call 0x004018C0 directly; both bodies clear the same table entry and both mask the index to 0xff, so the two are behaviorally identical, and they are transcribed as two separate natives because retail has two separate routines (rule 13). The byte slots are loaded with `mov cl,[0x53b920]` / `mov dl,[0x53b921]`, which pushes a register whose upper bytes are stale; the callee masks to the low byte, so passing the zero-extended std::uint8_t here is behaviorally identical and is recorded rather than silently widened.
// Status/limits: TRANSCRIBED; the link check reports every symbol it names as defined in tree, but the final program link still needs the 0x004BAB30 chain behind 0x004B8920 (see that file); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The nine string addresses are retail pointers into .data and are written here as byte-identical literals re-read from the image, which is the only permitted substitution (rule 10). All seven slots this body touches are declared extern and defined nowhere here: exactly one definition per retail address, and every owner defines its slot with C linkage, so all seven are declared inside extern "C" (the two-variable trap has a linkage axis as well as a namespace axis). Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint16_t g_53b90c; // 0x0053B90C: the 16-bit class id of the MenuWindow class, owned by sys/native_424850.cpp
extern std::uint32_t g_53b910; // 0x0053B910: event-table slot, owned by core/native_423fc0.cpp
extern std::uint32_t g_53b914; // 0x0053B914: event-table slot, owned by sys/native_424870.cpp
extern std::uint32_t g_53b918; // 0x0053B918: event-table slot, owned by sys/native_424880.cpp
extern std::uint32_t g_53b91c; // 0x0053B91C: event-table slot, owned by sys/native_424890.cpp
extern std::uint8_t g_53b920;  // 0x0053B920: event-table byte slot, owned by sys/native_424b80.cpp
extern std::uint8_t g_53b921;  // 0x0053B921: event-table byte slot, owned by core/native_423fc0.cpp

// 0x004B8920: removes a script symbol by name (engine/src/retail/script/native_4b8920.cpp).
extern void NATIVE_DECL Script_QHashSymbolRemove_4B8920(char const* name);

// 0x004018E0: the twelve-byte forwarder onto the slot clear (engine/src/retail/core/native_4018e0.cpp).
extern void NATIVE_DECL TaskEventTable_ClearSlot_4018E0(std::uint32_t const index);

// 0x004018C0: the slot clear itself (engine/src/retail/core/native_4018c0.cpp); masks its argument to 0xff.
void TaskEventTable_ClearSlot(std::uint32_t const index);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks to the low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassType_424790()
{
    // The nine symbol removals, each `push $<string>` followed by `call 0x004B8920`, in retail's order.
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_NONE");          // 0x00424790: push 0x0053B9DC; 0x00424795: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_VERT_TOP");      // 0x0042479A: push 0x0053B9C8; 0x0042479F: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_VERT_CENTRE");   // 0x004247A4: push 0x0053B9B0; 0x004247A9: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_VERT_BOTTOM");   // 0x004247AE: push 0x0053B998; 0x004247B3: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_VERT_FILLOUT");  // 0x004247B8: push 0x0053B980; 0x004247BD: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_HOR_LEFT");      // 0x004247C2: push 0x0053B96C; 0x004247C7: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_HOR_CENTRE");    // 0x004247CC: push 0x0053B954; 0x004247D1: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_HOR_RIGHT");     // 0x004247D6: push 0x0053B93C; 0x004247DB: call
    Script_QHashSymbolRemove_4B8920("LINEUPMODE_HOR_FILLOUT");   // 0x004247E0: push 0x0053B924; 0x004247E5: call

    // The four dword slots, each `mov eax/ecx/edx,[slot]` + `push` + `call 0x004018E0`.
    TaskEventTable_ClearSlot_4018E0(g_53b910);                   // 0x004247EA/0x004247EF/0x004247F0
    TaskEventTable_ClearSlot_4018E0(g_53b914);                   // 0x004247F5/0x004247FB/0x004247FC
    TaskEventTable_ClearSlot_4018E0(g_53b918);                   // 0x00424801/0x00424807/0x00424808
    TaskEventTable_ClearSlot_4018E0(g_53b91c);                   // 0x0042480D/0x00424812/0x00424813

    // The two byte slots, each `mov cl/dl,[slot]` + `push` + `call 0x004018C0` - the other routine, called directly.
    TaskEventTable_ClearSlot(g_53b920);                          // 0x00424818/0x0042481E/0x0042481F
    TaskEventTable_ClearSlot(g_53b921);                          // 0x00424824/0x0042482A/0x0042482B

    // 0x00424830: `mov ax,[0x53b90c]` / 0x00424836: `push eax` / 0x00424837: `call 0x00401A20`
    TaskType_Clear_401A20(g_53b90c);
    // 0x0042483C: `add esp,0x40` is the cdecl cleanup for the sixteen argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. 0x0042483F: `mov word [0x53b90c],0x181` - the load-bearing reset.
    g_53b90c = 0x0181U;
    // 0x00424848: `ret`
}

} // extern "C"
