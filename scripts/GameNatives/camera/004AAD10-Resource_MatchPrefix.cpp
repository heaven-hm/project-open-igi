// IGI native provenance: Resource_MatchPrefix - 0x004AAD10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004AAD10-Resource_MatchPrefix.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004AAD10-Resource_MatchPrefix.hpp"
int Resource_MatchPrefix(void* context, void* arg2, int arg3)

{
    int i = arg3;

    if (i)
    {
        int i_2 = i;
        void* dest_1 = context;
        void* source_1 = dest_1;

        while (i)
        {
            bool flag_0_1 = 0 != *dest_1;
            dest_1 += 1;
            i -= 1;

            if (!flag_0_1)
                break;
        }

        int i_1 = -(i) + i_2;
        void* dest_2 = source_1;
        void* source_2 = arg2;

        while (i_1)
        {
            bool flag_1_1 = *source_2 == *dest_2;
            source_2 += 1;
            dest_2 += 1;
            i_1 -= 1;

            if (!flag_1_1)
                break;
        }

        char data_1 = *(source_2 - 1);
        i = 0;
        char f0_1 = *(dest_2 - 1);

        if (data_1 > f0_1)
            return ~i;

        if (data_1 != f0_1)
            return ~0xfffffffe;
    }

    return i;
}
