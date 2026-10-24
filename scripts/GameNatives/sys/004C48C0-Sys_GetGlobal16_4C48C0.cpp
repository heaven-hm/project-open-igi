// Candidate: Sys_GetGlobal16_4C48C0; retail VA 0x004C48C0. Catalog candidate for engine/src/retail/sys/native_4c48c0.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads 2 byte(s) from 0x00547F42 and returns them; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint16_t g_547f42;
}
