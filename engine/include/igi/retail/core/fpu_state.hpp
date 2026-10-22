#pragma once
#include <cstdint>

namespace igi::retail::core
{
extern std::uint16_t g_6e5bc4;
// The retail's x87 control-word setup at 0x00498460..0x00498476 (23 bytes):
// fwait; fnstcw word [0x006E5BC4]; or word [0x006E5BC4],0x0C00; fldcw word
// [0x006E5BC4]; ret. Bits 10-11 are the x87 rounding-control field; 0b11
// selects truncate (chop), and `or 0x0C00` preserves every other control-word
// bit. The global control word is written in place; the previous control word
// is not restored. The legacy catalog name "InitExtendedPrecision" is a
// misnomer: this is rounding control, not the precision-control field.
void FP_InitExtendedPrecision();
// Legacy camera labels below are CRT floating-point control functions.
std::uint32_t FP_ControlWordToFlags(std::uint16_t control_word);
std::uint32_t FP_FlagsToControlWord(std::uint32_t flags);
std::uint32_t FP_MergeControlWord(std::uint32_t flags, std::uint32_t mask);
std::uint32_t FP_MergeControlWordNoDenormal(std::uint32_t flags, std::uint32_t mask);
std::uint32_t FP_SetDefaultPrecision();
}
