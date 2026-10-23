// IGI native provenance: ExplodeObject_Action - 0x00481F90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00481F90-ExplodeObject_Action.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00481F90-ExplodeObject_Action.hpp"
int ExplodeObject_Action(void* context)

{
    char result = *(context + 0x191);

    if (result)
        return result;

    *(context + 0x191) = 1;
    void item_c0;
    Script_ParseEnumString_6(&item_c0, context);
    void* item_d0_1 = &item_c0;
    void item_80;
    sprintf(&item_80, "%s.isExploded");
    return SymbolRegister_Bool8(&item_80, context + 0x190);
}
