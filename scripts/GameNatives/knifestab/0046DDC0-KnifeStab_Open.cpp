// IGI native provenance: KnifeStab_Open - 0x0046DDC0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046DDC0-KnifeStab_Open.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0046DDC0-KnifeStab_Open.hpp"
int* KnifeStab_Open(void* self, int* args)

{
    int* result_val_1 = DynObj_Missing(*args);

    if (!result_val_1)
    {
        int local_14_1 = *args;
        Warning_Show("VirModel "%s" not available");
    }

    memcpy(self + 0xf0, &args[8], 0x18);
    memcpy(self + 0x108, &args[0x18], 0x28);
    DynObj_LookupHashItem_6(args[1], self, result_val_1);
    *(self + 0xec) = 0;
    memcpy(self + 0x70, &args[0xe], 0x28);
    unsigned int frame_ptr_1 = args[0x22];
    *(self + 0x130) = frame_ptr_1;
    struct status_msg* result_val_3 = CarGun_Create(nullptr, frame_ptr_1);
    int counter_val_2 = *(self + 0x130);
    *(self + 0xe8) = result_val_3;
    return Car_GunCreate(result_val_3, self, counter_val_2);
}
