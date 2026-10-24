// IGI native provenance: WeaponGun_Invoke9_474F10 - 0x00474F10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00474F10-WeaponGun_Invoke9_474F10.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00474F10-WeaponGun_Invoke9_474F10.hpp"
int WeaponGun_Invoke9_474F10(int* arg1)

{
    void* result_val = *arg1;
    return FX_FireBallUpdate_9(result_val, result_val + 0x434);
}
