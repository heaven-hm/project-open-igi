// Candidate: Sys_MapCode_4ABC46; retail VA 0x004ABC46. Catalog candidate for engine/src/retail/sys/native_4abc46.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: stores code at 0x00936068, scans the 45-entry table at 0x005466D8 for a match and stores the mapped value at
// 0x00936064, otherwise maps 0x13..0x24 to 0x0D, 0xBC..0xCA to 0x08 then 0x16 above 0xCA, and everything else to 0x16.
#include <cstdint>
extern "C" {
std::uint32_t g_936068;
std::uint32_t g_936064;
void Sys_MapCode_4ABC46(std::int32_t code);
}
