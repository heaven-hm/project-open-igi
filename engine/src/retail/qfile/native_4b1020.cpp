// Native: QFile_AliasResolve_4B1020; retail VA 0x004B1020 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1020-0x004B117E (351 bytes, 146 instructions, 17 blocks, 25 edges); the byte after the final ret is 0x004B117F (90) padding and 0x004B1180 begins the next native (device-index allocator, 32 bytes).
// Tools/evidence: radare2 6.2.0 af/afij/afbj/pdf and the raw bytes (51 8b442410 53 55 56 57 89442410 33db be30239400 ... 5f 5e 5d 5b 59 c3), cross-decoded with NASM ndisasm over the exact 351-byte extent; evidence sheet section 22 (alias/device table family).
// Behavior/CFG: walks the 32 0x94-byte records at 0x00942330 (bound 0x009435B0); a record whose handle word at record+0x90 is non-zero has its record name at record+0x00 compared against argument2 with Resource_MatchPrefix (0x004AAD10, count = strlen(record name)). On a match it builds the result at 0x00942340 + index*0x94: skips `depth` semicolons (';' = 0x3B) with the CRT strchr helper (0x004AA0A0), copies the segment up to the next ';' into argument1, appends argument2 after the matched record-name prefix, copies the whole text to the 0x00943A18 buffer, and returns depth+1 when another ';' follows or 0 when the segment is the last one. No record matching means argument2 is copied to argument1 and to 0x00943A18 and 0 is returned. Running out of semicolons while skipping depth calls Debug_Error (0x004AF7B0) with the record's target string and then spins.
// Status/limits: STRUCTURALLY_VERIFIED; runtime UNOBSERVED. The record walk compares its cursor against the table end: retail compares to the absolute address 0x009435B0, which is 0x00942330 + 0x1280, and a port copy of the table is not linked at that retail address, so the bound is written as `record >= &g_942330[0x1280]` (the same form landed in native_4b12d0.cpp, native_4b12f0.cpp and native_4b13a0.cpp). This replaces the port-owned implementation in alias_resolve.cpp: the destination, the record walks, the 0x00943A18 buffer, the ';' skips and the copy orders are retail's, and the matching is the retail callee, not strncmp. 0x004AA0A0 (CRT strchr entry, body at 0x004AA090 and 0x004AA0A6) is the one callee not yet in the tree; it is declared extern here and must land with this file or be substituted by the CRT strchr. The 0x00943A18 buffer's extent is inferred, not stated by any instruction: the nearest referenced global above it is 0x00943A98, giving 0x80 bytes, while both copies write strlen+1 bytes with no bound, exactly as retail does. Retail interleaves the handle word at record+0x90; the tree currently splits the records (0x00942330, owned by native_4b13a0.cpp) from their handle words (0x009423C0, owned by native_4b12d0.cpp), which overlap in retail address space. This body reads the handle through the tree's 0x009423C0 array so it agrees with the landed writers; if the two are consolidated into one 0x1310-byte region at 0x00942330 the handle read becomes record[0x90].
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/resource/sys_resource.hpp"

#include <cstdint>
#include <cstring>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// 0x00942330: the 32 0x94-byte alias records (owned by qfile/native_4b13a0.cpp).
extern std::uint8_t g_942330[0x1280];
// 0x009423C0: the handle words at record+0x90 (owned by qfile/native_4b12d0.cpp).
extern std::uint8_t g_9423c0[0x1280];
// 0x00943A18: the resolved-alias text. No owner in the tree yet; extent inferred from the next
// referenced global at 0x00943A98 (see Status/limits), which is 0x80 bytes.
std::uint8_t g_943a18[0x80];

// 0x004AA0A0 (CRT strchr, sys/native_4aa0a0.cpp): first occurrence of `character` in `text`, or null.
const char* NATIVE_DECL String_FindCharacter_4AA0A0(const char* text, std::int32_t character);

} // extern "C"

namespace igi::retail::qfile {

std::int32_t NATIVE_DECL QFile_AliasResolve_4B1020(char* destination, const char* alias, std::int32_t depth)
{
    std::int32_t result = depth;                                     // 0x004B1021/0x004B1029: the third argument is copied into the [esp+0x10] local
    std::uint32_t index = 0;                                         // 0x004B102D: xor ebx,ebx
    std::uint32_t name_length = 0;                                   // 0x004B104A: mov ebp,ecx
    std::uint8_t* record = &::g_942330[0];                           // 0x004B102F: mov esi,0x00942330

    for (;;)
    {
        if (*reinterpret_cast<const std::int32_t*>(&::g_9423c0[0x94U * index]) != 0) // 0x004B1034-0x004B103C: mov eax,[esi+0x90]; test eax,eax; jz advance
        {
            name_length = 0;                                         // 0x004B103E-0x004B104A: repne scasb over record+0x00; ebp = length without the NUL
            while (record[name_length] != 0U)
            {
                ++name_length;
            }

            if (igi::retail::resource::Resource_MatchPrefix(           // 0x004B1050-0x004B105D: push ebp; push esi; push ecx(argument2); call 0x004AAD10
                    alias, record, static_cast<std::int32_t>(name_length)) == 0)
            {
                goto record_matched;                                 // 0x004B105D: je 0x004B10BB
            }
        }

        record += 0x94U;                                             // 0x004B105F: add esi,0x94
        ++index;                                                     // 0x004B1065: inc ebx
        if (record >= &::g_942330[0x1280])                            // 0x004B1066-0x004B106C: cmp esi,0x009435B0; jl record head
        {
            break;
        }
    }

    // No record matched: 0x004B106E-0x004B10BA.
    std::memcpy(destination, alias, std::strlen(alias) + 1U);        // 0x004B106E-0x004B1093: repne scasb on argument2, then rep movsd/rep movsb into argument1
    std::memcpy(&::g_943a18[0], alias, std::strlen(alias) + 1U);     // 0x004B1095-0x004B10B3: repne scasb on argument2 again, then rep movsd/rep movsb into 0x00943A18
    return 0;                                                        // 0x004B108E xor eax,eax survives; 0x004B10B5-0x004B10BA pop/ret

record_matched:
    {
        // 0x004B10BB-0x004B10C1: lea edx,[ebx+ebx*8]; lea ebx,[ebx+edx*4]; lea edi,[ebx*4+0x00942340]
        const char* target = reinterpret_cast<const char*>(record) + 0x10U;
        const char* cursor = target;                                 // 0x004B10CE: mov esi,edi

        for (std::int32_t remaining = depth; remaining != 0; --remaining) // 0x004B10C8-0x004B10E5: ebx = depth; loop while ebx != 0
        {
            const char* const separator =                             // 0x004B10D2-0x004B10DA: push 0x3B; push esi; call 0x004AA0A0
                String_FindCharacter_4AA0A0(cursor, 0x3B);
            if (separator == nullptr)                                 // 0x004B10DF-0x004B10E1: test esi,esi; jz 0x004B10FC
            {
                Debug_Error("Error in alias path: %s", target);       // 0x004B10FC-0x004B1107: push edi; push 0x00546F1C; call 0x004AF7B0 (prints the record target, not argument2)
                for (;;)                                             // 0x004B110A: jmp $ (retail never continues)
                {
                }
            }

            cursor = separator + 1U;                                  // 0x004B10E3-0x004B10E4: inc esi; dec ebx
        }

        if (String_FindCharacter_4AA0A0(cursor, 0x3B) != nullptr)     // 0x004B10E7-0x004B10F4: push 0x3B; push esi; call 0x004AA0A0; test eax,eax
        {
            ++result;                                                // 0x004B10F6: inc dword [esp+0x10]
        }
        else
        {
            result = 0;                                              // 0x004B110C: mov dword [esp+0x10],0
        }

        char* out = destination;                                     // 0x004B1116/0x004B111C: mov ebx,[esp+0x18]; mov edx,ebx
        while (*cursor != ';' && *cursor != '\0')                    // 0x004B1114-0x004B112D: cmp al,0x3B; test al,al; mov [edx],al loop
        {
            *out = *cursor;                                          // 0x004B1124: mov [edx],al
            ++out;                                                   // 0x004B1129: inc edx
            ++cursor;                                                // 0x004B112A: inc esi
        }

        std::memcpy(out, alias + name_length,                        // 0x004B112F-0x004B1153: repne scasb from argument2+ebp, then rep movsd/rep movsb
                    std::strlen(alias + name_length) + 1U);
        std::memcpy(&::g_943a18[0], destination,                     // 0x004B1155-0x004B1177: repne scasb on argument1, then rep movsd/rep movsb into 0x00943A18
                    std::strlen(destination) + 1U);
        return result;                                               // 0x004B1160: mov eax,[esp+0x10]; 0x004B1178-0x004B117E pop/ret
    }
}

} // namespace igi::retail::qfile

#undef NATIVE_DECL
