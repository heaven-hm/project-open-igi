// IGI native provenance: AI_FireWeapon_4 - 0x00452BE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00452BE0-AI_FireWeapon_4.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_4.h"
int* AI_FireWeapon_4(int context, int arg2)

{
    int* result = g_57d768;

    for (int* i = *result; i; i = *i)
    {
        if (result[2] == context && result[3] == arg2)
            return result;

        result = i;
    }

    return nullptr;
}
