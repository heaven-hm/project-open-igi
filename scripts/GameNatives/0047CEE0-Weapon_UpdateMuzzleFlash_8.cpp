// IGI native provenance: Weapon_UpdateMuzzleFlash_8 - 0x0047CEE0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047CEE0-Weapon_UpdateMuzzleFlash_8.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047CEE0-Weapon_UpdateMuzzleFlash_8.hpp"
int* Weapon_UpdateMuzzleFlash_8(int* context, int arg2)

{
    int source_val = *context;
    int entity_val = 0;

    if (source_val <= 0)
        return nullptr;

    int* result = &context[1];

    do
    {
        if (*result == arg2)
            return result;

        entity_val += 1;
        result = &result[3];
    } while (entity_val < source_val);

    return 0;
}
