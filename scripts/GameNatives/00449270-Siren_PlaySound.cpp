// IGI native provenance: Siren_PlaySound - 0x00449270 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00449270-Siren_PlaySound.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00449270-Siren_PlaySound.hpp"
int Siren_PlaySound(void* self, int* args)

{
    *args = self + 0x20;
    args[1] = self + 0x70;
    int result = *(self + 0x178);
    args[2] = result;
    return result;
}
