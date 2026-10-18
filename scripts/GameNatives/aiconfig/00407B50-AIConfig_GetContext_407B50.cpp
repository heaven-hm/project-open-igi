// IGI native provenance: AIConfig_GetContext_407B50 - 0x00407B50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00407B50-AIConfig_GetContext_407B50.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00407B50-AIConfig_GetContext_407B50.hpp"
int AIConfig_GetContext_407B50(void* arg1)

{
    int dest_ptr = 0x5690f0;
    *(arg1 + 0x20) = 0;
    int i = 0xffffffff;

    while (i)
    {
        bool cond:0_1 = 0 != *dest_ptr;
        dest_ptr += 1;
        i -= 1;

        if (!cond:0_1)
            break;
    }

    int result = ~i;
    int esi_1;
    int edi_3;
    edi_3 = __builtin_memcpy(arg1 + 0x21, dest_ptr - result, result & 0xfffffffc);
    __builtin_memcpy(edi_3, esi_1, result & 3);
    return result;
}
