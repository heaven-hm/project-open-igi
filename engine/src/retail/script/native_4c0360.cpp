// Native: Script_QHashTableCreate (placeholder until VERIFIED); retail VA 0x004C0360-0x004C03B7 (87 bytes, 5 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (56 57 6a04 6a08 e8.. 8b7c2414 8bf0 6a04 8d047f 897e04 c1e002 50 e8.. 33d2 83c410 3bfa 8906 7625 / 53 33c0 8b0e 03c8 8d5904 8919 8b0e 03c8 83c00c 4f 894908 8b0e 895401f8 75e4 5b 8bc6 5f 5e c3); Mem_Alloc reads two args (mov eax,[esp+4]; mov ebx,[esp+0xc]; lea ebp,[eax+ebx+0x1c]) so the second call's arg2 is the stacked 8.
// Behavior/CFG: allocates an 8-byte header and a count*12-byte bucket block via the verified Mem_Alloc, stores header[4]=count and header[0]=buckets, then for each bucket writes next=bucket+4, back=bucket and clears bucket+4 (self-linked 12-byte bucket, count times, zero iterations when count <= 0); returns the header.
// Status/limits: BUILD_VERIFIED (static only); the second Mem_Alloc's second argument is the retail's leftover stack value 8 (stack reuse), reproduced as an explicit argument; runtime UNOBSERVED.
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" {

void* Script_QHashTableCreate(std::int32_t const count)
{
    std::uint8_t* const header = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(8, 4));            // 0x004C0366
    *reinterpret_cast<std::int32_t*>(header + 4) = count;                                                  // 0x004C0376
    std::uint8_t* const buckets = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(count * 12, 8));   // 0x004C037D (arg2 = stacked 8)
    *reinterpret_cast<std::uint8_t**>(header) = buckets;                                                   // 0x004C0389
    if (count > 0) {                                                                                       // 0x004C0387: cmp edi,edx; jbe
        for (std::int32_t i = 0; i < count; i += 1) {                                                      // 0x004C0390..0x004C03AA
            std::uint8_t* const bucket = buckets + static_cast<std::ptrdiff_t>(i) * 12;                     // 0x004C0392: add ecx,eax
            *reinterpret_cast<std::uint8_t**>(bucket) = bucket + 4;                                         // 0x004C0397
            *reinterpret_cast<std::uint8_t**>(bucket + 8) = bucket;                                         // 0x004C03A1
            *reinterpret_cast<std::uint8_t**>(bucket + 4) = nullptr;                                        // 0x004C03A6
        }
    }
    return header;                                                                                         // 0x004C03AD
}

} // extern "C"
