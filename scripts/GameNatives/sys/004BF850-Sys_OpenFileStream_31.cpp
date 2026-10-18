// IGI native provenance: Sys_OpenFileStream_31 - 0x004BF850 - CFG score 95.2% (PASS; structural only); runtime behavior not verified.
// Evidence: 004BF850-Sys_OpenFileStream_31.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004BF850-Sys_OpenFileStream_31.hpp"
int Sys_OpenFileStream_31(int* context, int** arg2)

{
    Sys_While(context);
    int* data_val = Mem_Alloc(0x14, 4);
    memset(data_val, 0, 0x14);
    data_val[1] = 0;
    *data_val = 0;
    data_val[2] = 4;
    *arg2 = data_val;

    if (Sys_CloseFileStream_31(context, &context[2]) && *context[7] == 0x28)
    {
        int item_10_1 = 0;

        if (Sys_PopStack_24(context, &data_val[3]) && Sys_CloseFileStream_31(context, &context[2]) && Sys_UnexpectedEndOfFile(context, &data_val[4]))
            return 1;
    }

    return 0;
}
