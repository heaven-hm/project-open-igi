// IGI native provenance: Sector_Setup - 0x004DE6B0 - CFG score 85.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004DE6B0-Sector_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004DE6B0-Sector_Setup.hpp"
int Sector_Setup(unsigned int context, int* arg2)

{
    DynObj_LookupHashItem_6(arg2[1], context, 0);
    memcpy(context + 0x20, &arg2[2], 0x18);
    memcpy(context + 0x70, &arg2[0xe], 0x28);
    *(context + 0x6c) = DynObj_Missing(*arg2);
    struct status_msg* data_2 = CarGun_Create(nullptr, context);
    *(context + 0xe8) = data_2;
    Car_GunCreate(data_2, context, context);
    memcpy(context + 0xf0, &arg2[8], 0x18);
    memcpy(context + 0x108, &arg2[0x18], 0x28);
    return Sys_FreeMemory_2(context + 0x130, 4, 0x64, 0);
}
