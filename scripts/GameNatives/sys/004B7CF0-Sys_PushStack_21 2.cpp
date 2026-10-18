// IGI native provenance: Texture_UploadPalettePacket - 0x004B7CF0 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B7CF0-Texture_UploadPalettePacket.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Texture_UploadPalettePacket.h"
int Texture_UploadPalettePacket(int* context, int arg2, int arg3)

{
    int data_val = Sys_ReadFileBlock_3();
    int* item_c = context;
    *context = data_val;
    return (*((g_a84a48 << 2) + &g_a94e84))(&item_c, item_c, arg3, arg2);
}
