// IGI native provenance: Terrain_RenderQuadtree_6 - 0x005178E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 005178E0-Terrain_RenderQuadtree_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Terrain_RenderQuadtree_6.hpp"
void** Terrain_RenderQuadtree_6(void** context, int arg2)

{
    void** i = context;

    if (i[5] == arg2)
        i = *i;

    for (; i; i = *i)
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
