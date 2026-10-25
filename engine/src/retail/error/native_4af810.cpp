// Native: Script_WarningLog (placeholder until VERIFIED); retail VA 0x004AF810-0x004AF85D (78 bytes, 3 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (a174629300 85c0 7444 57 68c86d5400 684046a8.. e88a5bffff 8b4c2410 8d4424 14 50 51 bf4046a800 83c9ff 33c0 f2ae f7d1 49 81c14046a8.. 51 e8b45effff 6840 46a8.. ff1574629300 83c418 5f c3); dependencies 0x4A53B3 (CRT sprintf) and 0x4A5702 (String_VFormat) already transcribed in error_show.cpp; callback global owned by 0x4AF880.
// Behavior/CFG: returns immediately when no callback is installed; otherwise writes the literal prefix "Warning: \n\n" into the shared 0x00A84640 buffer, formats the caller's arguments at the buffer's end via String_VFormat, then invokes the installed callback with the whole buffer. Three blocks: callback test, prefix+format, callback tail.
// Status/limits: BUILD_VERIFIED (static only); the prefix uses the CRT sprintf that retail calls at 0x4A53B3, the strlen is an explicit byte loop (retail uses repne scasb), and String_VFormat is the already-transcribed 0x4A5702; runtime UNOBSERVED.
#include "igi/retail/error/error_show.hpp"

#include <cstdarg>
#include <cstdint>
#include <cstdio>

extern "C" void* g_936274; // 0x00936274, owned by 0x4AF880

extern "C" {

void Script_WarningLog(char const* const format, ...)
{
    void* const callback = g_936274;                                     // 0x004AF810
    if (callback == nullptr)                                            // 0x004AF817: je -> ret
        return;
    igi::retail::error::ErrorGlobalsState& globals = igi::retail::error::Error_GetGlobals();
    std::sprintf(globals.g_a84640, "Warning: \n\n");                     // 0x004AF824: CRT sprintf at 0x4A53B3
    std::int32_t length = 0;                                            // 0x004AF833..0x004AF848: repne scasb
    while (globals.g_a84640[length] != 0)
        length += 1;
    std::va_list args;                                                  // 0x004AF82D: lea eax,[esp+0x14]
    va_start(args, format);
    igi::retail::error::String_VFormat(globals.g_a84640 + length, format, args); // 0x004AF849: 0x4A5702
    reinterpret_cast<void (*)(char const*)>(callback)(globals.g_a84640); // 0x004AF853: call [0x936274]
}

} // extern "C"
