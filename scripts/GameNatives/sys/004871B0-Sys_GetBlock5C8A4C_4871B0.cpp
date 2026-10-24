// Candidate: Sys_GetBlock5C8A4C_4871B0; retail VA 0x004871B0. Catalog candidate for engine/src/retail/sys/native_4871b0.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: returns the address of the 0x005C8A4C block (exactly 0x28 bytes); no argument, no branch, no access.
#include <cstdint>
extern "C" {
std::uint8_t g_5c8a4c[0x28];
std::uint8_t* Sys_GetBlock5C8A4C_4871B0(void);
}
