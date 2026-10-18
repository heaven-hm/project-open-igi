// IGI native provenance: Weapon_UpdateZoom - 0x004712E0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004712E0-Weapon_UpdateZoom.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004712E0-Weapon_UpdateZoom.hpp"
void* Weapon_UpdateZoom(void* context, int arg2, int arg3)

{
    memset(context, 0, 0x14);
    *(context + 0x10) = arg2;
    *(context + 8) = arg3;
    return context;
}
