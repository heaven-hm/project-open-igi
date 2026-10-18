// IGI native provenance: SoundDefGroup_Update - 0x004EB620 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004EB620-SoundDefGroup_Update.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundDefGroup_Update.hpp"
int SoundDefGroup_Update(void* context, void** arg2)

{
    void* source_val = *(context + 0x14);
    unsigned int data_1 = SoundSys_SetFrequency_5();
    int entity_val = 0;
    entity_val = *(source_val + 0x1c);
    int result = *(((data_1 * 0x180 + entity_val) << 2) + &g_a96ae0);

    if (result)
        result = result(source_val, arg2);

    if (!*arg2)
        *arg2 = context + 0x48;

    return result;
}
