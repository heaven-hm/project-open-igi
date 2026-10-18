// IGI native provenance: Script_FormatString_14 - 0x004F3EF0 - CFG score 88.9% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F3EF0-Script_FormatString_14.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F3EF0-Script_FormatString_14.hpp"
void* const Script_FormatString_14(int context, int arg2)

{
    void* const data_1;

    if (arg2)
        data_1 = *(context + 0x20154);

    if (!arg2 || arg2 < data_1)
    {
        data_1 = nullptr;
        memset(context + 0x6c, 0, 0x88);
        *(context + 0x20154) = 0;

        if (arg2 >= 0)
        {
            int i_1 = arg2 + 1;
            int i;

            do
            {
                data_1 = Script_SpawnNewTask_15(context);
                i = i_1;
                i_1 -= 1;
            } while (i != 1);
        }
    }
    else if (arg2 > data_1)
    {
        int entity_2;

        do
        {
            Script_SpawnNewTask_15(context);
            entity_2 = *(context + 0x20154) + 1;
            *(context + 0x20154) = entity_2;
        } while (arg2 > entity_2);

        return entity_2;
    }

    return data_1;
}
