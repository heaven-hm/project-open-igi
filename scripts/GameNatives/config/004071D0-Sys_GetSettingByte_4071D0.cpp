// Candidate: Sys_GetSettingByte_4071D0; retail VA 0x4071D0. Catalog candidate for the native in engine/src/retail/config/native_4071d0.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads one setting byte into al and returns; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint32_t g_5690d8;
}
namespace igi::retail::config {
std::uint8_t Sys_GetSettingByte_43X_4071D0();
}
