// IGI native provenance: Camera_ApplyPostProcess_24 - 0x004AC786 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AC786-Camera_ApplyPostProcess_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AC786-Camera_ApplyPostProcess_24.hpp"
int Camera_ApplyPostProcess_24(int context, int arg2)

{
    int f0 = arg2 / 0x20;

    if (*(context + (f0 << 2)) & ~(0xffffffff << ((0x1f - arg2 % 0x20) & 0x1f)))
        return 0;

    int source_1 = f0 + 1;

    if (source_1 >= 3)
        return 1;

    int* data_5 = context + (source_1 << 2);

    while (true)
    {
        if (*data_5)
            return 0;

        source_1 += 1;
        data_5 = &data_5[1];

        if (source_1 >= 3)
            return 1;
    }
}
