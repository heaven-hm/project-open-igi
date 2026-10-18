// IGI native provenance: Script_TooFewParametersInLi - 0x004F1C20 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F1C20-Script_TooFewParametersInLi.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004F1C20-Script_TooFewParametersInLi.hpp"
void Script_TooFewParametersInLi(int* context, int arg2)

{
    int source_val = 0;
    int context_val = *context;

    if (arg2 <= 0)
        return;

    while (true)
    {
        if (!Sys_AllocateMemory_24(context[1], source_val + context_val))
        {
            int item_14_2 = context[2];
            int item_18_2 = context[3];
            Error_Show("Too few parameters in line %d of file '%s'");

            while (true)
                /* nop */
        }
        else
        {
            source_val += 1;

            if (source_val >= arg2)
                break;
        }
    }
}
