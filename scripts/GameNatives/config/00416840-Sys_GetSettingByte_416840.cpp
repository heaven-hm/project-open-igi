// Candidate: Sys_GetSettingByte_416840; retail VA 0x416840. Catalog candidate for the native in engine/src/retail/config/native_416840.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads one setting byte into al and returns; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {

}
namespace igi::retail::config {
std::uint8_t Sys_GetSettingByte_43X_416840();
}
