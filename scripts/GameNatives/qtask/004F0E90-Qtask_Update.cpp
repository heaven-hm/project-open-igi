// IGI native provenance: Qtask_Update - 0x004F0E90 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F0E90-Qtask_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F0E90-Qtask_Update.hpp"
int* Qtask_Update()

// Sig: void Qtask_Update(void)
// Note: Update QTask hash table call before loading level.
{
    void* data_val = g_a758a4;

    if (data_val)
        QTask_CleanupTypes(data_val);

    int* result;

    for (int* i = &g_a71890; i < &g_a75890; i = &i[1])
    {
        result = *i;

        if (result)
        {
            result = Qtask_UpdateList(result);
            *i = 0;
        }
    }

    return result;
}
