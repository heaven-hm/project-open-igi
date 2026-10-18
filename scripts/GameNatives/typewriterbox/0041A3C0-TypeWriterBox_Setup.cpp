// IGI native provenance: TypeWriterBox_Setup - 0x0041A3C0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0041A3C0-TypeWriterBox_Setup.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0041A3C0-TypeWriterBox_Setup.hpp"
int TypeWriterBox_Setup(void* context)

{
    int* source_val = context + 0x78;
    int i_1 = 4;
    int i;

    do
    {
        int* data_1 = *source_val;

        if (data_1)
            Sys_ReadFileBlock_18(data_1);

        source_val = &source_val[1];
        i = i_1;
        i_1 -= 1;
    } while (i != 1);
    int data_2 = Script_SpawnNewTask_6();
    int entity_val = 0;
    entity_val = *(context + 0x1c);
    return (*(((data_2 * 0x180 + entity_val) << 2) + &g_a96ae0))(context);
}
