// IGI native provenance: Script_ParseEnumString_27 - 0x004FA950 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004FA950-Script_ParseEnumString_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumString_27.h"
unsigned int Script_ParseEnumString_27(void* context, int arg2)

{
    Sys_ResolveSymbol_4(*(context + 4));
    void item_104;
    Resource_Unpack(*(context + 4), &item_104, 2);
    int item_103;
    unsigned int result_1 = item_103;
    Sys_ResolveSymbol_4(*(context + 4));
    Resource_Unpack(*(context + 4), (&item_103 + 1), result_1);
    unsigned int result = result_1;
    int source_1;
    int dest_2;
    dest_2 = memcpy(arg2, (&item_103 + 1), result_1 & 0xfffffffc);
    int count = result & 3;
    result = 1;
    memcpy(dest_2, source_1, count);
    return result;
}
