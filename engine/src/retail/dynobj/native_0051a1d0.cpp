// Native: SplinePathDynCubeObj_Init (placeholder until VERIFIED); retail VA 0x0051A1D0-0x0051A22F (96 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 565a08dd85a44918b2a3615962c144408431c2c43bb1256e276ef27ed1aa028b.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 5c 87 54 00 ...); calls 0x00401900, 0x00401400 (x2), 0x00401530, 0x004F1A60; string "SplinePathDynCubeObj" at 0x0054EDA4.
// Behavior/CFG: parent = word at 0x0054875C (the DynCubeObj id written by 0x004E5370); registers "SplinePathDynCubeObj" (0x200 bytes) -> observed id 117, stores the id at 0x0054ED4E, links phase 0 (0x0051A240) through 0x00401400, phase 2 (0x004D3710) through 0x00401530, reads the context byte through 0x004F1A60, and links that context to 0x0051A230 through 0x00401400, returning the last result. Single block.
// Status/limits: TRANSCRIBED (static only); handlers 0x0051A240/0x0051A230 are passed as retail-VA literals; the parent word is defined by DynCubeObj_Init in this batch; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54875c;  // retail 0x0054875C: DynCubeObj class id, defined by DynCubeObj_Init
std::uint16_t g_54ed4e;  // retail 0x0054ED4E: SplinePathDynCubeObj class id, written here

std::int32_t SplinePathDynCubeObj_Init()
{
    g_54ed4e = TaskType_Register("SplinePathDynCubeObj", 0x200U, g_54875c, 0U); // 0x0051A1E5: call 0x401900
    QTask_LinkQTaskEventHandler(g_54ed4e, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0051A240)); // 0x0051A1F8
    QTask_LinkQTaskEventHandlerNoContextFn(g_54ed4e, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710)); // 0x0051A20C
    std::uint32_t const context = Script_PhaseIdGetE4_4F1A60();                        // 0x0051A219: call 0x4F1A60
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandler(g_54ed4e, context, reinterpret_cast<igi::retail::core::FrameHandler>(0x0051A230))); // tail link
}
} // extern "C"
