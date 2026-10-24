// Native: QFile_AllocateDevice_4B1180; retail VA 0x004B1180 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1180-0x004B119F (32 bytes, 1 block, 8 instructions: a1 6c399400 / 8d0cc5 / 2bc8 / 8d0c89 / 89048d04379400 / 40 / a36c399400 / 48 / c3).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.2 of the evidence sheet.
// Behavior/CFG: takes the next device index from 0x0094396C, stores it into that record's +0x54 field at 0x00943704 + index*0x8C, increments the counter, decrements the local copy and returns the ORIGINAL index in EAX.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. This file owns the 0x009436B0 table storage and the 0x0094396C counter, because no transcribed native before it writes them; 0x004B1200 sets the counter to 1 and is not landed yet, so in this build the counter starts at 0.
#include "igi/retail/qfile/driver_table.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x009436B0: the five 0x8C-byte device-driver records.
std::uint8_t g_9436b0[5 * 0x8C];

// 0x0094396C: the next device index.
std::int32_t g_94396c;

std::int32_t NATIVE_DECL QFile_AllocateDevice_4B1180(void)
{
    std::int32_t index = ::g_94396c;                                        // 0x004B1180: mov eax,[0x0094396C]
    *reinterpret_cast<std::int32_t*>(                                       // 0x004B1185-0x004B1191: (index*8-index)*5*4 = index*0x8C
        reinterpret_cast<std::uint8_t*>(&::g_9436b0[0]) + 0x8C * index + 0x54U) = index;
    ::g_94396c = index + 1;                                                 // 0x004B1198/0x004B1199
    return index;                                                           // 0x004B119E/0x004B119F: dec eax; ret
}

} // extern "C"

#undef NATIVE_DECL
