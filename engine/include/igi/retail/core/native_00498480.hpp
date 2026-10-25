#pragma once

#include <cstdint>

// Retail 0x00498480: probes the installed DirectX tier and the Win32 platform id.
// Both outputs are 32-bit dwords written by the routine; retail callers pass the
// addresses of two stack locals (main.cpp 0x0048F6E2).
extern "C" void Display_ProbeDirectXVersion(std::uint32_t* tier, std::uint32_t* platform);
