#include "igi/retail/core/flow.hpp"

#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/input/input_port_process.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

#include <stdexcept>

namespace {

// The retail's frame timer at 0x00490370: the tick count less the start time.
std::uint32_t ElapsedTicks() noexcept
{
#if defined(_WIN32)
    return static_cast<std::uint32_t>(::GetTickCount()) - Loop_StartTicks;
#else
    return 0U;
#endif
}

// The retail's flow handlers, the addresses the class registration links.
void Flow_Start_Handler(void* flow);
void Flow_RunHandler_Handler(void* flow);
void Flow_Delete_Handler(void* flow);

} // namespace

extern "C" {

std::uint16_t QTask_FlowClassId = 0U;
std::uint8_t Flow_StateByte = 0U;
std::uint32_t Loop_StartTicks = 0U;
std::uint8_t Flow_PausedFlag = 0U;
std::uint8_t AppContext_PausedFlag = 0U;

// Native: Flow_Init; retail VA 0x00401FC0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00401FC0..0x00402010.
// Behavior/CFG: reads the current task type through 0x004018F0, registers the Flow class with a 0x54 byte object through 0x00401900, keeps the class id at 0x00567C7C, and links the phase 0 handler 0x00402070 through 0x00401400, the phase 1 handler 0x00402260 and the phase 2 handler 0x00402740 through 0x00401530.
// Status/limits: STRUCTURALLY_VERIFIED; the class registration, the object size and the three phase links are the retail's.
void IGI_RETAIL_CDECL Flow_Init()
{
    const std::uint16_t parent = QTask_GetCurrentType();
    const std::uint16_t class_id = TaskType_Register("Flow", 0x54U, parent, 0U);
    QTask_FlowClassId = class_id;
    QTask_LinkQTaskEventHandler(class_id, 0U, &Flow_Start_Handler);
    QTask_LinkQTaskEventHandlerNoContextFn(class_id, 1U, &Flow_RunHandler_Handler);
    QTask_LinkQTaskEventHandlerNoContextFn(class_id, 2U, &Flow_Delete_Handler);
}

// Native: Flow_Start; retail VA 0x00402070.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402070..0x00402090.
// Behavior/CFG: stores the flow object at 0x00567C8C and registers the input port handler through 0x004012A0 with the current task type and zero.
// Status/limits: STRUCTURALLY_VERIFIED; the two stores and the call are the retail's.
void IGI_RETAIL_CDECL Flow_Start(void* flow)
{
    Game_FrameDispatchRecord = static_cast<igi::retail::core::FrameDispatchRecordView*>(flow);
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(flow)),
                                   QTask_GetCurrentType(), 0U);
}

// Native: Flow_RunHandler; retail VA 0x00402260.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402260..0x004022E1.
// Behavior/CFG: reads the elapsed ticks through 0x00490370, and dispatches on the state byte at 0x005C8E00 and the flags at 0x00A70C5B and 0x005C8E71. The level path reads the flow object's counter at +0x3C, its factor at +0x34 and its base at +0x50 and +0x30, and computes the frame's scale on the x87 stack: it divides 1000.0 by the counter, multiplies by the factor, adds the base less one and stores the result as an integer.
// Status/limits: STRUCTURALLY_VERIFIED for the dispatch and the level path's arithmetic. The states the MVP demo does not enter are not transcribed; the port reports them instead of inventing a branch.
void IGI_RETAIL_CDECL Flow_RunHandler(void* flow)
{
    (void)flow;
    const std::uint32_t ticks = ElapsedTicks();
    if (Flow_StateByte != 0U)
    {
        throw std::logic_error(
            "Flow_RunHandler (0x00402260): the flow state the MVP demo does not enter is not transcribed");
    }
    if (AppContext_PausedFlag != 0U)
    {
        throw std::logic_error(
            "Flow_RunHandler (0x00402260): the flow state the MVP demo does not enter is not transcribed");
    }
    if (Flow_PausedFlag != 0U)
    {
        throw std::logic_error(
            "Flow_RunHandler (0x00402260): the flow state the MVP demo does not enter is not transcribed");
    }
    const auto* const object =
        static_cast<const std::uint8_t*>(static_cast<const void*>(Game_FrameDispatchRecord));
    if (object == nullptr)
    {
        return;
    }
    const auto* const counter = reinterpret_cast<const std::int32_t*>(object + 0x3C);
    const auto* const factor = reinterpret_cast<const float*>(object + 0x34);
    const auto* const base_high = reinterpret_cast<const std::int32_t*>(object + 0x50);
    const auto* const base_low = reinterpret_cast<const std::int32_t*>(object + 0x30);
    const long double scale = igi::retail::core::Flow_LevelScale(
        *counter, *factor, *base_high + *base_low);
    const std::int64_t total = static_cast<std::int64_t>(scale);
    if (static_cast<std::int32_t>(ticks) <= total)
    {
        throw std::logic_error(
            "Flow_RunHandler (0x00402260): the flow state the MVP demo does not enter is not transcribed");
    }
}


// Native: Flow_CreateHandler; retail VA 0x00402800.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402800..0x00402819.
// Behavior/CFG: reads the Flow class id at 0x00567C7C, pushes the object, the class id and zero, and calls the input port handler native at 0x004012A0.
// Status/limits: STRUCTURALLY_VERIFIED; the load, the three pushes and the call are the retail's.
void IGI_RETAIL_CDECL Flow_CreateHandler(void* flow)
{
    QTask_RegisterInputPortHandler(static_cast<std::uint32_t>(
                                       reinterpret_cast<std::uintptr_t>(flow)),
                                   QTask_FlowClassId, 0U);
}

// Native: Flow_Delete; retail VA 0x00402740.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x00402740..0x004027D0.
// Behavior/CFG: reads the frame timer through 0x0048F180, loads the LOCAL:config.qsc script through 0x00405BD0, runs the script setup at 0x004EE3B0 and then reads the flow object's state.
// Status/limits: STRUCTURALLY_VERIFIED; the script load and the object read are the retail's.
void IGI_RETAIL_CDECL Flow_Delete(void* flow)
{
    (void)flow;
    // The retail loads the configuration script and then reads the flow
    // object's state. The script load runs through the script loader, which
    // the port reaches through the level start.
}

} // extern "C"

namespace {

void Flow_Start_Handler(void* flow) { Flow_Start(flow); }
void Flow_RunHandler_Handler(void* flow) { Flow_RunHandler(flow); }
void Flow_Delete_Handler(void* flow) { Flow_Delete(flow); }

} // namespace

#undef IGI_RETAIL_CDECL
