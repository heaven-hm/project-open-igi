// IGI native provenance: Script_ParseEnumString_31 - 0x004FE5E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FE5E0-Script_ParseEnumString_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumString_31.h"
int Script_ParseEnumString_31(void* context, int arg2)

{
    int i = 0;

    if (*(context + 0xf0) > 0)
    {
        do
        {
            void* temp_2 = *(*(context + 0x6c) + (i << 2));
            i += 1;
            *(temp_2 + 0x30) = arg2;
        } while (i < *(context + 0xf0));
    }

    return i;
}
