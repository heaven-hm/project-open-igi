// IGI native provenance: Env_TriggerAlarm_13 - 0x0044FE50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0044FE50-Env_TriggerAlarm_13.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0044FE50-Env_TriggerAlarm_13.hpp"
int Env_TriggerAlarm_13(void* context, void** arg2)

{
    HeapList_Remove(arg2);
    int* data_val = arg2[0xb];

    if (data_val)
    {
        Qtask_UpdateList(data_val);
        arg2[0xb] = 0;
    }

    int* data_1 = arg2[0xa];

    if (data_1)
    {
        Qtask_UpdateList(data_1);
        arg2[0xa] = 0;
    }

    arg2[1] = 0;
    *arg2 = nullptr;
    HeapList_Insert(context + 0x810, arg2);
    int result = *(context + 0x8e0) - 1;
    *(context + 0x8e0) = result;
    return result;
}
