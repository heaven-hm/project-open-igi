// IGI native Human_CameraEyePosition - 0x00465820 - transcribed from retail igi.exe.
// Retail: cdecl (out, human). Standing eye height is the double 0x40A6147AE147AE14, crouched 0x4096666666666666; out gets the 16 bytes at human+0x20 and the double at human+0x30 added to the eye height.
#include <cstdint>
#include <cstring>

std::uint8_t __cdecl Human_IsCrouched(void* human); // 0x00463180

void __cdecl Human_CameraEyePosition(void* out, void* human)
{
    std::uint8_t* dst = reinterpret_cast<std::uint8_t*>(out);
    std::uint8_t* src = reinterpret_cast<std::uint8_t*>(human);
    const std::uint32_t standLow = 0xe147ae14u, standHigh = 0x40a6147au;
    const std::uint32_t crouchLow = 0x66666666u, crouchHigh = 0x40966666u;

    std::memcpy(dst + 0x10, &standLow, 4);
    std::memcpy(dst + 0x14, &standHigh, 4);
    if (Human_IsCrouched(human) != 0)
    {
        std::memcpy(dst + 0x10, &crouchLow, 4);
        std::memcpy(dst + 0x14, &crouchHigh, 4);
    }
    std::memcpy(dst + 0x00, src + 0x20, 4);
    std::memcpy(dst + 0x04, src + 0x24, 4);
    std::memcpy(dst + 0x08, src + 0x28, 4);
    std::memcpy(dst + 0x0c, src + 0x2c, 4);

    double eyeOffset;
    double eyeHeight;
    std::memcpy(&eyeOffset, src + 0x30, 8);
    std::memcpy(&eyeHeight, dst + 0x10, 8);
    double result = eyeOffset + eyeHeight;
    std::memcpy(dst + 0x10, &result, 8);
}
