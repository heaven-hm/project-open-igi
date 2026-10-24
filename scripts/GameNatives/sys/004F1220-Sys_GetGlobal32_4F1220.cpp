// Candidate: Sys_GetGlobal32_4F1220; retail VA 0x004F1220. Catalog candidate for engine/src/retail/sys/native_4f1220.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads 4 byte(s) from 0x0054DDAC and returns them; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint32_t g_54ddac;
}
