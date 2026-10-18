// IGI native provenance: Script_GraphnodeLinklimitRe - 0x004F9C10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9C10-Script_GraphnodeLinklimitRe.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F9C10-Script_GraphnodeLinklimitRe.hpp"
int Script_GraphnodeLinklimitRe(void* context, int arg2)

{
    int i = 0;
    int* entity_val = context + 0x13c;

    do
    {
        if (!*entity_val)
        {
            *(context + (i << 2) + 0x13c) = arg2;
            return i;
        }

        i += 1;
        entity_val = &entity_val[1];
    } while (i < 0x40);

    return Warning_Show("GRAPHNODE - LINKLIMIT REACHED");
}
