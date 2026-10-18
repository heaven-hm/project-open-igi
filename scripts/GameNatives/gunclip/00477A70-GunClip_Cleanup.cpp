// IGI native provenance: GunClip_Cleanup - 0x00477A70 - CFG score 85.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 00477A70-GunClip_Cleanup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00477A70-GunClip_Cleanup.hpp"
int* GunClip_Cleanup(void* context, int* arg2)

{
    void* data_1 = DynObj_Missing(*arg2);

    if (data_1)
    {
        memcpy(context + 0xf0, &arg2[8], 0x18);
        memcpy(context + 0x108, &arg2[0x18], 0x28);
        DynObj_LookupHashItem_6(arg2[1], context, data_1);
        *(context + 0xec) = 0;
        memcpy(context + 0x70, &arg2[0xe], 0x28);
        *(context + 0x138) = arg2[0x23];
        int flagP1 = arg2[0x22];
        *(context + 0x130) = flagP1;
        return Car_GunCreate(context, context, flagP1);
    }

    int saved_base_1 = *arg2;
    Error_Show("VirModel "%s" not available");

    while (true)
        /* nop */
}
