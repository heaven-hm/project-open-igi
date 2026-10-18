// IGI native provenance: Sys_OpenFileStream_22 - 0x004B7EA0 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 004B7EA0-Sys_OpenFileStream_22.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B7EA0-Sys_OpenFileStream_22.hpp"
int Sys_OpenFileStream_22(int context, int* arg2)

{
    int* data_1 = *arg2;
    int* item_18 = data_1;
    char* item_8 = &g_567c74;
    double item_10 = data_1;
    memcpy(context, &item_18, 0x18);
    return context;
}
