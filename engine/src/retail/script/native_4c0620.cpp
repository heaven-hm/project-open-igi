// Native: Script_QHashSymbolLookup_4C0620; retail VA 0x004C0620 (placeholder name; retail symbol unknown).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; VA 0x004C0620-0x004C065C (61 bytes, 7 blocks, 23 instructions), bytes re-read from the image (8b 44 24 0c 8b 54 24 04 8d 4c 24 0c 50 51 52 e8 ...).
// Tools/evidence: radare2 6.2.0 af/pd over the exact image; the only caller is 0x004BAB96 (0x004BAB80), which pushes three arguments.
// Behavior/CFG: cdecl lookup with three stack arguments ([esp+4] symbol table, [esp+8] node out, [esp+0xC] name). It hands the table, the address of its own third argument word and the name to the already transcribed Script_QHashNodeLookup at 0x004C04B0 (0x004C062C-0x004C062F; the lookup overwrites that argument word with the found record). On a hit it stores record dword 0x0C through the optional node out (0x004C0647/0x004C064A) and returns record dword 8 in EAX (0x004C064C); on a miss it stores null through the optional out (0x004C0654) and returns 0 (0x004C065A).
// Status/limits: BUILD_VERIFIED; runtime UNOBSERVED; STRUCTURALLY_VERIFIED against the instruction listing only. Link: needs Script_QHashNodeLookup 0x004C04B0 (already transcribed in native_4c04b0.cpp). What record dwords 8 and 0x0C mean is not established here; they are copied as raw dwords. The retail reuses its own third argument word as the lookup's out slot; this file uses a local because a C++ body cannot address an incoming argument word, and the value is read back immediately (0x004C063F) before any other use.
#include "dev_log.hpp"
#include <cstdint>

extern "C" std::int32_t Script_QHashNodeLookup(void* list, void** out, char const* value);

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

std::int32_t NATIVE_DECL Script_QHashSymbolLookupRecord_4C0620(void* table, void** out_node, const char* name)
{
    NATIVE_LOG_ENTER("Script_QHashSymbolLookupRecord_4C0620", "0x004C0620");
    void* record = nullptr;
    // 0x004C062C-0x004C062F: Script_QHashNodeLookup(table, &record, name).
    std::int32_t const found = Script_QHashNodeLookup(table, &record, name);

    // 0x004C0637-0x004C063D: branch on the lookup's AL result.
    if (found != 0) {
        if (out_node != nullptr) {                                                              // 0x004C0643-0x004C0645
            *out_node = *reinterpret_cast<void**>(static_cast<std::uint8_t*>(record) + 0x0C);   // 0x004C0647/0x004C064A
        }
        return *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(record) + 8);        // 0x004C064C
    }

    if (out_node != nullptr) {                                                                  // 0x004C0650-0x004C0652
        *out_node = nullptr;                                                                    // 0x004C0654
    }
    return 0;                                                                                   // 0x004C065A
}

} // extern "C"

#undef NATIVE_DECL
