// Candidate: Sys_GetGlobal16_4D9FA0; retail VA 0x004D9FA0. Catalog candidate for engine/src/retail/sys/native_4d9fa0.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads 2 byte(s) from 0x00548648 and returns them; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint16_t g_548648;
}
