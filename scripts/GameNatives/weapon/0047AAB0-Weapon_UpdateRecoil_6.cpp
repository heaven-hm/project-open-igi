// IGI native provenance: Weapon_UpdateRecoil_6 - 0x0047AAB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047AAB0-Weapon_UpdateRecoil_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047AAB0-Weapon_UpdateRecoil_6.hpp"
int Weapon_UpdateRecoil_6(float* context, float* arg2, int arg3, int arg4)

{
    int source_val = arg3;
    int dest_val = 0;
    int* flagP = *(source_val + 0x128);
    int* context_val = *(source_val + 0x12c);
    arg3 = 0;
    int data_val = g_c28b44;
    int entity_val = g_c28b48;
    void* source_1 = *(source_val + 0x14);
    unsigned int data_2 = Weapon_ReloadClip_8();
    int data_3 = 0;
    data_3 = *(source_1 + 0x1c);
    int data_4 = *(((data_2 * 0x180 + data_3) << 2) + &g_a96ae0);

    if (data_4)
        data_4(source_1, &arg3);

    int* data_6 = Weapon_UpdateMuzzleFlash_8(arg3, *context_val);

    if (data_6)
        dest_val = data_6[1];

    int item_2c_3 = dest_val;
    void item_10;
    Game_DataSymbolLoad(&item_10, "%-4d");
    Sys_FreeMemory_18(*(arg4 + 0x4e0), &item_10);
    void* data_9 = arg4 + 0x14c;

    if (*flagP != 0xc)
        data_9 = arg4 + 0xec;

    *context = data_val * 0.984375f - 4.0f - 8.0f;
    *arg2 = entity_val * 0.895833313f + 4.0f;
    long double fpu_val_7_7 = Sys_CloseFileStream_17(data_9);
    *context = *context - fpu_val_7_7;
    int result = Sys_CheckHeap_21(*(arg4 + 0x4e0));
    arg3 = result;
    *context = *context - arg3;
    return result;
}
