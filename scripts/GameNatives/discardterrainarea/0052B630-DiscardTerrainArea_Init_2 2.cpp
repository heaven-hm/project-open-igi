// IGI native provenance: DiscardTerrainArea_Init_2 - 0x0052B630 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0052B630-DiscardTerrainArea_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DiscardTerrainArea_Init_2.h"

void* DiscardTerrainArea_Init_2(int self, void* args)

{
    memcpy(args, self + 0x28, 0x18);
    *(args + 0x18) = 0;
    *(args + 0x1c) = 0;
    *(args + 0x20) = 0;
    *(args + 0x24) = 0x3f800000;
    *(args + 0x28) = 0;
    *(args + 0x2c) = 0;
    *(args + 0x30) = 0;
    *(args + 0x34) = 0x3f800000;
    *(args + 0x38) = 0;
    *(args + 0x3c) = 0;
    *(args + 0x40) = 0;
    *(args + 0x44) = 0x3f800000;
    *(args + 0x48) = 0;
    int source_ptr_1 = 0x40000000 >> (*(self + 0x70) & 0x1f);
    *(args + 0x50) = 0;
    *(args + 0x4c) = source_ptr_1 * 4.0f;
    return args;
}
