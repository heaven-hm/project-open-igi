// IGI native provenance: EditorMagicObj_Init_2 - 0x0051B940 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0051B940-EditorMagicObj_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "EditorMagicObj_Init_2.h"

void* EditorMagicObj_Init_2(void* self)

{
    return DynObj_RegisterSymbol_9(*(self + 0x68), self);
}
