#pragma once
#include <cstdint>

namespace igi::retail::core
{
extern std::uint16_t g_6e5bc4;
// The retail's x87 precision setup. The legacy catalog name called it a
// display clear; the retail's instructions store the control word, set the
// extended precision control and load it back.
void FP_InitExtendedPrecision();
// Legacy camera labels below are CRT floating-point control functions.
std::uint32_t Camera_ApplyBandFilter_24(std::uint16_t control_word);
std::uint32_t Camera_ApplyWideScreen_24(std::uint32_t flags);
std::uint32_t Camera_SetFieldOfView_24(std::uint32_t flags, std::uint32_t mask);
std::uint32_t Camera_ApplyLineFilter_24(std::uint32_t flags, std::uint32_t mask);
std::uint32_t Camera_ApplyBandFilter_10();
}
