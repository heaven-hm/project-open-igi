// Native: QFile_RegisterDevice_4B1B90; retail VA 0x004B1B90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1B90-0x004B1BB7 (40 bytes, 3 blocks, 3 edges).
// Tools/evidence: radare2 6.2.0 af/pdf; section 21 of the evidence sheet (identified as the writer of the device record's +0x1C word).
// Behavior/CFG: calls its second argument with its first argument, and when that returns a non-zero value stores 1 into the +0x1C word of the device record at that index (0x00943708 + index * 0x8C). No bound check on the returned index, exactly as retail has none. Returns void.
// Status/limits: BUILD_VERIFIED (3 blocks / 3 edges); runtime UNOBSERVED. This is the registrar that makes a device record present; wiring it into the startup sequence is a separate transcription.
#include "dev_log.hpp"
#include "igi/retail/qfile/device_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

typedef std::int32_t(NATIVE_DECL* QFile_RegisterDeviceCallback_4B1B90)(std::int32_t);

void NATIVE_DECL QFile_RegisterDevice_4B1B90(std::int32_t argument,
                                             QFile_RegisterDeviceCallback_4B1B90 callback)
{
    NATIVE_LOG_ENTER("QFile_RegisterDevice_4B1B90", "0x004B1B90");
    const std::int32_t index = callback(argument);   // 0x004B1B90/0x004B1B94/0x004B1B95/0x004B1B99
    if (index != 0)                                  // 0x004B1B9C/0x004B1B9E
    {
        ::g_9436ec[index].present = 1;               // 0x004B1BA0/0x004B1BA7/0x004B1BA9/0x004B1BAC/0x004B1BB7
    }
}

} // extern "C"

#undef NATIVE_DECL
