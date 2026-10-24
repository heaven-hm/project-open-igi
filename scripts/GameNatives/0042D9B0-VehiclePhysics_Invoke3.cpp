// IGI native provenance: VehiclePhysics_Invoke3 - 0x0042D9B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0042D9B0-VehiclePhysics_Invoke3.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0042D9B0-VehiclePhysics_Invoke3.hpp"
int VehiclePhysics_Invoke3(void* arg1, int arg2)

{
    void* source_ptr = *(arg1 + 0x130);
    unsigned int eax_1 = Level_LoadLevel_3();
    int data_val = 0;
    data_val = *(source_ptr + 0x1c);
    int result = *(((eax_1 * 0x180 + data_val) << 2) + &g_a96ae0);

    if (!result)
        return result;

    return result(source_ptr, arg2);
}
