// IGI native provenance: MenuItem_Move - 0x00505A10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00505A10-MenuItem_Move.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuItem_Move.h"

void* MenuItem_Move(void* self, int* args)

{
    if (*(self + 0x38) == *args)
        args[1] = self;

    return self;
}
