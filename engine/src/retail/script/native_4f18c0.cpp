// Native: Script_ParameterEventReset_4F18C0 (placeholder until VERIFIED); retail VA 0x004F18C0-0x004F194C (141 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 listing (a0ebe054.. 50 e8.. / a0ebe054.. 50 e8.. x6 for EB/EA/E7/E8/E9/E6, 0cff a2.. x6, a0e4e054.. 50 e88ffff0ff, a0e5e054.. 50 e883fff0ff, b0ff 83c420 a2e4e054.. a2e5e054.. c3); callees 0x4F1B50 and TaskEventTable_ClearSlot_4018C0 both transcribed.
// Behavior/CFG: releases the six parameter-event slots through 0x4F1B50 in retail order EB, EA, E7, E8, E9, E6, writes 0xFF over all six state bytes, clears the two task-event slots 0x54E0E4/0x54E0E5 through 0x4018C0, then writes 0xFF over those two. Single straight-line block.
// Status/limits: TRANSCRIBED (static only); the `or al,0xff` constant and the six-store group are reproduced as written rather than folded; runtime UNOBSERVED.
#include <cstdint>

extern "C" {

std::uint8_t g_54e0e4[8]; // 0x0054E0E4..0x0054E0EB, owned by 0x4F1840

void Script_ParameterEventSlotClear_4F1B50(std::int8_t index);
void TaskEventTable_ClearSlot_4018C0(std::uint32_t index);

void Script_ParameterEventReset_4F18C0()
{
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[7])); // 0x004F18C6 -> 0x54E0EB
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[6])); // 0x004F18D2 -> 0x54E0EA
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[3])); // 0x004F18DE -> 0x54E0E7
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[4])); // 0x004F18E9 -> 0x54E0E8
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[5])); // 0x004F18F5 -> 0x54E0E9
    Script_ParameterEventSlotClear_4F1B50(static_cast<std::int8_t>(g_54e0e4[2])); // 0x004F1901 -> 0x54E0E6
    g_54e0e4[7] = 0xFF;                                                          // 0x004F1908 (or al,0xff)
    g_54e0e4[6] = 0xFF;                                                          // 0x004F190D
    g_54e0e4[3] = 0xFF;                                                          // 0x004F1912
    g_54e0e4[4] = 0xFF;                                                          // 0x004F1917
    g_54e0e4[5] = 0xFF;                                                          // 0x004F191C
    g_54e0e4[2] = 0xFF;                                                          // 0x004F1921
    TaskEventTable_ClearSlot_4018C0(g_54e0e4[0]);                                // 0x004F192C
    TaskEventTable_ClearSlot_4018C0(g_54e0e4[1]);                                // 0x004F1938
    g_54e0e4[0] = 0xFF;                                                          // 0x004F1942 (b0ff)
    g_54e0e4[1] = 0xFF;                                                          // 0x004F1947
}

} // extern "C"
