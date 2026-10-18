// IGI native provenance: HumanAI_ProcessAnimFrame - 0x0045C700 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0045C700-HumanAI_ProcessAnimFrame.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanAI_ProcessAnimFrame.hpp"

void* HumanAI_ProcessAnimFrame(void* self, void* args)

{
    *(args + 0x20) = *(args + 0xd38) + *(args + 0x20);
    *(args + 0x28) = *(args + 0xd40) + *(args + 0x28);
    *(args + 0x30) = *(args + 0xd48) + *(args + 0x30);

    if (*(self + 8) & 1)
        *(self + 0x14) = HumanAI_CompleteAnimation;

    return self;
}
