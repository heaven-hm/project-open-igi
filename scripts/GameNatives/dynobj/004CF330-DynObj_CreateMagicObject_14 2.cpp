// Retail runtime observation: DynObj_LoadPackageMeshes - 0x004CF330 - 4 logged entries; full semantics NOT verified.
// Runtime/static evidence: assets/BinaryNinja/RUNTIME_LEVEL1_LOAD_2026-10-17.json.
// Scope: Model-package catalogue traversal; do not count this as level placement.
// IGI native provenance: DynObj_LoadPackageMeshes - 0x004CF330 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004CF330-DynObj_LoadPackageMeshes.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DynObj_LoadPackageMeshes.h"
int* DynObj_LoadPackageMeshes(void* context)

{
    int* result = Ilff_FindFormChunk(*(context + 8), 0x4d4f4453, nullptr);
    int i_1 = *result;
    void* source_1 = &result[1];

    if (i_1 > 0)
    {
        int i;

        do
        {
            HeapList_Insert(context + 0x10, DynObj_LoadModelMesh(source_1));
            void* dest_1 = source_1;
            int j = 0xffffffff;
            result = nullptr;

            while (j)
            {
                bool flag_0_1 = 0 != *dest_1;
                dest_1 += 1;
                j -= 1;

                if (!flag_0_1)
                    break;
            }

            i = i_1;
            i_1 -= 1;
            source_1 = source_1 + ~j - 1 + 1;
        } while (i != 1);
    }

    return result;
}
