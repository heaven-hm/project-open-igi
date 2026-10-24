// Native: EventTable_4F1EE0 (placeholder until VERIFIED); retail VA 0x004F1EE0-0x004F1F50 (112 bytes, 1 block).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 disassembly/bytes; Binary Ninja MCP decompile; chain position 10 of 0x5183D0 (registers no classes).
// Behavior/CFG: seven QTask_AllocateEvent(0) results stored as bytes at 0x00A76C02/03/09/00/01/10/08, two QTask_NewQTaskEventNoContextFn() results stored as dwords at 0x00A76BFC/0x00A76C04, in retail store order; returns the last event id (AL).
// Status/limits: TRANSCRIBED; callees 0x4017C0/0x401810 are transcribed. Global block is .data zero-fill (init 0). Runtime UNOBSERVED.
#include "igi/retail/core/task_types.hpp"
#include <cstdint>

extern "C" {
std::uint8_t QTask_NewQTaskEventNoContextFn(std::uint16_t value);

std::uint32_t g_a76bfc = 0U; // retail 0x00A76BFC
std::uint32_t g_a76c04 = 0U; // retail 0x00A76C04
std::uint8_t g_a76c02 = 0U;  // retail 0x00A76C02
std::uint8_t g_a76c03 = 0U;  // retail 0x00A76C03
std::uint8_t g_a76c09 = 0U;  // retail 0x00A76C09
std::uint8_t g_a76c00 = 0U;  // retail 0x00A76C00
std::uint8_t g_a76c01 = 0U;  // retail 0x00A76C01
std::uint8_t g_a76c10 = 0U;  // retail 0x00A76C10
std::uint8_t g_a76c08 = 0U;  // retail 0x00A76C08

std::uint8_t EventTable_4F1EE0()
{
    g_a76c02 = QTask_AllocateEvent(0);
    g_a76c03 = QTask_AllocateEvent(0);
    g_a76c09 = QTask_AllocateEvent(0);
    g_a76c00 = QTask_AllocateEvent(0);
    g_a76c01 = QTask_AllocateEvent(0);
    g_a76c10 = QTask_AllocateEvent(0);
    g_a76bfc = QTask_NewQTaskEventNoContextFn(0);
    g_a76c04 = QTask_NewQTaskEventNoContextFn(0);
    g_a76c08 = QTask_AllocateEvent(0);
    return g_a76c08;
}

} // extern "C"
