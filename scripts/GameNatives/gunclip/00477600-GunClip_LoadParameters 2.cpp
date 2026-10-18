// IGI native provenance: GunClip_LoadParameters - 0x00477600 - CFG score 87.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 00477600-GunClip_LoadParameters.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunClip_LoadParameters.h"
int GunClip_LoadParameters(void* context, int* arg2)

{
    DynObj_QtasklistIsFull(Qtask_GetList(), context);
    void* data_2 = DynObj_Missing(*arg2);

    if (data_2)
    {
        memcpy(context + 0xf0, &arg2[8], 0x18);
        memcpy(context + 0x108, &arg2[0x18], 0x28);
        DynObj_LookupHashItem_6(arg2[1], context, data_2);
        memcpy(context + 0x70, &arg2[0xe], 0x28);
        int result = arg2[0x23];
        *(context + 0x138) = result;
        *(context + 0x130) = arg2[0x22];
        *(context + 0x13c) = 0xffffffff;
        return result;
    }

    int saved_source_1 = *arg2;
    Error_Show("VirModel "%s" not available");

    while (true)
        /* nop */
}
