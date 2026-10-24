// Candidate: Sys_SetItemWord_4B8970; retail VA 0x004B8970. Catalog candidate for engine/src/retail/sys/native_4b8970.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: stores the argument at 0x00A96598; no branch, no call.
#include <cstdint>
extern "C" {
std::uint32_t g_a96598;
void Sys_SetItemWord_4B8970(std::int32_t value);
}
