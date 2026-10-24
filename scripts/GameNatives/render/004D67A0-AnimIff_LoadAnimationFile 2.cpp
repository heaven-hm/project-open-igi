// IGI native provenance: AnimIff_LoadAnimationFile - 0x004D67A0 - CFG score 93.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D67A0-AnimIff_LoadAnimationFile.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AnimIff_LoadAnimationFile.h"
int AnimIff_LoadAnimationFile(int* context)

{
    int* saved_frame = context;
    int data_val;
    void* entity_val;
    int temp_val;
    int* dest_val = Terrain_SampleHeightmap_6(Resource_Load(data_val, temp_val, entity_val, nullptr));

    if (!dest_val)
    {
        int* saved_obj = context;
        return Warning_Show("File %s is not IFF file.");
    }

    void** data_4 = Terrain_BuildMipMaps_5(dest_val, 0x424f424a);

    if (!data_4)
    {
        int* saved_base_1 = context;
        return Warning_Show("IFF File %s is not of correct type.");
    }

    void** data_6 = Terrain_BuildMipMaps_5(data_4, 0x424f4248);
    int* data_8 = Terrain_RenderWaterSurface_6(Terrain_SelectTerrainLOD_6(data_6, 0x424f5348));
    int* context_val = *data_8;
    int flagP = data_8[1];

    if (flagP <= 0)
        Terrain_RenderQuadtree_6(data_6, 0x424f4248);
    else
    {
        int context_2 = context_val * 0xc;
        *(context_2 + &g_a542f0) = flagP;
        int flagP2 = flagP * 0xc;
        *(context_2 + &g_a542f4) = Mem_Alloc(flagP << 2, 4);
        *(context_2 + 0xa542f8) = Mem_Alloc(flagP2, 4);
        void** data_12 = Terrain_SelectTerrainLOD_6(data_6, 0x504c5354);
        void** data_13 = Terrain_SelectTerrainLOD_6(data_6, 0x544c5354);
        int data_14 = Terrain_RenderWaterSurface_6(data_12);
        int entity_1 = flagP << 2;
        int source_3;
        int dest_3;
        dest_3 = memcpy(*(context_2 + &g_a542f4), data_14, entity_1 & 0xfffffffc);
        memcpy(dest_3, source_3, entity_1 & 3);
        int data_17 = Terrain_RenderWaterSurface_6(data_13);
        int source_5;
        int dest_5;
        dest_5 = memcpy(*(context_2 + 0xa542f8), data_17, flagP2 & 0xfffffffc);
        memcpy(dest_5, source_5, flagP2 & 3);
        void** data_19 = Terrain_RenderQuadtree_6(data_6, 0x424f414c);
        Terrain_RenderQuadtree_6(data_19, 0x424f4248);
        void** data_20 = Terrain_SelectTerrainLOD_6(data_19, 0x42414c48);
        int* data_21 = Terrain_RenderWaterSurface_6(data_20);
        int entity_9 = *data_21;
        int source_7 = data_21[1];
        int context_5 = context_val * 0x28;
        *(context_5 + 0xa53914) = source_7;
        int source_8 = source_7 << 2;
        *(context_5 + &g_a538f0) = Mem_Alloc(source_7 * 0x14, 4);
        void* data_23 = Mem_Alloc(source_8, 4);
        *(context_5 + &g_a538f4) = data_23;
        memset(memset(data_23, 0, source_8 & 0xfffffffc), 0, source_8 & 3);
        void** dest_8 = data_20;

        if (entity_9 > 0)
        {
            int item_2c_2 = entity_9;
            int source_10 = flagP << 4;
            int item_24_1 = source_10;

            while (true)
            {
                dest_8 = Terrain_RenderQuadtree_6(dest_8, 0x424f414e);
                void** item_34_1 = dest_8;
                int* data_27 = Terrain_RenderWaterSurface_6(Terrain_SelectTerrainLOD_6(dest_8, 0x424f4148));
                int temp_6 = *data_27;
                int data_28 = data_27[1];
                int flagP4 = data_27[2];
                *(*(context_5 + &g_a538f4) + (flagP4 << 2)) = *(context_5 + &g_a538f0) + flagP4 * 0x14;
                **(*(context_5 + &g_a538f4) + (flagP4 << 2)) = temp_6;
                *(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 4) = data_28;
                *(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) = Mem_Alloc(8, 4);
                *(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) = Mem_Alloc(source_10, 4);
                *(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0x10) = 0;
                void** data_32 = Terrain_SelectTerrainLOD_6(dest_8, 0x424f5448);
                int source_11 = *Terrain_RenderWaterSurface_6(data_32);
                **(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) = source_11;
                void** data_44;

                if (source_11 <= 0)
                    data_44 = data_32;
                else
                {
                    void** data_35 = Terrain_SelectTerrainLOD_6(dest_8, 0x424f5444);
                    int entity_26;
                    int source_12;
                    int dest_10;

                    if (source_11 <= 1)
                    {
                        int source_13 = source_11 << 4;
                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) + 4) =
                            Mem_Alloc(source_13, 4);
                        source_12 = Terrain_RenderWaterSurface_6(data_35);
                        dest_10 = *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) + 4);
                        entity_26 = source_13;
                    }
                    else
                    {
                        int data_37 = source_11 * 0x28;
                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) + 4) =
                            Mem_Alloc(data_37, 4);
                        source_12 = Terrain_RenderWaterSurface_6(data_35);
                        dest_10 = *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 8) + 4);
                        entity_26 = data_37;
                    }

                    data_44 = data_35;
                    int source_14;
                    int dest_11;
                    dest_11 = memcpy(dest_10, source_12, entity_26 & 0xfffffffc);
                    memcpy(dest_11, source_14, entity_26 & 3);
                    dest_8 = item_34_1;
                }

                if (flagP > 0)
                {
                    int dest_12 = 0;
                    int item_30_4 = flagP;
                    int item_38_2 = 0;

                    while (true)
                    {
                        int* data_45 = Terrain_UpdateWaterWaves_6(data_44, 0x424f5248);
                        int source_15 = *Terrain_RenderWaterSurface_6(data_45);
                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12) = source_15;

                        if (source_15 <= 1)
                            *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 4) =
                                Mem_Alloc(source_15 * 0x14, 4);
                        else
                            *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 4) =
                                Mem_Alloc(source_15 * 0x34, 4);

                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 8) = 0;
                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 0xc) = 0;
                        int* data_55 = Terrain_UpdateWaterWaves_6(data_45, 0x424f5244);
                        int entity_49;
                        int source_19;
                        int dest_14;

                        if (source_15 <= 1)
                        {
                            int source_21 = source_15 * 0x14;
                            int data_59 = Terrain_RenderWaterSurface_6(data_55);
                            dest_14 = memcpy(
                                *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 4),
                                data_59, source_21 & 0xfffffffc);
                            entity_49 = source_21;
                        }
                        else
                        {
                            int source_17 = source_15 * 0x34;
                            int data_56 = Terrain_RenderWaterSurface_6(data_55);
                            dest_14 = memcpy(
                                *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0xc) + dest_12 + 4),
                                data_56, source_17 & 0xfffffffc);
                            entity_49 = source_17;
                        }

                        data_44 = data_55;
                        memcpy(dest_14, source_19, entity_49 & 3);
                        item_38_2 += 0x10;
                        bool flag_7_1 = item_30_4 != 1;
                        item_30_4 -= 1;

                        if (!flag_7_1)
                            break;

                        dest_12 = item_38_2;
                    }

                    dest_8 = item_34_1;
                }

                int source_23 = *Terrain_RenderWaterSurface_6(Terrain_SelectTerrainLOD_6(dest_8, 0x424f4548));

                if (source_23 > 0)
                {
                    *(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0x10) = Mem_Alloc(8, 4);
                    int data_66 = source_23 * 0x18;
                    **(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0x10) = source_23;
                    *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0x10) + 4) =
                        Mem_Alloc(data_66, 4);
                    int data_69 = Terrain_RenderWaterSurface_6(Terrain_SelectTerrainLOD_6(dest_8, 0x424f4544));
                    int source_25;
                    int dest_17;
                    dest_17 = memcpy(
                        *(*(*(*(context_5 + &g_a538f4) + (flagP4 << 2)) + 0x10) + 4), data_69,
                        data_66 & 0xfffffffc);
                    memcpy(dest_17, source_25, data_66 & 3);
                    dest_8 = item_34_1;
                }

                bool flag_4_1 = item_2c_2 != 1;
                item_2c_2 -= 1;

                if (!flag_4_1)
                    break;

                source_10 = item_24_1;
            }
        }
    }

    Render_DrawSplineMesh_11(context_val);
    Terrain_CalculateNormals_5(dest_val);
    return Resource_Unload(context);
}
