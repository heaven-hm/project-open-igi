// IGI native provenance: VehiclePhysics_GetContext_432BB0 - 0x00432BB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00432BB0-VehiclePhysics_GetContext_432BB0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00432BB0-VehiclePhysics_GetContext_432BB0.hpp"
int* VehiclePhysics_GetContext_432BB0(void* arg1, int* arg2)

{
    arg2[1] = 0;
    *arg2 = arg1 + 0x20;
    arg2[2] = *(arg1 + 0x748);
    return arg2;
}
