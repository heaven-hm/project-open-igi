// Native: EventId (placeholder until VERIFIED); retail VA 0x00507E70-0x00507E80 (16 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes 6a 00 e8 .. .. .. .. 83 c4 04 a2 54 b2 a7 00 c3; Binary Ninja MCP decompile; chain position 4 of 0x5183D0 (registers no classes).
// Behavior/CFG: QTask_AllocateEvent(0), stores AL at 0x00A7B254, returns the same result in AL.
// Status/limits: TRANSCRIBED; only callee 0x4017C0 is transcribed (task_types.cpp). Runtime UNOBSERVED (position 4 runs inside registration, after the boundary).
#include "igi/retail/core/task_types.hpp"
#include <cstdint>

extern "C" std::uint8_t g_a7b254 = 0U; // retail 0x00A7B254, .data zero-fill, image init 0

extern "C" {

std::uint8_t EventId()
{
    g_a7b254 = QTask_AllocateEvent(0); // 0x00507E75: e8 .. (call QTask_AllocateEvent); 0x00507E7A: a2 54 b2 a7 00
    return g_a7b254;
}

} // extern "C"
