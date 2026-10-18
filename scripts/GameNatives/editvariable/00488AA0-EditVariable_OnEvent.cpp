// IGI native provenance: EditVariable_OnEvent - 0x00488AA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00488AA0-EditVariable_OnEvent.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00488AA0-EditVariable_OnEvent.hpp"
int EditVariable_OnEvent(void* context)

{
    Script_Isexists(context + 0x38, context);
    Script_Isexists(context + 0x88, context);
    return 0;
}
