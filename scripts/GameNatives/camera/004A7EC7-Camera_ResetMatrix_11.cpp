// IGI native provenance: Camera_ResetMatrix_11 - 0x004A7EC7 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A7EC7-Camera_ResetMatrix_11.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A7EC7-Camera_ResetMatrix_11.hpp"
int* Camera_ResetMatrix_11(char* context, int* arg2, int* arg3, int* arg4)

{
    int* i = arg2;
    int* i_1 = arg2 - 1;

    if (i > 0)
    {
        char* source_1 = context;

        do
        {
            int data_val = *source_1;
            source_1 = &source_1[1];
            i = Camera_ApplyPostProcess_11(data_val, arg3, arg4);

            if (*arg4 == 0xffffffff)
                break;

            i = i_1;
            i_1 -= 1;
        } while (i > 0);
    }

    return i;
}
