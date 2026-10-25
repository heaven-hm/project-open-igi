// Native: Task_DispatchPhase1_454EE0 (behavior-derived placeholder name; retail symbol unknown); retail VA 0x00454EE0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x00454EE0-0x00454EF9 (26 bytes, 1 block, 0 edges, 8 instructions: e8 / 8b4c2404 / 25ffff0000 / 51 / 8b0485e070a900 / ffd0 / 59 / c3; the next byte 0x00454EFA is nop padding).
// Tools/evidence: radare2 6.2.0 af/afbj/pdf and the raw bytes decode as `call 0x00460BE0` (Core_GetGlobal16_460BE0), `mov ecx,[esp+4]` (the first argument), `and eax,0xFFFF`, `push ecx`, `mov eax,[0x00A970E0+eax*4]`, `call eax`, `pop ecx`, `ret`. 0x00A970E0 is the handler table's phase-1 base: task_types.hpp declares `QTask_HandlerTable[]` (line 89) and `task_phase_stride = 0x180U` (line 58), and 0x00401400/0x00401530 store handlers at 0x00A96AE0 + index*4 with index = class_id + phase*384 (task_types.cpp), so 0x00A96AE0 + 384*4 = 0x00A970E0 is phase 1 indexed by the type id. 0x0040FE30 links this routine as the HumanPlayer event 1 handler at 0x0040FE85.
// Behavior/CFG: reads the current 16-bit type id from 0x00460BE0, masks it to 16 bits, loads the phase-1 handler for that type out of QTask_HandlerTable and forwards the single argument to it; one block, no branch, no loop, one indirect call, no memory write.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The indirect call goes through the table that the link natives fill, so the routine only does anything once 0x00401400 or 0x00401530 has stored a handler for the type; nothing in this body establishes what the handler receives beyond the forwarded argument, and the table slot is not checked for zero in retail either. The table is referenced by name from task_types.hpp rather than declared here, so 0x00A96AE0 keeps one owner. Domain: core/, because it is a handler of the 0x00402930 startup registrar 0x0040FE30.
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

std::uint16_t Core_GetGlobal16_460BE0(void);   // 0x00460BE0: argument-less word getter (current type id)

void NATIVE_DECL Task_DispatchPhase1_454EE0(void* argument)
{
    NATIVE_LOG_ENTER("Task_DispatchPhase1_454EE0", "");
    const std::uint16_t type_id = static_cast<std::uint16_t>(Core_GetGlobal16_460BE0()); // 0x00454EE0: call 0x460BE0; 0x00454EE9: and eax,0xffff
    ::QTask_HandlerTable[igi::retail::core::task_phase_stride + type_id](argument);      // 0x00454EEF: mov eax,[0x00A970E0+eax*4]; 0x00454EF6: call eax
}

} // extern "C"

#undef NATIVE_DECL
