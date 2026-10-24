// Candidate: Sys_GetBlock5BDDB0_46B4D0; retail VA 0x0046B4D0. Catalog candidate for engine/src/retail/resource/native_46b4d0.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: returns the address of the 0x005BDDB0 block; no argument, no branch, no access.
#include <cstdint>
extern "C" {
std::uint8_t g_5bddb0[0x3C];
std::uint8_t* Sys_GetBlock5BDDB0_46B4D0(void);
}
