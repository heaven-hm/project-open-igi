// Candidate: Sys_GetGlobal16_424850; retail VA 0x00424850. Catalog candidate for engine/src/retail/sys/native_424850.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads 2 byte(s) from 0x0053B90C and returns them; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint16_t g_53b90c;
}
