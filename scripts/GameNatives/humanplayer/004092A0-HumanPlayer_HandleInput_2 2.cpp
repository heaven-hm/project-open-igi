// IGI native provenance: HumanPlayer_HandleInput_2 - 0x004092A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004092A0-HumanPlayer_HandleInput_2.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "HumanPlayer_HandleInput_2.h"
int HumanPlayer_HandleInput_2(void* context, void* arg2)

{
    void* item_8 = arg2;
    int temp_val = *(context + 8);

    if (!(temp_val & 4))
    {
        void* temp_1 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x1d;
        int data_1 = *(temp_1 + 0x74);
        *(context + 0x14) = data_1;
        return data_1(context, arg2);
    }

    if (temp_val & 0x10000)
    {
        void* temp_2 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x2e;
        int data_4 = *(temp_2 + 0xb8);
        *(context + 0x14) = data_4;
        return data_4(context, arg2);
    }

    if (temp_val & 0x20000)
    {
        void* temp_3 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0x2f;
        int data_7 = *(temp_3 + 0xbc);
        *(context + 0x14) = data_7;
        return data_7(context, arg2);
    }

    if (temp_val & 0x40000)
    {
        void* temp_4 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 0xd;
        int data_10 = *(temp_4 + 0x34);
        *(context + 0x14) = data_10;
        return data_10(context, arg2);
    }

    long double fpu7 = *(arg2 + 0x1dc);
    long double f0 = 0.0f;
    fpu7 - f0;
    short result = (fpu7 < f0 ? 1 : 0) << 8 | (FCMP_UO(fpu7, f0) ? 1 : 0) << 0xa
        | (fpu7 == f0 ? 1 : 0) << 0xe;

    if (!(*(&result + 1) & 0x40))
    {
        long double fpu_val_7_1 = *(arg2 + 0x13c);
        long double f1_1 = 0.0f;
        fpu_val_7_1 - f1_1;
        result = (fpu_val_7_1 < f1_1 ? 1 : 0) << 8 | (FCMP_UO(fpu_val_7_1, f1_1) ? 1 : 0) << 0xa
            | (fpu_val_7_1 == f1_1 ? 1 : 0) << 0xe;

        if (*(&result + 1) & 0x40)
        {
            void* temp_5 = *(context + 4);
            *(context + 0xc) = *(context + 0x10);
            *(context + 0x10) = 0x1c;
            int data_13 = *(temp_5 + 0x70);
            *(context + 0x14) = data_13;
            return data_13(context, arg2);
        }
    }

    if (!(*(&temp_val + 1) & 1))
    {
        void* temp_6 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 7;
        int data_16 = *(temp_6 + 0x1c);
        *(context + 0x14) = data_16;
        return data_16(context, arg2);
    }

    if (!(temp_val & 0x40))
    {
        void* temp_7 = *(context + 4);
        *(context + 0xc) = *(context + 0x10);
        *(context + 0x10) = 4;
        int data_19 = *(temp_7 + 0x10);
        *(context + 0x14) = data_19;
        return data_19(context, arg2);
    }

    if (!(*(&temp_val + 1) & 0x20))
        return result;

    void* temp_8 = *(context + 4);
    *(context + 0xc) = *(context + 0x10);
    *(context + 0x10) = 0x18;
    int data_22 = *(temp_8 + 0x60);
    *(context + 0x14) = data_22;
    return data_22(context, arg2);
}
