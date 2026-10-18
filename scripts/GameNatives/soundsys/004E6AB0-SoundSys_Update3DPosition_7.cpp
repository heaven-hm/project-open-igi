// IGI native provenance: SoundSys_Update3DPosition_7 - 0x004E6AB0 - CFG score 91.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E6AB0-SoundSys_Update3DPosition_7.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "SoundSys_Update3DPosition_7.hpp"
int* SoundSys_Update3DPosition_7(int context)

{
    Qhash_Init(1);
    int* result = Mem_Alloc((context << 2) + 4, 4);
    Qhash_Reset();

    if (result)
    {
        *result = context;

        if (context > 0)
            memset(&result[1], 0, context << 2);
    }

    return result;
}
