// IGI native provenance: GlowTask_Init_2 - 0x004E93B0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E93B0-GlowTask_Init_2.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GlowTask_Init_2.h"

int GlowTask_Init_2()

{
    void* a5ec74_1 = g_a5ec74;
    int counter_val = *(a5ec74_1 + 0x28);
    int source_ptr = counter_val;

    if (counter_val >= 0xff)
        source_ptr = 0xff;

    int counter_val_1 = *(a5ec74_1 + 0x24);
    int scratch_val = counter_val_1;

    if (counter_val_1 >= 0xff)
        scratch_val = 0xff;

    int counter_val_2 = *(a5ec74_1 + 0x20);

    if (counter_val_2 >= 0xff)
        counter_val_2 = 0xff;

    int result = SoundSys_EvaluateTrigger_13(a5ec74_1, counter_val_2, scratch_val, source_ptr, *(a5ec74_1 + 0x2c));
    g_a5ec74 = 0;
    return result;
}
