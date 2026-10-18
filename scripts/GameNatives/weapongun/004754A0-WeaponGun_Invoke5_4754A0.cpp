// IGI native provenance: WeaponGun_Invoke5_4754A0 - 0x004754A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004754A0-WeaponGun_Invoke5_4754A0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004754A0-WeaponGun_Invoke5_4754A0.hpp"
int WeaponGun_Invoke5_4754A0(int arg1)

{
    return (*((Weapon_ComputeAccuracy_5() << 2) + &g_a976e0))(arg1);
}
