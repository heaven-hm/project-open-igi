// IGI native provenance: Sys_PushStack_16 - 0x004B6440 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B6440-Sys_PushStack_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B6440-Sys_PushStack_16.hpp"
int Sys_PushStack_16(int* context, void* arg2)

{
    void* source_val = arg2;

    if (!Is_ResourceLoaded(source_val, &arg2))
        return Sys_AllocateMemory_5(context, source_val);

    void* data_1 = arg2;
    *context = *(data_1 + 0x23c);
    context[1] = *(data_1 + 0x240);
    return 0;
}
