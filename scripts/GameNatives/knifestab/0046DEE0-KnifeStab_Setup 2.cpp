// IGI native provenance: KnifeStab_Setup - 0x0046DEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0046DEE0-KnifeStab_Setup.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "KnifeStab_Setup.h"

int KnifeStab_Setup(void* self)

{
    return QTask_FindType(*(self + 0xe8));
}
