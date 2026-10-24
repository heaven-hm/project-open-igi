#pragma once

#include <cstdint>

extern "C" {

// Native: Input_Run; retail VA 0x00507EC0. Polls one input slot: saves the old
// state, calls the slot callback, derives the pressed mask and updates the 32
// per-control hold counters; returns the pointer past the counters.
std::int16_t* Input_Run(std::int32_t* slot, std::int32_t arg2);

} // extern "C"
