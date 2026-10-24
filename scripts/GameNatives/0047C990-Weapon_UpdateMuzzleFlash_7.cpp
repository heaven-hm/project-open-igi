// IGI native provenance: Weapon_UpdateMuzzleFlash_7 - 0x0047C990 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0047C990-Weapon_UpdateMuzzleFlash_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0047C990-Weapon_UpdateMuzzleFlash_7.hpp"
void* Weapon_UpdateMuzzleFlash_7(void* context, int arg2)

{
    void* entity_1 = *(context + (arg2 << 2) + 0x900);
    void* temp_val = *(context + 0x914);
    int context_val = *(entity_1 + 0xbc);
    *(entity_1 + 0xc0) = 0;
    int dest_1 = *(temp_val + 0x9c) - 1;
    *(temp_val + 0x9c) = dest_1;
    memcpy(context_val * 0x50 + context + 0x918, dest_1 * 0x50 + context + 0x918, 0x50);
    int source_1 = *(context + 0x9c);
    int entity_7 = 0;

    if (source_1 > 0)
    {
        int* flagP1 = context + 0x900;

        while (true)
        {
            void* dest_3 = *flagP1;

            if (*(dest_3 + 0xc0) && *(dest_3 + 0xbc) == *(temp_val + 0x9c))
            {
                *(*(context + (entity_7 << 2) + 0x900) + 0xbc) = context_val;
                break;
            }

            entity_7 += 1;
            flagP1 = &flagP1[1];

            if (entity_7 >= source_1)
                return context;
        }
    }

    return context;
}
