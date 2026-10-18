// IGI native provenance: EditVariable_Create - 0x004886F0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004886F0-EditVariable_Create.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004886F0-EditVariable_Create.hpp"
void* EditVariable_Create(void* self)

{
    *(self + 0x20) = 0;
    return self;
}
