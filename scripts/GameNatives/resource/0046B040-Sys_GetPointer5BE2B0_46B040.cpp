// Candidate: Sys_GetPointer5BE2B0_46B040; retail VA 0x0046B040. Catalog candidate for engine/src/retail/resource/native_46b040.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: returns the dword stored at 0x005BE2B0; no argument, no branch, no store.
#include <cstdint>
extern "C" {
std::uint32_t g_5be2b0;
std::uint32_t Sys_GetPointer5BE2B0_46B040(void);
}
