// Native: QFile_DispatchDevicePath_4B1420; retail VA 0x004B1420 (placeholder name; retail symbol unknown, and the catalog's Sys_IllegalDeviceNameInP is contradicted by the bytes).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1420-0x004B1504 (229 bytes, 14 blocks, 21 edges, 80 instructions).
// Tools/evidence: radare2 6.2.0 af/afij/pdf, listing committed at docs/fidelity/evidence/0x004B1420-retail-listing.txt; cross-decoded with NASM ndisasm over the exact 229-byte extent; evidence sheet section 21.
// Behavior/CFG: resolves the path with 0x004B1020, enumerates a device index with 0x004B11A0, then dispatches through the five 0x8C-byte records at 0x009436EC: a record whose +0x1C word is non-zero has its +0x00 handler called with (index, text after ':' or after the NUL, arg2, arg3); while the resolver reports a further depth the path is resolved again and re-dispatched. Both error paths call Debug_Error and then run retail's `jmp $` (0x004B14E2, 0x004B14F8).
// Status/limits: TRANSCRIBED; the candidate's ':' scan emits 16 blocks / 23 edges where retail has 14/21 (GCC -O3 rotates the scan; four alternative spellings were tried, see section 21). Runtime UNOBSERVED. The device table has no transcribed registrar yet (retail 0x004B1B90 is missing), so every dispatch currently takes the "Device not present" path.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/qfile/alias_resolve.hpp"
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

// 0x004B11A0 and 0x004B1DE0, transcribed in native_4b11a0.cpp and native_4b1de0.cpp.
std::int32_t NATIVE_DECL QFile_DeviceIndex_4B11A0(const char* resolved_path);
char* NATIVE_DECL QFile_GetDeviceName_4B1DE0(std::int32_t index);

std::int32_t NATIVE_DECL QFile_DispatchDevicePath_4B1420(const char* path, void* callback, std::int32_t context)
{
    char buffer[0x80];                                                          // 0x004B1424: sub esp,0x80
    std::int32_t depth = igi::retail::qfile::QFile_AliasResolve_4B1020(buffer, path, 0); // 0x004B1436
    std::int32_t index = QFile_DeviceIndex_4B11A0(buffer);                      // 0x004B1442

    if (index > 0)                                                              // 0x004B144A/0x004B144C
    {
        void* const handler_callback = callback;                                // 0x004B1455: ebp = [esp+0x98]
        const std::int32_t handler_context = context;                           // 0x004B144E: ebx = [esp+0x9C]

        for (;;)
        {
            if (index >= 5)                                                     // 0x004B145C/0x004B145F
            {
                goto illegal_device_name;
            }

            DeviceRecord& record = ::g_9436ec[index];                           // 0x004B1461-0x004B146D: index*0x8C
            if (record.present == 0)                                            // 0x004B1470/0x004B1476/0x004B1478
            {
                goto device_not_present;
            }

            // 0x004B147A-0x004B1490: the byte already loaded is tested for ':' and only then is buffer[colon+1] loaded and counted.
            char character = buffer[0];                                         // 0x004B147A
            std::int32_t colon = 0;                                             // 0x004B147E
            if (character != '\0')                                              // 0x004B1480/0x004B1482
            {
                for (;;)
                {
                    if (character == ':')                                       // 0x004B1484/0x004B1487
                    {
                        break;
                    }
                    character = buffer[colon + 1];                              // 0x004B1489
                    ++colon;                                                    // 0x004B148D
                    if (character == '\0')                                      // 0x004B148E/0x004B1490
                    {
                        break;
                    }
                }
            }

            const std::int32_t handler_result =
                record.handler(index, &buffer[colon + 1], handler_callback, handler_context); // 0x004B1492-0x004B149A

            if (depth == 0)                                                     // 0x004B14A3/0x004B14A5
            {
                return handler_result;                                          // 0x004B14FA-0x004B1504
            }

            depth = igi::retail::qfile::QFile_AliasResolve_4B1020(buffer, path, depth); // 0x004B14B5
            index = QFile_DeviceIndex_4B11A0(buffer);                           // 0x004B14C1
            if (index <= 0)                                                     // 0x004B14C9/0x004B14CB
            {
                break;
            }
        }
    }

illegal_device_name:
    Debug_Error("Illegal device name in path: \"%s\".", path);                  // 0x004B14CD-0x004B14DA
    for (;;)                                                                    // 0x004B14E2: jmp $
    {
    }

device_not_present:
    Debug_Error("Device not present: %s", QFile_GetDeviceName_4B1DE0(index));   // 0x004B14E4-0x004B14F0
    for (;;)                                                                    // 0x004B14F8: jmp $
    {
    }
}

} // extern "C"

#undef NATIVE_DECL
