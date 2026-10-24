// IGI native provenance: Cabinet_Init_2 - 0x00443D80 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00443D80-Cabinet_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00443D80-Cabinet_Init_2.hpp"
void* Cabinet_Init_2(void* self)

{
    *(self + 0x238) = 1;
    *(self + 0x23c) = *(g_bc2360 + 0x68);
    return self;
}
