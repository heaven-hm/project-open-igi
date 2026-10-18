// IGI native provenance: HumanAI_ValidateAnimEntry - 0x00456920 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00456920-HumanAI_ValidateAnimEntry.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_ValidateAnimEntry.hpp"

void* HumanAI_ValidateAnimEntry(void* self, void* args)

{
    *(args + 0x20) = *(args + 0xd28) + *(args + 0x20);
    *(args + 0x28) = *(args + 0xd30) + *(args + 0x28);
    *(args + 0x30) = *(args + 0xd38) + *(args + 0x30);

    if (*(self + 8) & 1)
        *(self + 0x14) = HumanAI_CompleteAnimation;

    return self;
}
