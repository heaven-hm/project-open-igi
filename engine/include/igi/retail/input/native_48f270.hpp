#pragma once

#include <cstdint>

extern "C" {

// Native: Input_GetInstance (placeholder until VERIFIED); retail VA 0x0048F270.
// Returns the instance handle stored at retail 0x005C8BE0.
std::int32_t Input_GetInstance();

} // extern "C"
