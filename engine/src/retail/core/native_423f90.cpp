// Native: Task_ClearClassType_423F90 (behavior-derived placeholder name; retail symbol unknown; the tree's convention omits the "AndDeleteList" half of the sibling name because this body has no container to delete, and the VA suffix disambiguates). retail VA 0x00423F90-0x00423FBC (45 bytes, 1 block, 0 edges, 10 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x00423F90 plus capstone 5.0.7 aligned from the verified entry; both agree on 10 instructions. Exact bytes 68 9c b8 53 00 e8 86 49 09 00 68 78 b8 53 00 e8 7c 49 09 00 66 a1 74 b8 53 00 50 e8 70 da fd ff 83 c4 0c 66 c7 05 74 b8 53 00 81 01 c3 = `push $0x53B89C` / `call 0x004B8920` / `push $0x53B878` / `call 0x004B8920` / `mov ax,[0x0053B874]` / `push %eax` / `call 0x00401A20` / `add $0xc,%esp` / `mov word [0x0053B874],0x0181` / `ret`. The ret is at 0x00423FBC and the extent is 45 bytes. Call census from the whole image's 0xE8 enumeration: 0x004B8920 at 0x00423F95 and 0x00423F9F; 0x00401A20 at 0x00423FAB. Stack replay over the 10 instructions returns esp to its entry value at the ret with zero slack: three argument dwords pushed (0x00423F94, 0x00423F9E, 0x00423FAA) and one `add $0xc,%esp` at 0x00423FB0. Caller: this body is reached from the startup guard 0x00402930's ASCENDING teardown block that begins at 0x00402C40, and it sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x004230A0).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the ControlsMenu registrar core/native_4230a0.cpp: it removes the two script symbols that registrar published - "ControlsMenu_ConvertFloatFromOffset" (string at 0x0053B89C) and "ControlsMenu_ConvertOffsetFromFloat" (0x0053B878) - through 0x004B8920 (Script_QHashSymbolRemove_4B8920, script/native_4b8920.cpp), then clears the registered class type whose 16-bit id the registrar stored at 0x0053B874 through 0x00401A20 (TaskType_Clear, core/native_401a20.cpp), and finally writes the 0x0181 "not registered" sentinel back into the id slot. THE TWO NAMES ARE THE PAIRING PROOF AND THEY ARE EXACT: the registrar registered exactly these two strings in this order through 0x004B8890 (its provenance records the same two addresses), so this teardown cannot belong to a different class. THE ABSENT CONTAINER DELETE IS STRUCTURAL, NOT AN OMISSION: the registrar allocates no parameter container (no 0x004F1950, no 0x004F1A80), so there is no handle to delete, and the container-delete law of this band (a container delete is present if and only if the registrar allocated a container) holds on both sides. THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered, and no build or link check can catch that.
// Status/limits: TRANSCRIBED; the link check reports every symbol it names as defined in tree, but the final program link still needs the 0x004BAB30 chain behind 0x004B8920 (see that file); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The string addresses are retail pointers into .data and are written here as byte-identical literals, which is the only permitted substitution (rule 10). The one slot this body touches is declared extern and defined nowhere: exactly one definition per retail address, and its owner defines it at namespace scope in igi::retail::core, so it is declared inside that namespace here rather than in extern "C" (the two-variable trap has a linkage axis as well as a namespace axis). Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint16_t g_53b874; // 0x0053B874: the 16-bit class id of the ControlsMenu class, owned by core/native_4230a0.cpp

} // namespace igi::retail::core

extern "C" {

// 0x004B8920: removes a script symbol by name (engine/src/retail/script/native_4b8920.cpp).
extern void NATIVE_DECL Script_QHashSymbolRemove_4B8920(char const* name);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassType_423F90()
{
    // 0x00423F90: `push 0x53b89c` ("ControlsMenu_ConvertFloatFromOffset") / 0x00423F95: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("ControlsMenu_ConvertFloatFromOffset");
    // 0x00423F9A: `push 0x53b878` ("ControlsMenu_ConvertOffsetFromFloat") / 0x00423F9F: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("ControlsMenu_ConvertOffsetFromFloat");
    // 0x00423FA4: `mov ax,[0x53b874]` / 0x00423FAA: `push eax` / 0x00423FAB: `call 0x00401A20`
    TaskType_Clear_401A20(igi::retail::core::g_53b874);
    // 0x00423FB0: `add esp,0xc` is the cdecl cleanup for the three argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. 0x00423FB3: `mov word [0x53b874],0x181` - the load-bearing reset.
    igi::retail::core::g_53b874 = 0x0181U;
    // 0x00423FBC: `ret`
}

} // extern "C"
