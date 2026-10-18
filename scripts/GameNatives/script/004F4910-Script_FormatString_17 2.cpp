// IGI native provenance: Script_FormatString_17 - 0x004F4910 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004F4910-Script_FormatString_17.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Script_FormatString_17.h"
int Script_FormatString_17(int context, int arg2, int arg3) {
    int data_val = arg3;

    if (data_val < 0)
        data_val = 0;

    if (data_val < arg2)
        return context + (data_val << 3);

    return context + (arg2 << 3) - 8;
}
