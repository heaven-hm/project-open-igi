// IGI native provenance: FlashBang_LoadParameters - 0x0047E790 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047E790-FlashBang_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047E790-FlashBang_LoadParameters.hpp"
struct status_msg* FlashBang_LoadParameters(void* context)

{
    unsigned int data_1 = g_5c114c + 0x18;
    int item_c = 0;
    int item_8 = 0;
    int item_4 = 0;
    int item_20;
    memcpy(&item_20,
        "\x9a\x99\x19\x45\x9a\x99\x19\x45\x00\x00\xfe\x42\x00\x00\xfe\x42\x00\x00\xfe\x42", 0x14);
    void* data_2 = Level_StartMission_4();
    int item_18;
    struct status_msg* result = Script_FormatString_30(Script_FormatString(), data_2, data_1, &item_c, &item_18, &item_20,
        0x49480000, 0x1000008, 0, 0x3c6eeeef, 0, 0x14, 0);
    *(context + 0x38) = result;
    return result;
}
