// IGI native provenance: Camera_ResetMatrix - 0x004A2600 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A2600-Camera_ResetMatrix.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A2600-Camera_ResetMatrix.hpp"
int Camera_ResetMatrix(void* context)

{
    int i_1 = 4;
    int source_1 =
        *(context + 0x6c) * 128.0f - ((((*(context + 0x64) * -128.0f) << 8) + *(context + 0x68) * -128.0f) << 8);
    int temp_2 = 0x935c10;
    int result =
        *(context + 0x6c) * 255.0f - ((((*(context + 0x64) * -255.0f) << 8) + *(context + 0x68) * -255.0f) << 8);
    int i;

    do
    {
        int temp_3 = temp_2 + 0x80;
        int entity_8;
        int dest_2;
        dest_2 = __memfill_u32(temp_2, source_1, 0x20);
        temp_2 = temp_3 + 0x80;
        i = i_1;
        i_1 -= 1;
        int entity_9;
        int dest_4;
        dest_4 = __memfill_u32(temp_3, result, 0x20);
    } while (i != 1);
    return result;
}
