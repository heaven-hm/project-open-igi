// IGI native provenance: Vehicle_DeployGear_10 - 0x0043F820 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043F820-Vehicle_DeployGear_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043F820-Vehicle_DeployGear_10.hpp"
float* Vehicle_DeployGear_10(float* context, double* arg2)

{
    double item_18;
    WeaponType_UpdateAnim(&item_18, arg2, 0);
    double item_30;
    WeaponType_UpdateAnim(&item_30, arg2, 1);
    double item_48 = item_30 - item_18;
    double item_28;
    double item_10;
    double item_40 = item_28 - item_10;
    double item_20;
    double item_8;
    double item_38 = item_20 - item_8;
    Sys_FreeMemory_11(&item_48);
    float item_54 = item_48;
    float item_50 = item_40;
    float item_4c = item_38;
    return WeaponType_SetCategory(context, &item_54);
}
