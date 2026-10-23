// Host definitions for the verification build of open-igi.exe.
//
// When every retail native is linked into the executable (the
// IGI_KEEP_ALL_RETAIL_NATIVES option) the linker also pulls in translation
// units that reference retail globals and helpers the host does not otherwise
// need. These definitions satisfy those references so a runtime probe can
// observe every native in the candidate executable.
//
// They are host support only: they are not retail transcriptions and make no
// claim about retail behaviour.
#include "igi/retail/core/frame_natives.hpp"
#include "igi/retail/audio/sound_key_functions.hpp"

#include <cstdarg>
#include <cstdio>
#include <cstdint>

extern "C" {

// Retail global 0x00A970E0: the frame dispatch callback table. The host does
// not install retail frame callbacks, so every slot stays empty.
igi::retail::core::FrameDispatchCallback DAT_00A970E0[64]{};

// Retail logger used by the sound natives for empty or dotless paths.
void IGI_RETAIL_CDECL Engine_LogFormattedMessage(const char* format, ...)
{
    char buffer[512];
    va_list arguments;
    va_start(arguments, format);
    std::vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);
    std::fprintf(stderr, "[retail-native] %s\n", buffer);
}

} // extern "C"
