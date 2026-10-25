// Native: QTask_LiveTaskCountGet_401A60 (placeholder until VERIFIED); retail VA 0x00401A60-0x00401A65 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 linear decode; docs/fidelity/evidence/0x4EE970-data-extents.md sections 12 and 17.4.
// Behavior/CFG: loads the dword live-task-count storage at 0x00567C58 and returns it in EAX; no arguments, no callees, single basic block.
// Status/limits: BUILD_VERIFIED (static only); runtime UNOBSERVED because the startup boundary throws before this native runs. The storage is now defined by its retail writer, the task allocator 0x004012F0, so this file only declares it.
#include <cstdint>

extern "C" {

// Defined by the retail writer 0x004012F0 (which increments it before any branch) in
// engine/src/retail/input/input_port_process.cpp; the writer 0x004015F0 decrements it. Only a retail writer may define
// a global, so this reader declares it and the name remains the placeholder the writer currently uses.
extern std::uint32_t QTask_InputPortCounter;

std::uint32_t QTask_LiveTaskCountGet_401A60()
{
    return QTask_InputPortCounter;   // 0x00401A60: mov eax, dword [0x567c58] ; 0x00401A65: ret
}

} // extern "C"
