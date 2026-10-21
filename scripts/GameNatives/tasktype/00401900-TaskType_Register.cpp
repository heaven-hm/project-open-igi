// IGI native provenance: TaskType_Register - 0x00401900 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00401900-TaskType_Register.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "TaskType_Register.h"
int TaskType_Register(int context, int arg2, short arg3, int arg4)

// Sig: unsigned short TaskType_Register(unsigned int p1, unsigned int p2, unsigned short
// parentType, unsigned int p4)
// Note: Registers and allocates a task type, copies the task handler table, and
// returns the allocated 16-bit type id. Ghidra/r2 CFG agreement: 18 basic
// blocks and 23 edges; see assets/IGI1-Native-Name-Evidence.json.
{
    int flagP = 0;

    for (char* i = &g_af5f05; i < 0xafa705; )
    {
        if (!*i)
        {
            int source_1 = flagP * 0x30;
            *(source_1 + &g_af5f05) = 1;
            *(source_1 + 0xaf5ee4) = arg2;
            *(source_1 + &g_af5ee0) = context;
            *(source_1 + &g_af5ee8) = arg3;

            if (arg3 == 0x181)
                *(source_1 + 0xaf5f0c) = 0x20000000;
            else
                *(source_1 + 0xaf5f0c) = *(arg3 * 0x30 + 0xaf5f0c);

            if (!arg4)
                *(source_1 + 0xaf5f04) = 0;
            else
            {
                Sys_FreeMemory_2(source_1 + 0xaf5eec, arg2, arg4, 0);
                *(source_1 + 0xaf5f04) = 1;
            }

            if (arg3 == 0x181)
            {
                int* data_7 = (flagP << 2) + &g_a96ae0;
                int j_2 = 0xfe;
                int j;

                do
                {
                    *data_7 = 0;
                    data_7 = &data_7[0x180];
                    j = j_2;
                    j_2 -= 1;
                } while (j != 1);
                int data_8 = QTask_DeleteTask();
                data_8 = flagP;
                return data_8;
            }

            int* entity_2 = (flagP << 2) + &g_a96ae0;
            int j_3 = 0xfe;
            int* data_5 = (arg3 << 2) + &g_a96ae0;
            int j_1;

            do
            {
                int source_2 = *data_5;
                data_5 = &data_5[0x180];
                *entity_2 = source_2;
                entity_2 = &entity_2[0x180];
                j_1 = j_3;
                j_3 -= 1;
            } while (j_1 != 1);
            int data_6 = QTask_DeleteTask();
            data_6 = flagP;
            return data_6;
        }

        i = &i[0x30];
        flagP += 1;
    }

    Error_Show("Unable to allocate task type");

    while (true)
        /* nop */
}
