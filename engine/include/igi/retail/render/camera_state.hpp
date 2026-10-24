#pragma once

#include <cstdint>

extern "C" {

// Native: Camera_Get_4F0F80 (placeholder until VERIFIED); retail VA 0x004F0F80.
// Returns the dword at retail 0x00A71874 (written by CameraBase_Init 0x4EE970).
std::uint32_t Camera_Get_4F0F80();

// Native: Camera_Get_4F1000 (placeholder until VERIFIED); retail VA 0x004F1000.
// Returns the dword at retail 0x00A70C90 (written by CameraBase_Init 0x4EE970).
std::uint32_t Camera_Get_4F1000();

// Port globals for the two camera dwords; image init is loader zero-fill (retail
// .data tail, no file bytes). Written only by the untranscribed 0x4EE970.
extern std::uint32_t g_a71874;
extern std::uint32_t g_a70c90;

} // extern "C"
