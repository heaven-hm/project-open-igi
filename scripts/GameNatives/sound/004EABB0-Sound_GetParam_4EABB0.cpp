// IGI native provenance: Sound_GetParam_4EABB0 - 0x004EABB0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EABB0-Sound_GetParam_4EABB0.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sound_GetParam_4EABB0.hpp"

int Sound_GetParam_4EABB0(int arg1, int arg2)

{
    __builtin_memcpy(arg1 + 0xa0, arg2, 0x18);
    return arg1;
}
