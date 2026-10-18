// IGI native provenance: Sys_ResolveSymbol_27 - 0x004BC8D0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BC8D0-Sys_ResolveSymbol_27.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Sys_ResolveSymbol_27.h"
int Sys_ResolveSymbol_27(int* context, int arg2, int arg3)

{
    int item_108 = arg3;
    int item_10c = arg2;
    void item_100;
    Game_DataSymbolLoad(&item_100, "%f");
    void* dest_val = &item_100;
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    return Sys_CloseFileStream_4(context, &item_100, ~i - 1);
}
