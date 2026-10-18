// IGI native provenance: DisplayRender_InvokeSkinBones - 0x0049C800 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 0049C800-DisplayRender_InvokeSkinBones.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "DisplayRender_InvokeSkinBones.hpp"

int DisplayRender_InvokeSkinBones()

{
    int 72350c_2 = g_72350c;
    int dest_ptr = 0;

    if (72350c_2 <= 0)
    {
        g_72350c = 0;
        return 72350c_2;
    }

    int* source_ptr = &g_7ac0a0;
    int 72350c_1;

    do
    {
        int* result_val = *source_ptr;
        (*(*result_val + 8))(result_val);
        72350c_1 = g_72350c;
        *source_ptr = 0;
        dest_ptr += 1;
        source_ptr = &source_ptr[9];
    } while (dest_ptr < 72350c_1);

    g_72350c = 0;
    return 72350c_1;
}
