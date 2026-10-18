// IGI native provenance: SpriteParticleSystemDynCubeObj_Action - 0x00531250 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00531250-SpriteParticleSystemDynCubeObj_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00531250-SpriteParticleSystemDynCubeObj_Action.hpp"
int SpriteParticleSystemDynCubeObj_Action(int context, int* arg2)

{
    DynObj_RegisterSymbol_9(context, arg2);
    int* source_val = g_afa814;
    int entity_1 = *(arg2[0x1e] + source_val);
    int data_1 = g_afa808 - 1;
    g_afa808 = data_1;

    if (entity_1 != data_1)
    {
        int temp_1 = g_afa804;
        int data_4 = *(temp_1 + (data_1 << 2)) * g_afa810 + g_afa800;
        int dest_2 = *(temp_1 + (entity_1 << 2));
        int source_1 = *(source_val + data_4);
        *(temp_1 + (entity_1 << 2)) = *(temp_1 + (source_1 << 2));
        *(g_afa804 + (source_1 << 2)) = dest_2;
        *(g_afa814 + data_4) = entity_1;
    }

    return QTask_FindType(arg2);
}
