// IGI native provenance: SoundSysController_SetParent - 0x004E6000 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6000-SoundSysController_SetParent.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004E6000-SoundSysController_SetParent.hpp"
int SoundSysController_SetParent(int self, int args)

{
    int arg1 = args;
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *arg1;
        arg1 += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    int result = ~i;
    int source_ptr_1;
    int dest_ptr_2;
    dest_ptr_2 = memcpy(self + 0x20, arg1 - result, result & 0xfffffffc);
    memcpy(dest_ptr_2, source_ptr_1, result & 3);
    return result;
}
