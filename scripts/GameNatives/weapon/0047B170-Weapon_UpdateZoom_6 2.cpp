// IGI native provenance: Weapon_UpdateZoom_6 - 0x0047B170 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047B170-Weapon_UpdateZoom_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Weapon_UpdateZoom_6.h"
int* Weapon_UpdateZoom_6(int* context, int arg2)

{
    if (!context[0x25])
        return QTask_FindType(context);

    context[0x1a] = arg2;
    return context;
}
