// Candidate: Sys_GetGlobal32_4F0F70; retail VA 0x004F0F70. Catalog candidate for engine/src/retail/sys/native_4f0f70.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: loads 4 byte(s) from 0x00A70C6C and returns them; no arguments, no branch, no store, no callee.
#include <cstdint>
extern "C" {
std::uint32_t g_a70c6c;
}
