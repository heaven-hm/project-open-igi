// Native: QFile_DeviceIndex_4B11A0; retail VA 0x004B11A0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B11A0-0x004B11F0 (81 bytes, 6 blocks, 7 edges).
// Tools/evidence: radare2 6.2.0 af/pdf; section 21 of the evidence sheet; nine call sites (0x004B12F8, 0x004B1442, 0x004B14C1, 0x004B1538, 0x004B15AF, 0x004B184C, 0x004B1ADB, 0x004B1B4F, 0x005098D5).
// Behavior/CFG: walks the five device records from 0x009437B8 (record 1's name) to 0x009439E8 stride 0x8C with a 1-based index; for each record whose +0x1C present word is non-zero it measures the record name with a byte scan and calls 0x004AAD10 Resource_MatchPrefix(record name, input, length); returns the 1-based index on the first zero result, otherwise 0. One argument (the resolved path), cdecl, caller cleans.
// Status/limits: BUILD_VERIFIED (6 blocks / 7 edges); runtime UNOBSERVED. The name length is computed with the same byte scan retail performs inline at 0x004B11BB-0x004B11C4.
#include "igi/retail/qfile/device_table.hpp"
#include "igi/retail/resource/sys_resource.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL QFile_DeviceIndex_4B11A0(const char* resolved_path)
{
    std::int32_t index = 1;                                              // 0x004B11A8: ebx = 1
    DeviceRecord* record = &::g_9436ec[1];                               // 0x004B11AD: esi = 0x009437B8
    do
    {
        if (record->present != 0)                                        // 0x004B11B2/0x004B11B5/0x004B11B7
        {
            std::int32_t length = 0;                                     // 0x004B11BB-0x004B11C4: strlen of the record name
            while (record->name[length] != '\0')
            {
                ++length;
            }
            if (igi::retail::resource::Resource_MatchPrefix(record->name, resolved_path, length) == 0) // 0x004B11C5-0x004B11CD
            {
                return index;                                            // 0x004B11EA-0x004B11EC: eax = ebx
            }
        }
        record = reinterpret_cast<DeviceRecord*>(
            reinterpret_cast<std::uint8_t*>(record) + 0x8CU);             // 0x004B11D4
        ++index;                                                         // 0x004B11DA
    } while (record < &::g_9436ec[5]);                             // 0x004B11DB/0x004B11E1: cmp esi,0x009439E8; jl (record 5's base)
    return 0;                                                            // 0x004B11E6: xor eax, eax
}

} // extern "C"

#undef NATIVE_DECL
