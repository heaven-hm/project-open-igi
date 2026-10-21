#pragma once

// The retail's human player and configuration script natives.
//
// The human player's movement values come from humanplayer.qvm's
// DefineHumanPlayerGeneral, which scales the script's real arguments with the
// retail's constants, and the human player's parameters come from
// LOCAL:humanplayer/humanplayer.qsc, which the retail opens, reads and
// closes. The configuration file is read the same way.

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define IGI_RETAIL_CDECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define IGI_RETAIL_CDECL __attribute__((cdecl))
#else
#define IGI_RETAIL_CDECL
#endif

namespace igi::retail::script {

// The retail's human player general parameters, at 0x0056E1E8.
struct HumanPlayerGeneral
{
    float first;         // +0x00, 0x0056E1E8, the first argument as it comes
    float second;        // +0x04, 0x0056E1EC
    float third;         // +0x08, 0x0056E1F0
    float fourth;        // +0x0C, 0x0056E1F4
    float fifth;         // +0x10, 0x0056E1F8
    float sixth;         // +0x14, 0x0056E1FC
};

// The retail's scale constants: 0x005334A4 and 0x005334A0 are single precision
// floats and 0x00533498 is a double.
inline constexpr float human_player_scale_4096000 = 4096000.0F;
inline constexpr float human_player_scale_inverse = 9.259259059035685e-06F;
inline constexpr double human_player_scale_4096 = 4096.0;

} // namespace igi::retail::script

extern "C" {

// The retail's human player general parameters at 0x0056E1E8.
extern igi::retail::script::HumanPlayerGeneral HumanPlayer_GeneralParameters;

// Native: DefineHumanPlayerGeneral() - VA 0x00410070 - complete port.
// Behavior: reads the script's real arguments with the reader at 0x004B8A50
//           and scales them on the x87 stack: the first argument is stored as
//           it comes at 0x0056E1E8, and each following pair is scaled by
//           4096000.0 and by 1/108000 before it is stored, with the wider
//           scale left on the stack for the next store. The 80-bit
//           intermediates are kept with long double, the x87 type on the x86
//           target.
void IGI_RETAIL_CDECL DefineHumanPlayerGeneral(void* qvm_context, const float* arguments,
                                               unsigned int argument_count);

// Native: HumanPlayer_LoadParameters() - VA 0x004137A0 - complete port.
// Behavior: opens "LOCAL:humanplayer/humanplayer.qsc" through the file native
//           at 0x004B80B0, reads it through 0x004B85B0 and closes it through
//           0x004B83D0. It returns when the file is not there.
void IGI_RETAIL_CDECL HumanPlayer_LoadParameters();

// Native: Config_Read() - VA 0x00405850.
// Behavior: Qvm_Load(path); on success Qvm_Execute and Qvm_Cleanup, then calls
//           natives 0x004058A0, 0x00405980 and 0x00405B30 in order. On load
//           failure it first calls native 0x00405300, then the same final
//           three natives.
std::int32_t IGI_RETAIL_CDECL Config_Read(void* path);

} // extern "C"

#undef IGI_RETAIL_CDECL
