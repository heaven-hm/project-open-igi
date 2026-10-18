// IGI native provenance: Physics_CheckHullCollision_10 - 0x0050A050 - CFG score 73.8% (FAIL; structural only); runtime behavior not verified.
// Evidence: 0050A050-Physics_CheckHullCollision_10.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "Physics_CheckHullCollision_10.h"
HANDLE Physics_CheckHullCollision_10(int context, int arg2, int arg3, int arg4)

{
    int i = 0xffffffff;
    int dest_val = arg2;

    while (i)
    {
        bool flag_0_1 = 0 != *dest_val;
        dest_val += 1;
        i -= 1;

        if (!flag_0_1)
            break;
    }

    int entity_val = ~i;
    CHAR fileName[0x104];
    int source_1;
    int dest_2;
    dest_2 = memcpy(&fileName, dest_val - entity_val, entity_val & 0xfffffffc);
    memcpy(dest_2, source_1, entity_val & 3);
    int dest_3 = arg2;
    int i_1 = 0xffffffff;

    while (i_1)
    {
        bool flag_1_1 = 0 != *dest_3;
        dest_3 += 1;
        i_1 -= 1;

        if (!flag_1_1)
            break;
    }

    char* context_1;

    if (*(~i_1 - 1 + arg2 - 1) == 0x2f)
        context_1 = &g_567c74;
    else
    {
        void* dest_4 = &g_54e9b8;
        int i_2 = 0xffffffff;

        while (i_2)
        {
            bool flag_2_1 = 0 != *dest_4;
            dest_4 += 1;
            i_2 -= 1;

            if (!flag_2_1)
                break;
        }

        int entity_6 = ~i_2;
        CHAR (* dest_6)[0x104] = &fileName;
        int i_3 = 0xffffffff;

        while (i_3)
        {
            bool flag_3_1 = 0 != *dest_6;
            dest_6 = &(*dest_6)[1];
            i_3 -= 1;

            if (!flag_3_1)
                break;
        }

        int source_3;
        int dest_8;
        dest_8 = memcpy(dest_6 - 1, dest_4 - entity_6, entity_6 & 0xfffffffc);
        context_1 = &g_54e9b8;
        memcpy(dest_8, source_3, entity_6 & 3);
    }

    void* dest_9 = &g_54e9b4;
    int i_4 = 0xffffffff;

    while (i_4)
    {
        bool flag_11_1 = 0 != *dest_9;
        dest_9 += 1;
        i_4 -= 1;

        if (!flag_11_1)
            break;
    }

    int entity_11 = ~i_4;
    CHAR (* dest_11)[0x104] = &fileName;
    int i_5 = 0xffffffff;

    while (i_5)
    {
        bool flag_12_1 = 0 != *dest_11;
        dest_11 = &(*dest_11)[1];
        i_5 -= 1;

        if (!flag_12_1)
            break;
    }

    int source_5;
    int dest_13;
    dest_13 = memcpy(dest_11 - 1, dest_9 - entity_11, entity_11 & 0xfffffffc);
    memcpy(dest_13, source_5, entity_11 & 3);
    WIN32_FIND_DATAA findFileData;
    HANDLE hFindFile = FindFirstFileA(&fileName, &findFileData);
    void item_348;
    void* item_354 = &item_348;

    if (hFindFile == 0xffffffff)
        return hFindFile;

    void* dest_14 = Sys_CheckHeap_6(context);
    int i_6 = 0xffffffff;

    while (i_6)
    {
        bool flag_4_1 = 0 != *dest_14;
        dest_14 += 1;
        i_6 -= 1;

        if (!flag_4_1)
            break;
    }

    int entity_16 = ~i_6;
    int source_7;
    int dest_16;
    dest_16 = memcpy(&item_348, dest_14 - entity_16, entity_16 & 0xfffffffc);
    memcpy(dest_16, source_7, entity_16 & 3);
    int i_7 = 0xffffffff;
    int dest_17 = arg2;

    while (i_7)
    {
        bool flag_5_1 = 0 != *dest_17;
        dest_17 += 1;
        i_7 -= 1;

        if (!flag_5_1)
            break;
    }

    int entity_20 = ~i_7;
    void* dest_19 = &item_348;
    int i_8 = 0xffffffff;

    while (i_8)
    {
        bool flag_6_1 = 0 != *dest_19;
        dest_19 += 1;
        i_8 -= 1;

        if (!flag_6_1)
            break;
    }

    int source_9;
    int dest_21;
    dest_21 = memcpy(dest_19 - 1, dest_17 - entity_20, entity_20 & 0xfffffffc);
    memcpy(dest_21, source_9, entity_20 & 3);
    char* dest_22 = context_1;
    int i_9 = 0xffffffff;

    while (i_9)
    {
        bool flag_7_1 = 0 != *dest_22;
        dest_22 = &dest_22[1];
        i_9 -= 1;

        if (!flag_7_1)
            break;
    }

    int entity_25 = ~i_9;
    void* dest_24 = &item_348;
    int i_10 = 0xffffffff;

    while (i_10)
    {
        bool flag_8_1 = 0 != *dest_24;
        dest_24 += 1;
        i_10 -= 1;

        if (!flag_8_1)
            break;
    }

    int source_11;
    int dest_26;
    dest_26 = memcpy(dest_24 - 1, dest_22 - entity_25, entity_25 & 0xfffffffc);
    memcpy(dest_26, source_11, entity_25 & 3);
    item_218;
    void* dest_27 = &item_218;
    int i_11 = 0xffffffff;

    while (i_11)
    {
        bool flag_9_1 = 0 != *dest_27;
        dest_27 += 1;
        i_11 -= 1;

        if (!flag_9_1)
            break;
    }

    int entity_30 = ~i_11;
    void* dest_29 = &item_348;
    int i_12 = 0xffffffff;

    while (i_12)
    {
        bool flag_10_1 = 0 != *dest_29;
        dest_29 += 1;
        i_12 -= 1;

        if (!flag_10_1)
            break;
    }

    int source_13;
    int dest_31;
    dest_31 = memcpy(dest_29 - 1, dest_27 - entity_30, entity_30 & 0xfffffffc);
    DWORD nFileSizeLow = findFileData.nFileSizeLow;
    memcpy(dest_31, source_13, entity_30 & 3);
    unsigned char item_34c_1 = findFileData.dwFileAttributes >> 4 & 1;

    if (findFileData.cFileName[0] != 0x2e)
        arg3(&item_354, arg4);

    if (FindNextFileA(hFindFile, &findFileData))
    {
        BOOL i_13;

        do
        {
            void* dest_32 = Sys_CheckHeap_6(context);
            int j = 0xffffffff;

            while (j)
            {
                bool flag_14_1 = 0 != *dest_32;
                dest_32 += 1;
                j -= 1;

                if (!flag_14_1)
                    break;
            }

            int entity_38 = ~j;
            int source_15;
            int dest_34;
            dest_34 = memcpy(&item_348, dest_32 - entity_38, entity_38 & 0xfffffffc);
            memcpy(dest_34, source_15, entity_38 & 3);
            int j_1 = 0xffffffff;
            int dest_35 = arg2;

            while (j_1)
            {
                bool flag_15_1 = 0 != *dest_35;
                dest_35 += 1;
                j_1 -= 1;

                if (!flag_15_1)
                    break;
            }

            int entity_42 = ~j_1;
            void* dest_37 = &item_348;
            int j_2 = 0xffffffff;

            while (j_2)
            {
                bool flag_16_1 = 0 != *dest_37;
                dest_37 += 1;
                j_2 -= 1;

                if (!flag_16_1)
                    break;
            }

            int source_17;
            int dest_39;
            dest_39 = memcpy(dest_37 - 1, dest_35 - entity_42, entity_42 & 0xfffffffc);
            memcpy(dest_39, source_17, entity_42 & 3);
            char* dest_40 = context_1;
            int j_3 = 0xffffffff;

            while (j_3)
            {
                bool flag_17_1 = 0 != *dest_40;
                dest_40 = &dest_40[1];
                j_3 -= 1;

                if (!flag_17_1)
                    break;
            }

            int entity_47 = ~j_3;
            void* dest_42 = &item_348;
            int j_4 = 0xffffffff;

            while (j_4)
            {
                bool flag_18_1 = 0 != *dest_42;
                dest_42 += 1;
                j_4 -= 1;

                if (!flag_18_1)
                    break;
            }

            int source_19;
            int dest_44;
            dest_44 = memcpy(dest_42 - 1, dest_40 - entity_47, entity_47 & 0xfffffffc);
            memcpy(dest_44, source_19, entity_47 & 3);
            void* dest_45 = &item_218;
            int j_5 = 0xffffffff;

            while (j_5)
            {
                bool flag_19_1 = 0 != *dest_45;
                dest_45 += 1;
                j_5 -= 1;

                if (!flag_19_1)
                    break;
            }

            int entity_52 = ~j_5;
            void* dest_47 = &item_348;
            int j_6 = 0xffffffff;

            while (j_6)
            {
                bool flag_20_1 = 0 != *dest_47;
                dest_47 += 1;
                j_6 -= 1;

                if (!flag_20_1)
                    break;
            }

            int source_21;
            int dest_49;
            dest_49 = memcpy(dest_47 - 1, dest_45 - entity_52, entity_52 & 0xfffffffc);
            DWORD nFileSizeLow_1 = findFileData.nFileSizeLow;
            memcpy(dest_49, source_21, entity_52 & 3);
            unsigned char item_34c_2 = findFileData.dwFileAttributes >> 4 & 1;

            if (findFileData.cFileName[0] != 0x2e)
                arg3(&item_354, arg4);

            i_13 = FindNextFileA(hFindFile, &findFileData);
        } while (i_13);
    }

    return FindClose(hFindFile);
}
