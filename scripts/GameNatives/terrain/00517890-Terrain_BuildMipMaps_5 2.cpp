// IGI native provenance: Terrain_BuildMipMaps_5 - 0x00517890 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00517890-Terrain_BuildMipMaps_5.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_BuildMipMaps_5.h"
void** Terrain_BuildMipMaps_5(void** context, int arg2)

{
    for (void** i = context; i; i = *i)
    {
        int data_2 = i[5];

        if (i[4] == 0x464f524d)
        {
            if (data_2 == arg2)
                return i;

            void** data_3 = i[2];

            if (data_3)
            {
                void** data_4 = Terrain_BuildMipMaps_5(data_3, arg2);

                if (data_4)
                    return data_4;
            }
        }
        else if (data_2 == arg2)
            return i;
    }

    return nullptr;
}
