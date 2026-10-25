// Native: Script_QHashSymbolRegister (placeholder until VERIFIED); retail VA 0x004C0560-0x004C061F (192 bytes, 4 blocks).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (53 8b5c240c 55 8b6c240c 8d442410 53 50 55 e8.. 83c40c 84c0 7410 53 68..e828f2feff 83c408 ebfe / 56 57 8bfb 83c9ff 33c0 6a04 f2ae f7d1 83c110 51 e8bb06ffff 8d4810 89442420 894808 ... 8bfb 89420c 83c9ff ... f3a5 ... f3a4 c7410400.. c70200.. 8b4504 50 53 e8.. / 51 8d0c90 51 e8fff2feff ... 8b4208); both call sites push three dwords; callees 0x4C04B0/0x4C0530/0x4AF910/Mem_Alloc landed.
// Behavior/CFG: looks the symbol up via 0x4C04B0; if it already exists it calls Debug_Error("Symbol %s already registered", value) and then spins at 0x004C058B (fail-closed, preserved). Otherwise it measures the string, allocates strlen+0x11 bytes via Mem_Alloc(size,4), stores key = record+0x10 at record+8, stores the owner argument at record+0xC, copies the string with its terminator to record+0x10, zeroes record+0 and record+4, hashes the value with 0x4C0530 against list+4, links the record into bucket (hash*3) via 0x4AF910 and returns record->key.
// Status/limits: BUILD_VERIFIED (static only); the strlen and copy are written as explicit loops (retail uses repne scasb and a rep movsd/movsb pair) so the key-copy shape is behavioural, not instruction-identical; record field order and the spin are retail; runtime UNOBSERVED.
#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/mem/mem_alloc.hpp"

#include <cstdint>

extern "C" std::int32_t Script_QHashNodeLookup(void* list, void** out, char const* value);
extern "C" std::int32_t Script_Hash32(char const* text, std::int32_t bucket_count);
extern "C" void Script_QHashListInsert(void* head, void* node);

extern "C" {

void* Script_QHashSymbolRegister(void* const list, char const* const value, void* const owner)
{
    void* existing = nullptr;                                                       // stack out slot
    if (Script_QHashNodeLookup(list, &existing, value) != 0) {                 // 0x004C0571..0x004C057B
        Debug_Error("Symbol %s already registered", value);                          // 0x004C0583: symbol message
        for (;;) { }                                                                 // 0x004C058B: jmp $
    }
    std::int32_t length = 0;                                                         // 0x004C0591..0x004C059C: repne scasb/not ecx
    while (value[length] != 0)
        length += 1;
    std::uint8_t* const record = static_cast<std::uint8_t*>(igi::retail::mem::Mem_Alloc(length + 0x11, 4)); // 0x004C05A0
    std::uint8_t* const key = record + 0x10;                                         // 0x004C05A5: lea ecx,[eax+0x10]
    *reinterpret_cast<std::uint8_t**>(record + 8) = key;                             // 0x004C05AC
    *reinterpret_cast<void**>(record + 0x0C) = owner;                                // 0x004C05B9: owner stored
    for (std::int32_t i = 0; i <= length; i += 1)                                    // 0x004C05D5/0x004C05DC: rep movsd/movsb
        key[i] = static_cast<std::uint8_t>(value[i]);
    *reinterpret_cast<std::int32_t*>(record + 4) = 0;                                // 0x004C05E2
    *reinterpret_cast<std::int32_t*>(record) = 0;                                    // 0x004C05ED
    std::int32_t const bucket_count = *reinterpret_cast<std::int32_t*>(static_cast<std::uint8_t*>(list) + 4); // 0x004C05F3
    std::int32_t const index = Script_Hash32(value, bucket_count);            // 0x004C05F8
    std::uint8_t* const buckets = *reinterpret_cast<std::uint8_t**>(list);           // 0x004C0604
    Script_QHashListInsert(buckets + index * 12, record);                     // 0x004C060C
    return *reinterpret_cast<void**>(record + 8);                                    // 0x004C0618: mov eax,[edx+8]
}

} // extern "C"
