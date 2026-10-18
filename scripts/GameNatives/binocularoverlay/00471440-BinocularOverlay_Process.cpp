// IGI native provenance: BinocularOverlay_Process - 0x00471440 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00471440-BinocularOverlay_Process.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00471440-BinocularOverlay_Process.hpp"
int BinocularOverlay_Process(void* context)

{
    Display_CloseDevice_15(context + 0x88, context + 0x38, context + 0x60, *(context + 0x78));
    Transcontext_SetActiveTransContext(context + 0x88);
    int result = Weapon_WeaponsStringsBinocu(context);
    Display_SwapBuffers_15();
    return result;
}
