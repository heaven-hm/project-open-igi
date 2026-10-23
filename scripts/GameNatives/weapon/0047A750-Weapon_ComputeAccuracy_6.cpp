// IGI native provenance: Weapon_ComputeAccuracy_6 - 0x0047A750 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047A750-Weapon_ComputeAccuracy_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047A750-Weapon_ComputeAccuracy_6.hpp"
int Weapon_ComputeAccuracy_6(int context, float arg2)

{
    int dest_val = 0;
    int* context_val = *(context + 0x12c);
    int* item_1c = nullptr;
    int data_val = g_c28b44;
    int entity_val = g_c28b48;
    int flagP = *(context + 0x144);
    void* source_1 = *(context + 0x14);
    unsigned int data_2 = Weapon_ReloadClip_8();
    int data_3 = 0;
    data_3 = *(source_1 + 0x1c);
    int data_4 = *(((data_2 * 0x180 + data_3) << 2) + &g_a96ae0);

    if (data_4)
        data_4(source_1, &item_1c);

    int* data_6 = Weapon_UpdateMuzzleFlash_8(item_1c, *context_val);

    if (data_6)
        dest_val = data_6[1];

    int context_1 = 7 - flagP;

    if (7 - flagP < 0)
        context_1 = 0;

    int item_30_3 = dest_val;
    void item_10;
    sprintf(&item_10, "%-4d");
    float source_2 = arg2;
    Sys_FreeMemory_18(*(source_2 + 0x4e0), &item_10);
    arg2 = data_val * 0.984375f - 4.0f;
    float item_18_2 = entity_val * 0.895833313f + 4.0f;
    Sys_ResolveSymbol_14(source_2 + 0x11c, arg2 - 8.0f - Sys_CloseFileStream_17(source_2 + 0x11c) - Sys_CheckHeap_21(*(source_2 + 0x4e0)),
        item_18_2, 128.0f, 128.0f, 128.0f, context_1, 0, 0);
    *(*(source_2 + 0x4e0) + 4) = arg2 - Sys_CheckHeap_21(*(source_2 + 0x4e0));
    *(*(source_2 + 0x4e0) + 8) = item_18_2;
    return Sys_CloseFileStream_18(*(source_2 + 0x4e0));
}
