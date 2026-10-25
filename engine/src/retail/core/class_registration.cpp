#include "dev_log.hpp"
#include "igi/retail/core/class_registration.hpp"

#include "igi/retail/core/flow.hpp"
#include "igi/retail/core/game_flow.hpp"
#include "igi/retail/core/native_4c1780.hpp"

// retail 0x00489560: AI-detection registration position, transcribed body.
extern "C" std::int32_t AI_Detection_Init();

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include "igi/diagnostic_log.hpp"

#include <stdexcept>
#include <string>

namespace {

// The retail's registrars the MVP demo needs. Each one registers the classes
// the retail registers, in the retail's order, so the class ids follow.
void RegisterGameClasses()
{
    Game_Init();
}

void RegisterFlowClass()
{
    Flow_Init();
}

// The retail's registrars outside the MVP scope. Their classes are not
// transcribed yet, and the port reports that rather than registering fewer
// classes and silently shifting every id that follows.
[[noreturn]] void ReportNotTranscribed(const char* registrar)
{
    IGI_DIAG("0x00402930", "fail-closed: startup init sequence not transcribed, stopping at %s", registrar);
    throw std::logic_error(
        std::string("QTask_RegisterClasses (0x00402930): the retail function at this address is the startup init "
                    "sequence, 155 ordered init/open/load calls ending in a tail jump to 0x004CEF30, not a class "
                    "registrar; it is not transcribed, 146 of its 154 distinct callees have no engine "
                    "implementation, and it stops at ") + registrar);
}

} // namespace

extern "C" {

// Native: QTask_RegisterClasses; retail VA 0x00402930.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402930..0x00402C3C (780 bytes). See
// docs/fidelity/evidence/startup-402930-call-sequence.md for the ordered call inventory, measured twice with
// agreeing results.
// Correction (evidence section 19, re-measured 2026-10-25): 0x00402930 is 780 bytes and 156 instructions of exactly
// 5 bytes each - 155 `call`s and one tail `jmp 0x004CEF30` at 0x00402C37 - one basic block, ending in that jump
// rather than a ret. It is the startup init sequence of 155 ordered calls. Of the 154 distinct callees, 8 have an
// engine implementation in this tree (0x00401FC0, 0x00415530, 0x00419760, 0x00424B90, 0x0042DAC0, 0x0042FE40,
// 0x00489560, 0x004C1780) and 146 do not; the earlier 134 figure understated the gap and is retracted. The
// per-registrar description below is the older, incomplete reading and is kept only until the native itself is
// transcribed.
// Behavior/CFG: the retail's class registration sequence. It calls the class registrars in order: the AI graph at 0x00508F80, the AI detection at 0x00489560, the material defines at 0x00407D60, the level flow at 0x00488BB0, the game options at 0x00402FF0, the Level and Game classes at 0x00415530, the task list at 0x004C1780, the Flow class at 0x00401FC0, and the registrars after it. The class ids the handler table is indexed by follow this order.
// Status/limits: ANALYZED, PARTIAL. The registrar sequence is transcribed only up to the report calls; the file
//           deliberately aborts through ReportNotTranscribed before the AI graph and the material defines, so the
//           class ids after that point cannot match retail. No CFG claim, no signature claim, and it must not count
//           toward transcribed or verified totals until the AI graph path is actually transcribed.

void NATIVE_DECL QTask_RegisterClasses()
{
    NATIVE_LOG_ENTER("QTask_RegisterClasses", "~0x00402930");
    ReportNotTranscribed("AI graph");
    // 0x489560: transcribed body (position 2); registers no classes in the retail capture.
    (void)AI_Detection_Init();
    ReportNotTranscribed("material defines");
    ReportNotTranscribed("level flow");
    ReportNotTranscribed("game options");
    RegisterGameClasses();
    // 0x004C1780: ret-only task-list registrar, transcribed; it registers nothing.
    AnimSound_LoadParameters();
    RegisterFlowClass();
}

} // extern "C"

#undef NATIVE_DECL
