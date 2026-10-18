// IGI native provenance: AI_TakeCover - 0x00450D50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00450D50-AI_TakeCover.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_TakeCover.h"
int AI_TakeCover(void* context, int arg2)

{
    unsigned int data_val = AI_CheckHearing();
    int temp_val = 0;
    temp_val = *(context + 0x1c);
    return (*(((data_val * 0x180 + temp_val) << 2) + &g_a96ae0))(context, arg2);
}
