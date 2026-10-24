// Native: CamTask_Init; retail VA 0x00530220.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP + radare2 6.2.0; 1 block, 2 callees (0x004018F0 QTask_GetCurrentType, 0x00401900 TaskType_Register); no branches; spec build/audit/main-0x48F530/level1-closure/20261024T024343Z/REGISTRARS-AND-4014-SPEC.md §3.3.
// Behavior/CFG: registers the "CamTask" class (object size 0x88, parent = the current task type) with a zero task-list count, stores the returned 16-bit class id at retail 0x0054F768 and returns the TaskType_Register result.
// Status/limits: TRANSCRIBED; no runtime evidence; the handler set is empty in retail, this body is only the registration.
#include "igi/retail/core/task_types.hpp"
#include <cstdint>

extern "C" {

// Retail global at VA 0x0054F768: the CamTask class id (16-bit store in the retail body).
std::uint16_t g_54f768 = 0x0181U; // retail 0x0054F768, .data image init 0x0181

std::int32_t CamTask_Init()
{
    // 0x00530220-0x00530234: push 0; push 0; call QTask_GetCurrentType; push eax; push 0x88;
    // push "CamTask"; call TaskType_Register. The two zero pushes are the retail fourth and
    // fifth stack words (task-list argument 0), i.e. the task_list_count argument here.
    const std::uint16_t type_id = TaskType_Register("CamTask", 0x88U, QTask_GetCurrentType(), 0U);
    // 0x0053023C: mov word [0x0054F768], ax.
    g_54f768 = type_id;
    return static_cast<std::int32_t>(type_id);
}

} // extern "C"
