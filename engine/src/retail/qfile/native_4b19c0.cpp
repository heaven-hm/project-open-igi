// Native: QFile_CreateLoadedFileRecord_4B19C0; retail VA 0x004B19C0 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B19C0-0x004B1A27 (104 bytes, 1 block, 0 edges, 45 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes; section 22.3 of the evidence sheet. Creates the records that 0x004B1A90 walks from the same head word 0x00943970.
// Behavior/CFG: measures the name (strlen), allocates strlen+0x1C bytes through Mem_Alloc (0x004B0C60) with alignment 4, clears the 0x1C-byte header, splices the record into the list whose head word is 0x00943970 through HeapList_Insert (0x004AF8F0), points the record's +0x08 word at the trailing name area, copies the name there (NUL included), and returns the record. No branch and no conditional error path; retail measures the length twice and repeats two zero stores the header clear already performed - both are kept.
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED. The record type and head storage are shared with native_4b1a90.cpp through loaded_file_record.hpp. The +0x04 field is written by HeapList_Insert; no native in the tree reads it yet. GCC inlines the 0x1C-byte clear and calls CRT strlen/memcpy where retail inlines rep sequences, so the emitted body is larger in bytes with the same call order.
#include "igi/retail/qfile/loaded_file_record.hpp"

#include "igi/retail/mem/heap_list.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

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

LoadedFileRecord* NATIVE_DECL QFile_CreateLoadedFileRecord_4B19C0(const char* name)
{
    const std::int32_t length =                                        // 0x004B19C7-0x004B19D4: repne scasb, not ecx
        static_cast<std::int32_t>(std::strlen(name));
    LoadedFileRecord* const record = static_cast<LoadedFileRecord*>(   // 0x004B19D7-0x004B19D8: push 4; push length+0x1C; call 0x004B0C60
        ::igi::retail::mem::Mem_Alloc(length + 0x1C, 4));

    std::memset(record, 0, 0x1CU);                                     // 0x004B19E6-0x004B19EE: mov ecx,7; rep stosd
    record->link_04 = nullptr;                                         // 0x004B19F0: mov dword [ebx+4],eax
    record->next = nullptr;                                            // 0x004B19F3: mov dword [ebx],eax
    ::igi::retail::mem::HeapList_Insert(                               // 0x004B19F5: push ebx; push 0x00943970; call 0x004AF8F0
        static_cast<void*>(&::g_943970), reinterpret_cast<void**>(record));

    char* const name_copy = reinterpret_cast<char*>(record) + 0x1CU;  // 0x004B19FA: lea edx,[ebx+0x1C]
    record->name = name_copy;                                          // 0x004B1A07: mov dword [ebx+8],edx
    const std::size_t copy_length = std::strlen(name) + 1U;            // 0x004B19FD-0x004B1A14: second repne scasb
    std::memcpy(name_copy, name, copy_length);                         // 0x004B1A16-0x004B1A22: rep movsd / rep movsb
    return record;                                                     // 0x004B1A24-0x004B1A26: mov eax,ebx; pops; ret
}

} // extern "C"

#undef NATIVE_DECL
