// IGI native provenance: GunShot_TakeSlot - 0x0047B190 - CFG score 66.7% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0047B190-GunShot_TakeSlot.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "GunShot_TakeSlot.hpp"
int GunShot_TakeSlot(void* context, int arg2, int* arg3, void* arg4, int arg5, int arg6)

{
    int dest_val = *(context + 0x94);

    if (dest_val >= 0xf)
    {
        int data_5 = *(context + 0x90) + 1;
        *(context + 0x90) = data_5;
        return data_5;
    }

    int i = 0;
    long double fpu_val_7_1 = *(arg4 + 0x650) * 4096.0f;
    void* entity_val = *(arg4 + 0x658);
    void* data_val = *(arg4 + 0x654);
    int* entity_1 = context + 0xe0;
    int* data_1;

    do
    {
        data_1 = entity_1;

        if (!*entity_1)
            break;

        i += 1;
        entity_1 = &entity_1[0x12];
    } while (i < 0xf);

    *(context + (dest_val << 2) + 0xa4) = data_1;
    *data_1 = 1;
    memcpy(&data_1[2], arg2, 0x18);
    data_1[8] = *arg3;
    data_1[9] = arg3[1];
    int source_2 = arg3[2];
    data_1[0x10] = 0;
    data_1[0xa] = source_2;
    data_1[0xe] = data_val;
    data_1[0xd] = fpu_val_7_1;
    data_1[0xf] = entity_val;
    data_1[0x11] = arg6;
    *(data_1 + 0x41) = 0;
    data_1[0xb] = arg4;
    data_1[0xc] = arg5;
    int data_3 = *(context + 0x90) + 1;
    *(context + 0x94) += 1;
    *(context + 0x90) = data_3;
    return data_3;
}
