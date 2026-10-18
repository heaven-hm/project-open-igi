// IGI native provenance: MagicObj_ValidateVirModel - 0x004D99E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004D99E0-MagicObj_ValidateVirModel.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MagicObj_ValidateVirModel.h"

int MagicObj_ValidateVirModel(int self)

{
    return (*((DynObj_LookupHashItem_5() << 2) + &g_a982e0))(self, 0);
}
