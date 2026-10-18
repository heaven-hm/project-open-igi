// IGI native provenance: Script_ParseEnumReal32_23 - 0x004F9620 - CFG score 90.5% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F9620-Script_ParseEnumReal32_23.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_ParseEnumReal32_23.h"
int Script_ParseEnumReal32_23(void* context, int arg2)

{
    int flagP = *(context + 0x58);
    void* source_val = *(context + 0x6c);
    int flagP1 = flagP + ((arg2 - flagP) / 0x64 + 1) * 0x64;
    int data_4 = Script_UndefinedGraphnodeda(context);
    *(context + 0x6c) = Mem_Alloc(data_4 * flagP1, 4);
    Script_ParseEnumString_23(context, flagP1);

    if (source_val)
    {
        int entity_3 = *(context + 0x58) * data_4;
        int source_1;
        int dest_2;
        dest_2 = memcpy(*(context + 0x6c), source_val, entity_3 & 0xfffffffc);
        memcpy(dest_2, source_1, entity_3 & 3);
        Sys_ExecuteBytecode_2(source_val);
    }

    *(context + 0x58) = flagP1;
    unsigned int data_8 = Script_EvaluateCondition_22();
    int data_9 = 0;
    data_9 = *(context + 0x1c);
    int result = *(((data_8 * 0x180 + data_9) << 2) + &g_a96ae0);

    if (!result)
        return result;

    return result(context, 0);
}
