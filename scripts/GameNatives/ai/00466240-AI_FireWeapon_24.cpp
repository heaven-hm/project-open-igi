// IGI native provenance: AI_FireWeapon_24 - 0x00466240 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00466240-AI_FireWeapon_24.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "AI_FireWeapon_24.hpp"
struct status_msg* AI_FireWeapon_24(unsigned int context, unsigned int arg2, int arg3, void** arg4, int* arg5, int* arg6, unsigned int arg7)

{
    int kind;
    kind = g_53f520;
    int* context_val;
    int* item_14 = context_val;
    struct status_msg* result = QTask_HandleInput(context, kind, 0);
    result->head[0x16] = arg7;
    result->head[8] = 0;
    result->head[9] = arg2;
    result->head[0x10] = 0;
    result->head[0x1c] = 0;
    result->head[0xd] = *arg5;
    result->head[0xe] = arg5[1];
    result->head[0xf] = arg5[2];
    result->head[0xa] = *arg6;
    result->head[0xb] = arg6[1];
    unsigned int temp_4 = arg6[2];
    result->head[0xc] = temp_4;

    if (arg2 > 0)
    {
        void** dest_4 = arg4;
        int** source_1 = &result->head[0x17];
        context_val = arg3 - dest_4;
        context = arg2;
        bool flag_1_1;

        do
        {
            int data_1 = *(context_val + dest_4);
            source_1[-6] = data_1;
            void* entity_3 = *dest_4;
            void* item_28_1 = entity_3;
            int* data_2;
            data_2 = Resource_Load(data_1, temp_4, entity_3, nullptr);
            *source_1 = data_2;
            source_1[6] = 0;
            dest_4 = &dest_4[1];
            source_1 = &source_1[1];
            flag_1_1 = context != 1;
            context -= 1;
        } while (flag_1_1);
    }

    int item_10 = 0x10;
    int item_c = 0x10000;
    int item_8 = 0x44;
    int item_4 = 0x32;
    result->head[0x22] = 0;
    int* entity_4 = g_5ca110;
    (*(*entity_4 + 0x14))(entity_4, &item_10, &result->head[0x22], 0);
    char data_6 = QHash_ValueGet();

    if (!data_6)
        QHash_ValueSet();

    result->head[0x23] = 0;
    result->head[0x23] = Sys_DispatchOpcode_18("LOCAL:computer/font3.fnt");

    if (!data_6)
        Sys_DispatchOpcode_2();

    return result;
}
