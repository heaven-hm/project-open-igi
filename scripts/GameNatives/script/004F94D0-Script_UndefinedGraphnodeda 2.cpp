// IGI native provenance: Script_UndefinedGraphnodeda - 0x004F94D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F94D0-Script_UndefinedGraphnodeda.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_UndefinedGraphnodeda.h"
int Script_UndefinedGraphnodeda(void* context)

{
    int entity_val = 0;

    for (short* i = &g_a77228; i < &g_a77248; )
    {
        if (*(context + 0x1c) == *i)
            return *((entity_val << 3) + &g_a7722c);

        i = &i[4];
        entity_val += 1;
    }

    Error_Show("Undefined GraphNodeDataSize");

    while (true)
        /* nop */
}
