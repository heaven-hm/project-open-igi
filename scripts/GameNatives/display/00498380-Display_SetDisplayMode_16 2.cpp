// IGI native provenance: Display_SetDisplayMode_16 - 0x00498380 - CFG score 77.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 00498380-Display_SetDisplayMode_16.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Display_SetDisplayMode_16.h"
int Display_SetDisplayMode_16(int* context, int arg2, int arg3)

{
    int source_2 = (arg2 * arg3) << 2;
    void* data_val = Mem_Alloc(source_2, 4);
    context[2] = data_val;
    context[1] = arg3;
    *context = arg2;
    memset(memset(data_val, 0, source_2 & 0xfffffffc), 0, source_2 & 3);
    return 0;
}
