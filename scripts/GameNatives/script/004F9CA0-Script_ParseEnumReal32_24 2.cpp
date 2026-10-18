// IGI native provenance: Script_ParseEnumReal32_24 - 0x004F9CA0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9CA0-Script_ParseEnumReal32_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumReal32_24.h"
int Script_ParseEnumReal32_24(void* context, int arg2)

{
    int i = 0;
    int* entity_val = context + 0x13c;

    do
    {
        if (*entity_val == arg2)
        {
            *(context + (i << 2) + 0x13c) = 0;
            return i;
        }

        i += 1;
        entity_val = &entity_val[1];
    } while (i < 0x40);

    return i;
}
