// Native: Task_ClearClassTypeAndDeleteList_4213E0 (behavior-derived placeholder name; retail symbol unknown; the form follows the accepted siblings in this tree, with the VA suffix disambiguating). retail VA 0x004213E0-0x0042142C (77 bytes, 1 block, 0 edges, 17 instructions).
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Tools/evidence: radare2 6.2.0 `pd` at 0x004213E0 plus capstone 5.0.7 aligned from the verified entry; both agree on 17 instructions. Exact bytes a1 f4 bb 57 00 50 e8 45 04 0a 00 68 d8 b7 53 00 e8 2b 75 09 00 68 cc b7 53 00 e8 21 75 09 00 68 c0 b7 53 00 e8 17 75 09 00 68 b4 b7 53 00 e8 0d 75 09 00 66 8b 0d 70 b7 53 00 51 e8 00 06 fe ff 83 c4 18 66 c7 05 70 b7 53 00 81 01 c3 = `mov eax,[0x0057BBF4]` / `push %eax` / `call 0x004C1830` / `push $0x53B7D8` / `call 0x004B8920` / `push $0x53B7CC` / `call 0x004B8920` / `push $0x53B7C0` / `call 0x004B8920` / `push $0x53B7B4` / `call 0x004B8920` / `mov cx,[0x0053B770]` / `push %ecx` / `call 0x00401A20` / `add $0x18,%esp` / `mov word [0x0053B770],0x0181` / `ret`. The ret is at 0x0042142C and the extent is 77 bytes. Call census from the whole image's 0xE8 enumeration: 0x004C1830 at 0x004213E6; 0x004B8920 at 0x004213F0, 0x004213FA, 0x00421404, 0x0042140E; 0x00401A20 at 0x0042141B. Stack replay over the 17 instructions returns esp to its entry value at the ret with zero slack: six argument dwords pushed (0x004213E5, 0x004213EB, 0x004213F5, 0x004213FF, 0x00421409, 0x0042141A) and one `add $0x18,%esp` at 0x00421420. Caller: this body is reached from the startup guard 0x00402930's ASCENDING teardown block that begins at 0x00402C40, and it sits immediately before the next registrar in that block, which is why it tears down the class registered by the PREVIOUS one (0x00420CC0).
// Behavior/CFG: one straight-line block, no branch, no frame and no saved register. It is the teardown counterpart of the MenuText registrar core/native_420cc0.cpp: it deletes the task-parameter container whose handle that registrar stored at 0x0057BBF4 through 0x004C1830 (QTaskList_Delete, core/native_4c1830.cpp), then removes the four script symbols the registrar published - "TEXT_BLACK" (string at 0x0053B7D8), "TEXT_WHITE" (0x0053B7CC), "TEXT_GREEN" (0x0053B7C0) and "TEXT_ORANGE" (0x0053B7B4) - through 0x004B8920 (Script_QHashSymbolRemove_4B8920, script/native_4b8920.cpp), then clears the registered class type whose 16-bit id the registrar stored at 0x0053B770 through 0x00401A20 (TaskType_Clear, core/native_401a20.cpp), and finally writes the 0x0181 "not registered" sentinel back into the id slot. THE FOUR NAMES ARE THE PAIRING PROOF AND THEY ARE EXACT: the registrar registered exactly these four strings in this order (its provenance records the same four addresses), so this teardown cannot belong to a different class. THE `mov word [..],0x0181` IS LOAD-BEARING: without it the class would stay marked registered after its container was deleted, and no build or link check can catch that.
// Status/limits: TRANSCRIBED; the link check reports every symbol it names as defined in tree, but the final program link still needs the 0x004BAB30 chain behind 0x004B8920 (see that file); STRUCTURALLY_VERIFIED for the CFG only (1 block, 0 edges, no branch structure to disagree about); BUILD_VERIFIED (i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra single-file compile with zero warning lines); runtime UNOBSERVED. The string addresses are retail pointers into .data and are written here as byte-identical literals, which is the only permitted substitution (rule 10). The two slots this body touches are declared extern and defined nowhere: exactly one definition per retail address, and both owners define them at namespace scope in igi::retail::core, so they are declared inside that namespace here rather than in extern "C" (the two-variable trap has a linkage axis as well as a namespace axis). Domain: core/, because it is the teardown half of the guard 0x00402930 registrar family.

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

extern std::uint32_t g_57bbf4; // 0x0057BBF4: the container handle of the MenuText class, owned by core/native_420cc0.cpp
extern std::uint16_t g_53b770; // 0x0053B770: the 16-bit class id of the MenuText class, owned by core/native_420cc0.cpp

} // namespace igi::retail::core

extern "C" {

// 0x004C1830: deletes a task list (engine/src/retail/core/native_4c1830.cpp).
extern void QTaskList_Delete_4C1830(std::uint32_t list);

// 0x004B8920: removes a script symbol by name (engine/src/retail/script/native_4b8920.cpp).
extern void NATIVE_DECL Script_QHashSymbolRemove_4B8920(char const* name);

// 0x00401A20: clears the type slot for a class id (engine/src/retail/core/native_401a20.cpp); masks its argument to the
// low 16 bits.
extern void NATIVE_DECL TaskType_Clear_401A20(std::uint16_t class_id);

void NATIVE_DECL Task_ClearClassTypeAndDeleteList_4213E0()
{
    NATIVE_LOG_ENTER("Task_ClearClassTypeAndDeleteList_4213E0", "0x004213E0");
    // 0x004213E0: `mov eax,[0x57bbf4]` / 0x004213E5: `push eax` / 0x004213E6: `call 0x004C1830`
    QTaskList_Delete_4C1830(igi::retail::core::g_57bbf4);
    // 0x004213EB: `push 0x53b7d8` ("TEXT_BLACK") / 0x004213F0: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("TEXT_BLACK");
    // 0x004213F5: `push 0x53b7cc` ("TEXT_WHITE") / 0x004213FA: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("TEXT_WHITE");
    // 0x004213FF: `push 0x53b7c0` ("TEXT_GREEN") / 0x00421404: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("TEXT_GREEN");
    // 0x00421409: `push 0x53b7b4` ("TEXT_ORANGE") / 0x0042140E: `call 0x004B8920`
    Script_QHashSymbolRemove_4B8920("TEXT_ORANGE");
    // 0x00421413: `mov cx,[0x53b770]` / 0x0042141A: `push ecx` / 0x0042141B: `call 0x00401A20`
    TaskType_Clear_401A20(igi::retail::core::g_53b770);
    // 0x00421420: `add esp,0x18` is the cdecl cleanup for the six argument dwords this body pushed, emitted by the
    // compiler rather than written by hand. 0x00421423: `mov word [0x53b770],0x181` - the load-bearing reset.
    igi::retail::core::g_53b770 = 0x0181U;
    // 0x0042142C: `ret`
}

} // extern "C"
