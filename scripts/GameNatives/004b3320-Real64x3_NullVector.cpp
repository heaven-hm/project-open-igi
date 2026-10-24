// Native: Real64x3_NullVector
// Retail VA: 0x4b3320
// Source: Binary Ninja MCP decompile
// Signature: int32_t(double* arg1)

  int32_t Real64x3_NullVector(double* arg1)

{
    long double x87_r7 = *arg1;
    long double temp0 = 0.0;
    x87_r7 - temp0;

    if (*(&((x87_r7 < temp0 ? 1 : 0) << 8 | (FCMP_UO(x87_r7, temp0) ? 1 : 0) << 0xa
        | (x87_r7 == temp0 ? 1 : 0) << 0xe) + 1) & 0x40)
    {
        long double x87_r7_1 = arg1[1];
        long double temp1_1 = 0.0;
        x87_r7_1 - temp1_1;

        if (*(&((x87_r7_1 < temp1_1 ? 1 : 0) << 8 | (FCMP_UO(x87_r7_1, temp1_1) ? 1 : 0) << 0xa
            | (x87_r7_1 == temp1_1 ? 1 : 0) << 0xe) + 1) & 0x40)
        {
            long double x87_r7_2 = arg1[2];
            long double temp2_1 = 0.0;
            x87_r7_2 - temp2_1;

            if (*(&((x87_r7_2 < temp2_1 ? 1 : 0) << 8 | (FCMP_UO(x87_r7_2, temp2_1) ? 1 : 0) << 0xa
                    | (x87_r7_2 == temp2_1 ? 1 : 0) << 0xe) + 1) & 0x40)
                return 1;
        }
    }

    return 0;
}
