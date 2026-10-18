// IGI native provenance: Script_ParseEnumString_23 - 0x004F96E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F96E0-Script_ParseEnumString_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F96E0-Script_ParseEnumString_23.hpp"
int Script_ParseEnumString_23(void* context, int arg2)

{
    int result = *(context + 0x6c);

    if (result)
    {
        result = Script_UndefinedGraphnodeda(context);
        int i_1 = arg2;

        if (i_1 > 0)
        {
            int* entity_1 = nullptr;
            int i;

            do
            {
                *(entity_1 + *(context + 0x6c)) = 0xffffffff;
                entity_1 += result;
                i = i_1;
                i_1 -= 1;
            } while (i != 1);
        }
    }

    return result;
}
