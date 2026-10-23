#pragma once
#include <cstdint>

namespace igi::retail::core
{
extern std::uint16_t g_6e5bc4;
// Legacy catalog name: this entry initializes x87 rounding, not display surfaces.
void Display_ClearSurfaces_16();
// Legacy camera labels below are CRT floating-point control functions.
std::uint32_t Camera_ApplyBandFilter_24(std::uint16_t control_word);
std::uint32_t Camera_ApplyWideScreen_24(std::uint32_t flags);
std::uint32_t Camera_SetFieldOfView_24(std::uint32_t flags, std::uint32_t mask);
std::uint32_t Camera_ApplyLineFilter_24(std::uint32_t flags, std::uint32_t mask);
std::uint32_t Camera_ApplyBandFilter_10();
}
