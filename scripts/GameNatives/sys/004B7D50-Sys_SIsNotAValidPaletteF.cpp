// IGI native provenance: Sys_SIsNotAValidPaletteF - 0x004B7D50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B7D50-Sys_SIsNotAValidPaletteF.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7D50-Sys_SIsNotAValidPaletteF.hpp"
int Sys_SIsNotAValidPaletteF(int* context, int* arg2)

{
    int* item_c = arg2;
    int data_val;
    void* entity_val;
    int temp_val;
    int* data_1 = Resource_Load(data_val, temp_val, entity_val, nullptr);

    if (*data_1 == 0x4c && *(data_1 + 1) == 0x4f && *(data_1 + 2) == 0x4f && *(data_1 + 3) == 0x50
        && data_1[1] == 3)
    {
        Texture_UploadPalettePacket(context, &data_1[4], data_1[3]);
        return Resource_Unload(arg2);
    }

    int* item_8_1 = arg2;
    Error_Show("'%s' is not a valid palette file.");

    while (true)
        /* nop */
}
