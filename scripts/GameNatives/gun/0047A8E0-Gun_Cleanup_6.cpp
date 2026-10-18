// IGI native provenance: Gun_Cleanup_6 - 0x0047A8E0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0047A8E0-Gun_Cleanup_6.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Gun_Cleanup_6.hpp"
int Gun_Cleanup_6(void* context, char* arg2)

{
    int result = *(arg2 + 8);

    if (result != context)
    {
        int entity_1 = *(context + 0x14);

        if (result != entity_1)
        {
            result = *(arg2 + 0xc);

            if (result != context && result != entity_1)
            {
                if (!entity_1)
                {
                    *arg2 = 1;
                    return result;
                }

                short data_val = MagicObj_GetTypeId();
                entity_1 = *(*(context + 0x14) + 0x1c);
                result = TaskType_IsDerivedFrom(entity_1, data_val);

                if (!result)
                {
                    *arg2 = 1;
                    return result;
                }

                result = *(arg2 + 8);

                if (result != *(*(context + 0x14) + 0x130))
                {
                    *arg2 = 1;
                    return result;
                }
            }
        }
    }

    *arg2 = 0;
    return result;
}
