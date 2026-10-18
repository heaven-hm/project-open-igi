// IGI native provenance: HumanAI_FindAnimation - 0x004547B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004547B0-HumanAI_FindAnimation.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_FindAnimation.hpp"

void* HumanAI_FindAnimation(void* self, void* args)

{
    *(args + 0x20) = *(args + 0xd38) + *(args + 0x20);
    *(args + 0x28) = *(args + 0xd40) + *(args + 0x28);
    *(args + 0x30) = *(args + 0xd48) + *(args + 0x30);

    if (*(self + 8) & 1)
        *(self + 0x14) = HumanAI_CompleteAnimation;

    return self;
}
