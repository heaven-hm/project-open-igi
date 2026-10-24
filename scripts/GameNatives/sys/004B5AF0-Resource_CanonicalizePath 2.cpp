// IGI native provenance: Resource_CanonicalizePath - 0x004B5AF0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B5AF0-Resource_CanonicalizePath.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004B5AF0-Resource_CanonicalizePath.hpp"
int Resource_CanonicalizePath(char* context, char* arg2)

{
    char* dest_val = arg2;
    char i = *dest_val;

    if (!i)
    {
        *context = 0;
        return i;
    }

    char* source_val = context;

    do
    {
        int i_1 = i;
        int entity_val;
        i = Camera_ApplyWideScreen_28(entity_val);
        *source_val = i;
        i = dest_val[1];
        source_val = &source_val[1];
        dest_val = &dest_val[1];
    } while (i);

    *source_val = i;
    return i;
}
