// IGI native provenance: LightmapInfo_Update - 0x00525380 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00525380-LightmapInfo_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "LightmapInfo_Update.hpp"
int* LightmapInfo_Update(void* context)

{
    int* flagP = *(context + 0xdc);
    int* result = flagP[8];

    if (!result)
    {
        result = *(context + 0x6c);
        int i = 0;

        if (result[0xf] > 0)
        {
            int* dest_1 = flagP;

            do
            {
                result = Light_ComputeDirectionalLight_5(dest_1);
                i += 1;
                dest_1 = &dest_1[9];
            } while (i < *(*(context + 0x6c) + 0x3c));
        }

        flagP[8] = 1;
    }

    return result;
}
