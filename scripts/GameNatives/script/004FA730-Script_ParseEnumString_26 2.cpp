// IGI native provenance: Script_ParseEnumString_26 - 0x004FA730 - CFG score 83.3% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004FA730-Script_ParseEnumString_26.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumString_26.h"
int Script_ParseEnumString_26(void* context, int arg2)

{
    int dest_val = arg2;
    int i = 0xffffffff;
    char item_104 = 9;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    char item_103 = ~i;
    void item_102;
    int source_1;
    int dest_1;
    dest_1 = memcpy(&item_102, arg2, ~i & 0xfffffffc);
    memcpy(dest_1, source_1, ~i & 3);
    Sys_ResolveSymbol_4(*(context + 4));
    int result = Sys_CloseFileStream_4(*(context + 4), &item_104, ~i + 2);
    result = 1;
    return result;
}
