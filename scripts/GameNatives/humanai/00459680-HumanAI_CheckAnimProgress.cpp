// IGI native provenance: HumanAI_CheckAnimProgress - 0x00459680 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00459680-HumanAI_CheckAnimProgress.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_CheckAnimProgress.hpp"

void* HumanAI_CheckAnimProgress(void* self, void* args)

{
    *(args + 0x20) = *(args + 0xd38) + *(args + 0x20);
    *(args + 0x28) = *(args + 0xd40) + *(args + 0x28);
    *(args + 0x30) = *(args + 0xd48) + *(args + 0x30);

    if (*(self + 8) & 1)
        *(self + 0x14) = HumanAI_CompleteAnimation;

    return self;
}
