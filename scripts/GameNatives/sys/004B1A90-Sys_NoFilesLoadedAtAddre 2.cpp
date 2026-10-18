// IGI native provenance: Sys_NoFilesLoadedAtAddre - 0x004B1A90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B1A90-Sys_NoFilesLoadedAtAddre.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_NoFilesLoadedAtAddre.h"
int* Sys_NoFilesLoadedAtAddre(int context)

{
    int* result = g_943970;

    for (int* i = *result; i; i = *i)
    {
        if (result[6] == context)
            return result;

        result = i;
    }

    int item_4 = context;
    Error_Show("No files loaded at address: %p");

    while (true)
        /* nop */
}
