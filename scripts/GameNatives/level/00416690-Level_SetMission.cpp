// IGI native provenance: Level_SetMission - 0x00416690 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00416690-Level_SetMission.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Level_SetMission.hpp"
int Level_SetMission()

{
    int result = g_57bab8;
    int source_val = 0;

    if (result > 0)
    {
        int* dest_1 = &g_57b1c0;

        do
        {
            Sys_ReadFileBlock_17(dest_1);
            result = g_57bab8;
            source_val += 1;
            dest_1 = &dest_1[0xc];
        } while (source_val < result);
    }

    return result;
}
