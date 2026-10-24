// Native: QFile_FindRecordAtAddress_4B1A90; retail VA 0x004B1A90 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004B1A90-0x004B1ABC (45 bytes, 6 blocks, 8 edges, 17 instructions).
// Tools/evidence: radare2 6.2.0 af/pdf and the raw bytes a170399400 8b542404 8b08 85c9 / 395018 / 8bc1 8b09 85c9 / 52 68706f5400 e8f9dcffff 83c408 ebfe / c3; section 22.3 of the evidence sheet (landability sweep row 0x004B1A90).
// Behavior/CFG: walks the singly linked list whose head pointer is 0x00943970, comparing each record's +0x18 word against the argument. The walk requires the current record's next pointer to be non-null before its own +0x18 is compared, which is retail's order: the next pointer is loaded before the comparison, so the list must be circular or the last record is never compared - that is retail's shape and it is kept. On a match the routine returns without loading a value; when the walk ends it calls Debug_Error("No files loaded at address: %p", address) and spins at 0x004B1ABA.
// Status/limits: TRANSCRIBED; runtime UNOBSERVED. The head word 0x00943970 has no other owner in the tree, so this file defines it and it starts null; retail's head is set elsewhere, and this routine dereferences it with no null check exactly as retail does. The record layout beyond +0x00 and +0x18 is not established, so the middle is opaque padding.
#include "igi/retail/core/retail_error.hpp"

#include <cstdint>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

// A loaded-file record as this routine indexes it: the next pointer at +0x00 and the address word at +0x18.
struct LoadedFileRecord {
    LoadedFileRecord* next;   // +0x00
    std::uint8_t opaque[0x14]; // +0x04..+0x17: not touched here, layout not established
    std::uint32_t address;    // +0x18
};

// 0x00943970: head pointer this routine walks from.
LoadedFileRecord* g_943970;

void NATIVE_DECL QFile_FindRecordAtAddress_4B1A90(std::uint32_t address)
{
    LoadedFileRecord* current = ::g_943970;              // 0x004B1A90: eax = [0x00943970]
    LoadedFileRecord* next = current->next;              // 0x004B1A99: ecx = [eax]

    if (next != nullptr)                                 // 0x004B1A9D: test ecx,ecx; je 0x004B1AAC
    {
        for (;;)
        {
            if (current->address == address)             // 0x004B1A9F: cmp dword [eax+0x18], edx; je 0x004B1ABC
            {
                return;                                  // 0x004B1ABC: ret, no value loaded
            }
            current = next;                              // 0x004B1AA4: mov eax, ecx
            next = next->next;                           // 0x004B1AA6: mov ecx, [ecx]
            if (next == nullptr)                         // 0x004B1AAA: test ecx,ecx; jne 0x004B1A9F
            {
                break;
            }
        }
    }

    // 0x004B1AAC-0x004B1AB7: push address; push "No files loaded at address: %p"; call Debug_Error; add esp,8.
    Debug_Error("No files loaded at address: %p", reinterpret_cast<void*>(static_cast<std::uintptr_t>(address)));
    for (;;)                                             // 0x004B1ABA: jmp 0x004B1ABA
    {
    }
}

} // extern "C"

#undef NATIVE_DECL
