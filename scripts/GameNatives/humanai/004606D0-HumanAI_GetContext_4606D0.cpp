// IGI native provenance: HumanAI_GetContext_4606D0 - 0x004606D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004606D0-HumanAI_GetContext_4606D0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_GetContext_4606D0.hpp"

int* HumanAI_GetContext_4606D0(int arg1, int* arg2)

{
    *arg2 = arg1 + 0x20;
    arg2[1] = arg1 + 0x78;
    arg2[2] = 0;
    return arg2;
}
