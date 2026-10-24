// Candidate: Sys_GetSettingByte_416860; retail VA 0x00416860. Catalog candidate for engine/src/retail/config/native_416860.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads byte 0x0B of the 0x00539540 block and returns it; no arguments, no branch, no store.
#include <cstdint>
extern "C" {
std::uint8_t g_539540[0x0F];
}
