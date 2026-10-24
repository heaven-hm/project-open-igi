// IGI native provenance: Camera_ApplyPostProcess_28 - 0x004AD809 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AD809-Camera_ApplyPostProcess_28.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AD809-Camera_ApplyPostProcess_28.hpp"
char* Camera_ApplyPostProcess_28(unsigned int context, int arg2)

{
    unsigned int dwBytes = context * arg2;
    context = dwBytes;

    if (dwBytes <= 0xffffffe0)
    {
        if (!dwBytes)
            dwBytes = 1;

        dwBytes = (dwBytes + 0xf) & 0xfffffff0;
    }

    bool flag_1_1;

    do
    {
        if (dwBytes <= 0xffffffe0)
        {
            int data_1 = g_c3242c;
            char* result;

            if (data_1 != 3)
            {
                if (data_1 == 2 && dwBytes <= g_5466d4)
                {
                    result = Mem_AllocateCustomPool(dwBytes >> 4);

                    if (result)
                    {
                        Camera_ApplyBandFilter_26(result, 0, dwBytes);
                        return result;
                    }
                }
            }
            else if (context <= g_c321fc)
            {
                result = Mem_GrowCustomPool_1(context);

                if (result)
                {
                    Camera_ApplyBandFilter_26(result, 0, context);
                    return result;
                }
            }

            result = HeapAlloc(g_c32428, HEAP_ZERO_MEMORY, dwBytes);

            if (result || !g_936250)
                return result;
        }
        else if (!g_936250)
            return nullptr;

        flag_1_1 = !Mem_InvokeNewHandler(dwBytes);
    } while (!flag_1_1);
    return nullptr;
}
