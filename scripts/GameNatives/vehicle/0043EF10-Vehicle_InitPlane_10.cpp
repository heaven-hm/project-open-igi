// IGI native provenance: Vehicle_InitPlane_10 - 0x0043EF10 - CFG score 86.7% (PASS; structural only); runtime behavior not verified.
// Evidence: 0043EF10-Vehicle_InitPlane_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "0043EF10-Vehicle_InitPlane_10.hpp"
struct status_msg* Vehicle_InitPlane_10(void* context, int arg2, int arg3, int arg4, int* arg5)

{
    int kind = QHash_ValueGet();
    struct status_msg* result;

    if (kind)
    {
        int kind_1;
        kind_1 = g_53c4a8;
        result = QTask_HandleInput(context, kind_1, 0);
    }
    else
    {
        QHash_ValueSet();
        kind = g_53c4a8;
        result = QTask_HandleInput(context, kind, 0);
        Sys_DispatchOpcode_2();
    }

    memcpy(&result->sprite[0xb4], arg2, 0x18);
    memcpy(&result->sprite[0xd8], arg3, 0x28);
    int data_3 = *arg5;
    int dest_2 = arg4;
    result->sprite[0xcc] = data_3;
    result->sprite[0xcd] = *(&data_3 + 1);
    result->sprite[0xce] = *(&data_3 + 2);
    result->sprite[0xcf] = *(&data_3 + 3);
    int data_4 = arg5[1];
    result->sprite[0xd0] = data_4;
    result->sprite[0xd1] = *(&data_4 + 1);
    result->sprite[0xd2] = *(&data_4 + 2);
    result->sprite[0xd3] = *(&data_4 + 3);
    int temp_1 = arg5[2];
    result->sprite[0xd4] = temp_1;
    result->sprite[0xd5] = *(&temp_1 + 1);
    result->sprite[0xd6] = *(&temp_1 + 2);
    result->sprite[0xd7] = *(&temp_1 + 3);
    int i = 0xffffffff;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_2;
        dest_2 += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_3 = ~i;
    int source_3;
    int dest_5;
    dest_5 = memcpy(&result->sprite[0xa0], dest_2 - entity_3, entity_3 & 0xfffffffc);
    memcpy(dest_5, source_3, entity_3 & 3);
    unsigned int data_7 = Script_ParseEnumReal32_8();
    int temp_3 = 0;
    temp_3 = result->head[7];
    int data_8 = *(((data_7 * 0x180 + temp_3) << 2) + &g_a96ae0);

    if (data_8)
        data_8(result, 0);

    return result;
}
