#include "igi/retail/core/class_registration.hpp"

#include "igi/retail/core/flow.hpp"
#include "igi/retail/core/game_flow.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

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
    throw std::logic_error(
        std::string("QTask_RegisterClasses (0x00402930): the ") + registrar +
        " registrar is outside the MVP scope and is not transcribed, so the class ids after it "
        "do not match the retail's");
}

} // namespace

extern "C" {

// Native: QTask_RegisterClasses; retail VA 0x00402930.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402930..0x00402A40.
// Behavior/CFG: the retail's class registration sequence. It calls the class registrars in order: the AI graph at 0x00508F80, the AI detection at 0x00489560, the material defines at 0x00407D60, the level flow at 0x00488BB0, the game options at 0x00402FF0, the Level and Game classes at 0x00415530, the task list at 0x004C1780, the Flow class at 0x00401FC0, and the registrars after it. The class ids the handler table is indexed by follow this order.
// Status/limits: STRUCTURALLY_VERIFIED for the sequence's order and the two registrars the MVP demo needs. The registrars before the Game class are outside the MVP scope, so the port reports them instead of registering fewer classes and silently shifting every id that follows.
void IGI_RETAIL_CDECL QTask_RegisterClasses()
{
    ReportNotTranscribed("AI graph");
    ReportNotTranscribed("AI detection");
    ReportNotTranscribed("material defines");
    ReportNotTranscribed("level flow");
    ReportNotTranscribed("game options");
    RegisterGameClasses();
    ReportNotTranscribed("task list");
    RegisterFlowClass();
}

} // extern "C"

#undef IGI_RETAIL_CDECL
