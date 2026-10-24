// Native: Input_Run
// Retail VA: 0x507ec0
// Source: Binary Ninja MCP decompile
// Signature: int16_t*(int32_t* arg1, int32_t arg2)

  int16_t* Input_Run(int32_t* arg1, int32_t arg2)

{
    arg1[6] = arg1[7];
    int32_t eax_1 = *arg1;

    if (eax_1)
        eax_1(arg1, arg2);

    int32_t edx = arg1[7];
    int32_t i = 0;
    arg1[5] = ~arg1[6] & edx;
    int16_t* result = &arg1[8];

    do
    {
        if (!(edx & 1 << (i & 0x1f)))
            *result = 0;
        else
            *result += 1;

        i += 1;
        result = &result[1];
    } while (i < 0x20);

    return result;
}
