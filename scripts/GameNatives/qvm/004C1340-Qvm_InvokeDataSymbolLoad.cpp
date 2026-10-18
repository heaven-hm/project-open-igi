// IGI native provenance: Qvm_InvokeDataSymbolLoad - 0x004C1340 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004C1340-Qvm_InvokeDataSymbolLoad.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004C1340-Qvm_InvokeDataSymbolLoad.hpp"
int Qvm_InvokeDataSymbolLoad(int* arg1, int* arg2)

{
    int var_2c = *(arg1[2] + *arg2 + 1);
    void var_20;
    Game_DataSymbolLoad(&var_20, "%d\r\n");
    void* dest_ptr = &var_20;
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *dest_ptr;
        dest_ptr += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    Sys_CloseFileStream_4(*arg1, &var_20, ~i - 1);
    int result = arg1[2] + 2;
    arg1[2] = result;
    return result;
}
