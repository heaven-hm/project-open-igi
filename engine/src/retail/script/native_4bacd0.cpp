// Native: Script_QHashSymbolTableInit (placeholder until VERIFIED); retail VA 0x004BACD0-0x004BAD39 (106 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 complete listing (68 87130000 e8.. a3883ea400 33c0 83c404 a3ac3ea400 a38c3ea400 c705a03ea4.. c705a83ea4.. a3a43ea400 c705b03ea4.. c705b83ea4.. a3b43ea400 c705903ea4.. c705983ea4.. a3943ea400 c3); owns all six retail globals it writes; callee 0x4C0360 landed.
// Behavior/CFG: creates the symbol table (4999 buckets via 0x4C0360), stores it at 0x00A43E88, zeroes the counters at 0x00A43EAC/0x00A43E8C, then self-links three embedded sentinel lists (0x00A43EA0/A4/A8, 0x00A43EB0/B4/B8, 0x00A43E90/94/98) with forward=base+4, middle=0, back=base; returns nothing.
// Status/limits: BUILD_VERIFIED (static only); the globals are port-owned storage declared here because this native is their retail writer, and the sentinel self-links use port addresses rather than the retail VAs (documented, unavoidable in a non-address-image port); runtime UNOBSERVED.
#include <cstdint>

extern "C" void* Script_QHashTableCreate(std::int32_t count);

extern "C" {

// Retail storage owned by this init: 0x00A43E88 bucket pointer, 0x00A43E8C / 0x00A43EAC
// counters, and three embedded sentinel lists at 0x00A43E90 / 0x00A43EA0 / 0x00A43EB0.
void* g_a43e88 = nullptr;                                        // 0x00A43E88
std::int32_t g_a43e8c = 0;                                       // 0x00A43E8C
std::int32_t g_a43eac = 0;                                       // 0x00A43EAC

struct SymbolSentinel {                                          // {A0/A4/A8 style triplet}
    std::uint8_t* field_first;                                   // +0
    std::uint8_t* field_middle;                                  // +4
    std::uint8_t* field_back;                                    // +8
};

SymbolSentinel g_a43ea0 = {};                                    // 0x00A43EA0/A4/A8
SymbolSentinel g_a43eb0 = {};                                    // 0x00A43EB0/B4/B8
SymbolSentinel g_a43e90 = {};                                    // 0x00A43E90/94/98

void Script_QHashSymbolTableInit()
{
    g_a43e88 = Script_QHashTableCreate(0x1387);           // 0x004BACD5: bucket count 4999
    g_a43eac = 0;                                                // 0x004BACE4
    g_a43e8c = 0;                                                // 0x004BACE9
    g_a43ea0.field_first = reinterpret_cast<std::uint8_t*>(&g_a43ea0.field_middle);   // 0x004BACEE
    g_a43ea0.field_back = reinterpret_cast<std::uint8_t*>(&g_a43ea0);                 // 0x004BACF8
    g_a43ea0.field_middle = nullptr;                                                  // 0x004BAD02
    g_a43eb0.field_first = reinterpret_cast<std::uint8_t*>(&g_a43eb0.field_middle);   // 0x004BAD07
    g_a43eb0.field_back = reinterpret_cast<std::uint8_t*>(&g_a43eb0);                 // 0x004BAD11
    g_a43eb0.field_middle = nullptr;                                                  // 0x004BAD1B
    g_a43e90.field_first = reinterpret_cast<std::uint8_t*>(&g_a43e90.field_middle);   // 0x004BAD20
    g_a43e90.field_back = reinterpret_cast<std::uint8_t*>(&g_a43e90);                 // 0x004BAD2A
    g_a43e90.field_middle = nullptr;                                                  // 0x004BAD34
}

} // extern "C"
